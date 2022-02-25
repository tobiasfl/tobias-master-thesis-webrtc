//
// Created by tobias on 03.10.2021.
//
#include "modules/congestion_controller/goog_cc/fse_ng.h"

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

const base::Feature kUpdateValFinalRate {
  "UpdateValFinalRate", base::FEATURE_DISABLED_BY_DEFAULT
};

FseNg::FseNg()
    : base_rtt_(TimeDelta::PlusInfinity()),
      sum_calculated_rates_(DataRate::Zero()),
      update_call_num(0),
      rate_flow_id_counter_(0),
      cwnd_flow_id_counter_(0),
      update_val_final_rate_(false){
  update_val_final_rate_ = base::FeatureList::IsEnabled(kUpdateValFinalRate);

  RTC_LOG(LS_INFO) 
      << "FseNg created with update_val_final_rate_=" << update_val_final_rate_;
}

std::shared_ptr<FseNgCwndFlow> FseNg::RegisterCwndFlow(
    uint32_t initial_max_cwnd,
    cricket::UsrsctpTransport& transport) {
  fse_mutex_.lock();

  RTC_LOG(LS_INFO) << "FseNgRegisterWindowBasedFlow was called";

  int id = cwnd_flow_id_counter_++;
  std::shared_ptr<FseNgCwndFlow> new_flow =
      std::make_shared<FseNgCwndFlow>(id, 
              FseConfig::Instance().ResolveCwndFlowPriority(id), 
              initial_max_cwnd, transport);
  cwnd_flows_.insert(new_flow);

  fse_mutex_.unlock();

  return new_flow;
}

std::shared_ptr<FseNgRateFlow> FseNg::RegisterRateFlow(
    DataRate initial_rate,
    std::function<void(DataRate)> update_callback) {
  fse_mutex_.lock();

  RTC_LOG(LS_INFO) 
      << "RegisterRateFlow in FseNg with init_rate_kbps: " 
      << initial_rate.kbps();

  int id = rate_flow_id_counter_++;

  std::shared_ptr<FseNgRateFlow> new_flow = std::make_shared<FseNgRateFlow>(
      id, 
      FseConfig::Instance().ResolveRateFlowPriority(id), 
      initial_rate, 
      FseConfig::Instance().ResolveDesiredRate(id),
      update_callback);

  rate_flows_.insert(new_flow);

  sum_calculated_rates_ += initial_rate;
  
  fse_mutex_.unlock();

  return new_flow;
}

void FseNg::RateUpdate(std::shared_ptr<FseNgRateFlow> flow,
                       DataRate new_rate,
                       TimeDelta last_rtt) {
  fse_mutex_.lock();

  update_call_num++;

  flow->SetCurrMaxRate(FseConfig::Instance().ResolveDesiredRate(flow->Id()));

  //If rtt is 0, assume there is no real measurement yet and say it is infinity
  //so that base_rtt is never set to 0
  //This line only relevant if UpdateValFinalRate is enabled
  //since AimdRateControl has a default rtt of 200ms
  last_rtt = last_rtt == TimeDelta::Zero() ? TimeDelta::PlusInfinity() : last_rtt;
 
  int64_t relative_rate_change_bps = new_rate.bps() - flow->FseRate().bps();

  RTC_LOG(LS_INFO) 
      << "PLOT_THISFSE_NG" << flow->Id()
      << " relative_rate_change=" 
      << relative_rate_change_bps/1000
      << " new_rate=" << new_rate.kbps()
      << " max_rate=" << flow->CurrMaxRate().kbps()
      << " last_rtt=" << (last_rtt.IsFinite() ? last_rtt.ms() : 0);

  OnRateFlowUpdate(
          flow, 
          relative_rate_change_bps, 
          new_rate,
          last_rtt);

  RTC_LOG(LS_INFO) 
      << "PLOT_THISFSENG new sum_calculated_rates_=" 
      << sum_calculated_rates_.kbps()
      << " base_rtt_=" << (base_rtt_.IsFinite() ? base_rtt_.ms() : 0);
  
  fse_mutex_.unlock();
}

// Must be called while owning the mutex
void FseNg::OnRateFlowUpdate(std::shared_ptr<FseNgRateFlow> flow,
                             int64_t relative_rate_change_bps,
                             DataRate cc_rate,
                             TimeDelta last_rtt ) {
  
  UpdateSumCalculatedRates(relative_rate_change_bps, cc_rate, flow->FseRate());

  // We are only gonna allocate to SCTP flows if there is a valid base_rtt_
  bool rtt_is_valid = !last_rtt.IsPlusInfinity() || !base_rtt_.IsPlusInfinity();

  //Extension, if we we don't have valid base_rtt yet we are not going to allocate 
  //Bandwidth to any SCTP flows and should only use rate based flow priorities
  int sum_priorities = rtt_is_valid ? SumPriorities() : SumRatePriorities();

  DataRate sum_rtp_rates = UpdateRateFlows(sum_priorities);

  // Extension, we can not allocate to SCTP if there is no valid base_rtt 
  if (rtt_is_valid) {
    base_rtt_ = std::min(base_rtt_, last_rtt);
    
    DataRate sum_cwnd_rates = sum_calculated_rates_ - sum_rtp_rates;
    UpdateCwndFlows(sum_cwnd_rates);
  }
  //TODO: Maybe check if all RTP flows are application limited as well
  //TODO:This one might actually be removable if we make it so updating S_CR handles it
  if (cwnd_flows_.empty() || !rtt_is_valid) {
    //Extension, to make sure there is no extra unallocated bandwidth accumulating
    //when there are no SCTP flows to spend it
    sum_calculated_rates_ = sum_rtp_rates;
  }
}

