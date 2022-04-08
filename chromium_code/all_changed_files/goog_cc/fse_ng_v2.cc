//
// Created by tobias on 03.10.2021.
//
#include "modules/congestion_controller/goog_cc/fse_ng_v2.h"

#include "base/feature_list.h"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <chrono>
#include <functional>

#include "api/units/data_rate.h"
#include "api/units/time_delta.h"
#include "api/units/timestamp.h"
#include "modules/congestion_controller/goog_cc/fse_flow.h"
#include "modules/congestion_controller/goog_cc/fse_config.h"
#include "rtc_base/diagnostic_logging.h"
#include "rtc_base/logging.h"

namespace cricket {
class UsrsctpTransport;
}

namespace webrtc {

FseNgV2::FseNgV2()
    : base_rtt_(TimeDelta::PlusInfinity()),
      sum_calculated_rates_(DataRate::Zero()),
      cwnd_sum_calculated_rates_(DataRate::Zero()),
      rate_flow_id_counter_(0),
      cwnd_flow_id_counter_(0)
      {}

std::shared_ptr<HybridCwndFlow> FseNgV2::RegisterCwndFlow(
    uint32_t initial_cwnd,
    uint32_t initial_max_cwnd,
    uint64_t initial_rtt_us,
    std::function<void(uint32_t)> update_callback) {
  fse_mutex_.lock();

  UpdateRttValues(TimeDelta::Micros(initial_rtt_us));

  auto new_flow = CreateAndInsertNewCwndFlow(
          initial_max_cwnd, 
          Flow::CwndToRate(initial_cwnd, initial_rtt_us), 
          update_callback);

  if (AllRateFlowsApplicationLimited() || rate_flows_.empty()) {
    CwndUpdateSumCalculatedRates(DataRate::Zero(), Flow::CwndToRate(initial_cwnd, initial_rtt_us));
  }
  
  LogFseState();
  fse_mutex_.unlock();

  return new_flow;
}

std::shared_ptr<HybridCwndFlow> FseNgV2::CreateAndInsertNewCwndFlow(
          uint32_t initial_max_cwnd, 
          DataRate cwnd_as_rate, 
          std::function<void(uint32_t)> update_callback) {

  int id = cwnd_flow_id_counter_++;
    std::shared_ptr<HybridCwndFlow> new_flow =
      std::make_shared<HybridCwndFlow>(id, 
              FseConfig::Instance().ResolveCwndFlowPriority(id), 
              initial_max_cwnd, 
              cwnd_as_rate,
              update_callback);
  cwnd_flows_.insert(new_flow);
  return new_flow;
}

std::shared_ptr<RateFlow> FseNgV2::RegisterRateFlow(
    DataRate initial_rate,
    std::function<void(DataRate)> update_callback) {
  fse_mutex_.lock();

  auto new_flow = CreateAndInsertNewRateFlow(initial_rate, update_callback);

  sum_calculated_rates_ += initial_rate;

  LogFseState();
  fse_mutex_.unlock();

  return new_flow;
}

std::shared_ptr<RateFlow> FseNgV2::CreateAndInsertNewRateFlow(
      DataRate initial_rate,
      std::function<void(DataRate)> update_callback) {
  int id = rate_flow_id_counter_++;
  std::shared_ptr<RateFlow> new_flow = std::make_shared<RateFlow>(
      id, 
      FseConfig::Instance().ResolveRateFlowPriority(id), 
      initial_rate, 
      FseConfig::Instance().ResolveDesiredRate(id),
      update_callback);

  rate_flows_.insert(new_flow);

  return new_flow;
}

void FseNgV2::RateUpdate(std::shared_ptr<RateFlow> flow,
                       DataRate new_rate) {
  fse_mutex_.lock();

  flow->SetDesiredRate(FseConfig::Instance().ResolveDesiredRate(flow->Id()));

  int64_t relative_rate_change_bps = new_rate.bps() - flow->FseRate().bps();

  OnRateFlowUpdate(flow, relative_rate_change_bps, new_rate);
 
  LogFseState();
  fse_mutex_.unlock();
}

uint32_t FseNgV2::CwndFlowUpdate(std::shared_ptr<HybridCwndFlow> flow,
                       uint32_t new_cwnd,
                       uint64_t last_rtt) {
  fse_mutex_.lock();

  UpdateRttValues(TimeDelta::Micros(last_rtt));

  DataRate cwnd_as_rate = Flow::CwndToRate(new_cwnd, last_rtt);
  if (AllRateFlowsApplicationLimited() || rate_flows_.empty()) {
    CwndUpdateSumCalculatedRates(flow->GetPrevCwnd(), cwnd_as_rate);
  }

  flow->SetPrevCwnd(cwnd_as_rate);

  LogFseState();
  fse_mutex_.unlock();
  return 0;
}


// Must be called while owning the mutex
void FseNgV2::OnRateFlowUpdate(std::shared_ptr<RateFlow> flow,
                             int64_t relative_rate_change_bps,
                             DataRate cc_rate) {
  
  UpdateSumCalculatedRates(relative_rate_change_bps, cc_rate, flow->FseRate());

  int sum_priorities = SumPriorities();
  DataRate sum_rtp_rates = UpdateRateFlows(sum_priorities);

  DataRate sum_cwnd_rates = sum_calculated_rates_ - sum_rtp_rates;
  UpdateCwndFlows(sum_cwnd_rates);

  if (cwnd_flows_.empty() ) {
    //Extension, to make sure there is no extra unallocated bandwidth accumulating
    //when there are no SCTP flows to spend it
    sum_calculated_rates_ = sum_rtp_rates;
  }
}

void FseNgV2::UpdateSumCalculatedRates(
        int64_t relative_rate_change_bps, 
        DataRate cc_rate,
        DataRate prev_fse_rate) {
  int64_t sum_of_difference = 
      sum_calculated_rates_.bps() + cc_rate.bps() - prev_fse_rate.bps();
  //if (cwnd_flows_.empty() || AllRateFlowsApplicationLimited()) {
    sum_calculated_rates_ = DataRate::BitsPerSec(sum_of_difference);
  /*}
  else {
    sum_calculated_rates_ = 
        DataRate::BitsPerSec(sum_of_difference + relative_rate_change_bps);
  }  */
}


void FseNgV2::CwndUpdateSumCalculatedRates(DataRate prev_rate, DataRate new_rate) {
  int64_t sum_of_difference = 
        sum_calculated_rates_.bps() + new_rate.bps() - prev_rate.bps();
      sum_calculated_rates_ = DataRate::BitsPerSec(sum_of_difference);
}

DataRate FseNgV2::UpdateRateFlows(int sum_priorities) {
  DataRate sum_rtp_rates = DataRate::Zero();
  for (const auto& rate_flow : rate_flows_) {
    DataRate fse_rate = std::min((rate_flow->Priority() * sum_calculated_rates_) 
            / sum_priorities, rate_flow->DesiredRate());
    rate_flow->SetFseRate(fse_rate);
    RTC_LOG(LS_INFO) << "PLOT_THIS_RTP_FSE_RATE_KBPS" << rate_flow->Id() 
                     << " rate=" << rate_flow->FseRate().kbps();
 

    rate_flow->UpdateCc();
  
    sum_rtp_rates += fse_rate;
  }
  return sum_rtp_rates;
}

void FseNgV2::UpdateCwndFlows(DataRate sum_cwnd_rates) {
 int sum_cwnd_priorities = SumCwndPriorities();

  for(const auto& cwnd_flow : cwnd_flows_) {
    DataRate cwnd_flow_max_rate = (cwnd_flow->Priority() * sum_cwnd_rates) / sum_cwnd_priorities;
    uint32_t flow_max_cwnd = Flow::RateToCwnd(base_rtt_, cwnd_flow_max_rate);

    RTC_LOG(LS_INFO) << "PLOT_THIS_SCTP_FSE_RATE_KBPS" << cwnd_flow->Id() 
                     << " rate=" << cwnd_flow_max_rate.kbps();
 
    //Hacky way to avoid deadlock
    fse_mutex_.unlock();
    cwnd_flow->UpdateCcMaxCwnd(flow_max_cwnd);
    fse_mutex_.lock();
  }  
}

bool FseNgV2::AllRateFlowsApplicationLimited() const {
  for (const auto& flow : rate_flows_) {
    if (!flow->IsApplicationLimited()) {
      return false;
    }
  }
  return true;
}

void FseNgV2::DeRegisterWindowBasedFlow(std::shared_ptr<HybridCwndFlow> flow) {
  fse_mutex_.lock();

  RTC_LOG(LS_INFO) << "DeRegisterWindowBasedFlow was called";
  cwnd_flows_.erase(flow);

  fse_mutex_.unlock();
}

void FseNgV2::DeRegisterRateFlow(std::shared_ptr<RateFlow> flow) {
  fse_mutex_.lock();

  RTC_LOG(LS_INFO) << "DeRegisterRateFlow removing rate flow with id:" << flow->Id();

  rate_flows_.erase(flow);

  // If last RTP flow leaves, the window limits of all window based flows are removed
  // Since no congestion control info is available
  if (rate_flows_.empty()){
    for (const auto& cwnd_flow : cwnd_flows_) {
      cwnd_flow->UpdateCcMaxCwnd(cwnd_flow->GetInitialMaxCwnd());
    }
  }

  fse_mutex_.unlock();
}

void FseNgV2::UpdateRttValues(TimeDelta last_rtt) {
  //If rtt is 0, assume there is no real measurement yet and say it is infinity
  //so that base_rtt is never set to 0
  last_rtt = last_rtt == TimeDelta::Zero() ? TimeDelta::PlusInfinity() : last_rtt;
  base_rtt_ = std::min(base_rtt_, last_rtt);
}

int FseNgV2::SumPriorities() const {
  int sum_priorities = 0;
  for (const auto& i : rate_flows_) {
    sum_priorities += i->Priority();
  }
  for (const auto& i : cwnd_flows_) {
    sum_priorities += i->Priority();
  }
  return sum_priorities;
}

int FseNgV2::SumRatePriorities() const {
  int sum_priorities = 0;
  for (const auto& i : rate_flows_) {
    sum_priorities += i->Priority();
  }
  return sum_priorities;
}

int FseNgV2::SumCwndPriorities() const {
  int sum_priorities = 0;
  for (const auto& i : cwnd_flows_) {
      sum_priorities += i->Priority();
    }
    return sum_priorities;
}

bool FseNgV2::UseCwndBasedSumCalculatedRates() const {
  return rate_flows_.empty() && !cwnd_flows_.empty();
}



void FseNgV2::LogFseState() const {
  RTC_LOG(LS_INFO) 
      << "PLOT_THISFSENG sum_calculated_rates_=" 
      << sum_calculated_rates_.kbps()
      << " base_rtt_=" << (base_rtt_.IsFinite() ? base_rtt_.ms() : 0);
}

FseNgV2& FseNgV2::Instance() {
  CR_DEFINE_STATIC_LOCAL(FseNgV2, s, ());
  return s;
}

}  // namespace webrtc