void FseNg::UpdateSumCalculatedRates(
        int64_t relative_rate_change_bps, 
        DataRate cc_rate,
        DataRate prev_fse_rate) {
  int64_t sum_of_difference = 
      sum_calculated_rates_.bps() + cc_rate.bps() - prev_fse_rate.bps();
  if (cwnd_flows_.empty() || AllRateFlowsApplicationLimited()) {
    sum_calculated_rates_ = DataRate::BitsPerSec(sum_of_difference);
  }
  else {
    sum_calculated_rates_ = 
        DataRate::BitsPerSec(sum_of_difference + relative_rate_change_bps);
  }  
}

DataRate FseNg::UpdateRateFlows(int sum_priorities) {
  DataRate sum_rtp_rates = DataRate::Zero();
  for (const auto& rate_flow : rate_flows_) {
    // We use the max_rate of the corresponding stream, 
    // because they might have different
    // limitations based on quality of the stream, config, etc.
    DataRate fse_rate = std::min((rate_flow->Priority() * sum_calculated_rates_) 
            / sum_priorities, rate_flow->CurrMaxRate());
    rate_flow->UpdateFlow(fse_rate);
  
    sum_rtp_rates += fse_rate;
  }
  return sum_rtp_rates;
}

void FseNg::UpdateCwndFlows(DataRate sum_cwnd_rates) {
 int sum_cwnd_priorities = SumCwndPriorities();

  for(const auto& cwnd_flow : cwnd_flows_) {
    DataRate cwnd_flow_max_rate = (cwnd_flow->Priority() * sum_cwnd_rates) / sum_cwnd_priorities;
    uint32_t flow_max_cwnd = FseFlow::RateToCwnd(base_rtt_, cwnd_flow_max_rate);

    RTC_LOG(LS_INFO) << "PLOT_THIS_SCTP_FSE_RATE_KBPS" << cwnd_flow->Id() 
                     << " rate=" << cwnd_flow_max_rate.kbps();
  
    cwnd_flow->UpdateCc(flow_max_cwnd);
  }  
}

bool FseNg::AllRateFlowsApplicationLimited() const {
  for (const auto& flow : rate_flows_) {
    if (!flow->IsApplicationLimited()) {
      return false;
    }
  }
  return true;
}

void FseNg::DeRegisterWindowBasedFlow(std::shared_ptr<FseNgCwndFlow> flow) {
  fse_mutex_.lock();

  RTC_LOG(LS_INFO) << "DeRegisterWindowBasedFlow was called";
  cwnd_flows_.erase(flow);

  fse_mutex_.unlock();
}

void FseNg::DeRegisterRateFlow(std::shared_ptr<FseNgRateFlow> flow) {
  fse_mutex_.lock();

  RTC_LOG(LS_INFO) << "DeRegisterRateFlow removing rate flow with id:" << flow->Id();

  rate_flows_.erase(flow);

  // If last RTP flow leaves, the window limits of all window based flows are removed
  // Since no congestion control info is available
  if (rate_flows_.empty()){
    for (const auto& cwnd_flow : cwnd_flows_) {
      cwnd_flow->UpdateCc(cwnd_flow->GetInitialMaxCwnd());
    }
  }

  fse_mutex_.unlock();
}

int FseNg::SumPriorities() const {
  int sum_priorities = 0;
  for (const auto& i : rate_flows_) {
    sum_priorities += i->Priority();
  }
  for (const auto& i : cwnd_flows_) {
    sum_priorities += i->Priority();
  }
  return sum_priorities;
}

int FseNg::SumRatePriorities() const {
  int sum_priorities = 0;
  for (const auto& i : rate_flows_) {
    sum_priorities += i->Priority();
  }
  return sum_priorities;
}

int FseNg::SumCwndPriorities() const {
  int sum_priorities = 0;
  for (const auto& i : cwnd_flows_) {
      sum_priorities += i->Priority();
    }
    return sum_priorities;
}

bool FseNg::UpdateValFinalRate() const {
  return update_val_final_rate_;
}

FseNg& FseNg::Instance() {
  CR_DEFINE_STATIC_LOCAL(FseNg, s, ());
  return s;
}

}  // namespace webrtc
