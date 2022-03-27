//
// Created by tobias on 15.03.2022.
//

#include "modules/congestion_controller/goog_cc/fse_v2.h"
#include "modules/congestion_controller/goog_cc/fse_config.h"

#include <stdint.h>

#include <cstdint>
#include <iostream>
#include <memory>
#include <mutex>
#include <unordered_set>
#include <numeric>


#include "absl/strings/match.h"
#include "api/units/data_rate.h"
#include "api/units/timestamp.h"
#include "modules/congestion_controller/goog_cc/fse_flow.h"
#include "rtc_base/logging.h"

#include "system_wrappers/include/clock.h"

namespace webrtc {

FseV2::FseV2()
    : rate_flow_id_counter_(0), 
      cwnd_flow_id_counter_(0),
      sum_calculated_rates_(DataRate::Zero()),
      last_rtt_(TimeDelta::Zero()),
      clock_(Clock::GetRealTimeClock())
    {
  RTC_LOG(LS_INFO) << "FSE_V2 created";
}

FseV2::~FseV2() = default;

std::shared_ptr<GccRateFlow> FseV2::RegisterRateFlow(
      DataRate initial_bit_rate,
      std::function<void(DataRate, Timestamp)> delay_update_callback,
      std::function<void(DataRate, Timestamp)> loss_update_callback) {
  mutex_.lock();
  
  int flow_id = rate_flow_id_counter_++;
  std::shared_ptr<GccRateFlow> newFlow = std::make_shared<GccRateFlow>(
      flow_id, 
      FseConfig::Instance().ResolveRateFlowPriority(flow_id), 
      initial_bit_rate, 
      FseConfig::Instance().ResolveDesiredRate(flow_id), 
      delay_update_callback,
      loss_update_callback);

  RTC_LOG(LS_INFO) 
      << "FSE Registering new flow with id: " 
      << flow_id;

  rate_flows_.insert(newFlow);

  sum_calculated_rates_ += newFlow->FseRate();

  RTC_LOG(LS_INFO)
      << "PLOT_THISFSE after registration sum_calculated_rates_="
      << sum_calculated_rates_.kbps();

  mutex_.unlock();

  return newFlow;
}



std::shared_ptr<ActiveCwndFlow> FseV2::RegisterCwndFlow(
      uint32_t initial_cwnd,
      uint64_t last_rtt,
      std::function<void(uint32_t)> update_callback) {
  mutex_.lock();

  RTC_LOG(LS_INFO) 
      << "RegisterCwndFlow was called with initial_cwnd=" 
      << initial_cwnd 
      << " last_rtt=" 
      << last_rtt/1000;

  UpdateRttValues(TimeDelta::Micros(last_rtt));

  int id = cwnd_flow_id_counter_++;
  std::shared_ptr<ActiveCwndFlow> new_flow =
      std::make_shared<ActiveCwndFlow>(id, 
              FseConfig::Instance().ResolveCwndFlowPriority(id), 
              Flow::CwndToRate(initial_cwnd, last_rtt_.us()), 
              update_callback);


  cwnd_flows_.insert(new_flow);

  sum_calculated_rates_ += Flow::CwndToRate(initial_cwnd, last_rtt_.us());
  RTC_LOG(LS_INFO)
      << "PLOT_THISFSE_CWND after registration sum_calculated_rates_="
      << sum_calculated_rates_.kbps();

  mutex_.unlock();

  return new_flow;
}

void FseV2::RateFlowLossBasedUpdate(std::shared_ptr<GccRateFlow> flow, DataRate new_rate, TimeDelta last_rtt, Timestamp at_time) {
  mutex_.lock();

  RTC_LOG(LS_INFO) 
      << "PLOT_THIS_RTP" << flow->Id()
      << " ratecc=" << new_rate.kbps()
      << " last_rtt=" << last_rtt.ms();

  UpdateRttValues(last_rtt);

  sum_calculated_rates_ = sum_calculated_rates_ + new_rate - flow->FseRate();

  OnFlowUpdated();

  RTC_LOG(LS_INFO) 
      << "PLOT_THISFSE_RATE sum_calculated_rates_=" 
      << sum_calculated_rates_.kbps();

  DistributeToRateFlows(at_time);

  DistributeToCwndFlows(nullptr);

  mutex_.unlock();
}

void FseV2::RateFlowDelayBasedUpdate(std::shared_ptr<GccRateFlow> flow,
              DataRate new_rate,
              TimeDelta last_rtt, 
              Timestamp at_time) {
  mutex_.lock();

  RTC_LOG(LS_INFO) 
      << "PLOT_THIS_RTP" << flow->Id()
      << " ratecc=" << new_rate.kbps()
      << " last_rtt=" << last_rtt.ms();

  UpdateRttValues(last_rtt);

  sum_calculated_rates_ = sum_calculated_rates_ + new_rate - flow->FseRate();

  OnFlowUpdated();

  RTC_LOG(LS_INFO) 
      << "PLOT_THISFSE_RATE sum_calculated_rates_=" 
      << sum_calculated_rates_.kbps();

    // d. Distribute FSE_R to all the flows 
  for (const auto& i : rate_flows_) {
    RTC_LOG(LS_INFO) 
        << "PLOT_THIS_RTP" << i->Id() 
        << " rate=" << i->FseRate().kbps();

    i->UpdateDelayBasedCc(at_time);
  }

  DistributeToCwndFlows(nullptr);

  mutex_.unlock();
}


uint32_t FseV2::CwndFlowUpdate(
        std::shared_ptr<ActiveCwndFlow> flow,
        uint32_t new_cwnd,
        uint64_t last_rtt) {
  mutex_.lock();

  UpdateRttValues(TimeDelta::Micros(last_rtt));

  DataRate new_rate = Flow::CwndToRate(new_cwnd, last_rtt_.us());

  RTC_LOG(LS_INFO) 
      << "PLOT_THIS_SCTP" << flow->Id()
      << " ratecc=" << new_rate.kbps()
      << " last_rtt=" << last_rtt/1000;

  sum_calculated_rates_ = sum_calculated_rates_ + new_rate - flow->FseRate();
  
  OnFlowUpdated();

  RTC_LOG(LS_INFO) 
      << "PLOT_THISFSE_CWND sum_calculated_rates_=" 
      << sum_calculated_rates_.kbps();

  Timestamp at_time = clock_->CurrentTime();
  DistributeToRateFlows(at_time);

  //TODO: make sure this does not make deadlock when there are several SCTP flows
  DistributeToCwndFlows(flow);

  mutex_.unlock();
  return Flow::RateToCwnd(last_rtt_, flow->FseRate());
}


void FseV2::OnFlowUpdated() {

  // b. calculate the sum of all priorities and initialize FSE_R
  int sum_rate_priorities = SumPrioritiesAndInitializeRateFlowRates();
  int sum_cwnd_priorities = SumPrioritiesAndInitializeCwndFlowRates();
  int sum_total_priorities = sum_rate_priorities + sum_cwnd_priorities;

  // c. Distribute S_CR among all flows, ensuring desired rate is not exceeded
  DataRate rate_flows_share = 
      sum_calculated_rates_ * sum_rate_priorities / sum_total_priorities; 
  AllocateToRateFlows(sum_rate_priorities, rate_flows_share);

  DataRate cwnd_flows_share = sum_calculated_rates_ - rate_flows_share;
  AllocateToCwndFlows(sum_cwnd_priorities, cwnd_flows_share);

  //Extension to avoid S_CR leftover buildup, make sure S_CR == total allocated rate
  sum_calculated_rates_ = SumAllocatedRates();
}

int FseV2::SumPrioritiesAndInitializeRateFlowRates() {
  int sum_rate_priorities = 0;
  for (const auto& i : rate_flows_) {
    sum_rate_priorities += i->Priority();
    i->SetFseRate(DataRate::Zero());
  }
  return sum_rate_priorities;
}

int FseV2::SumPrioritiesAndInitializeCwndFlowRates() {
  int sum_cwnd_priorities = 0;
  for (const auto& i : cwnd_flows_) {
    sum_cwnd_priorities += i->Priority();
    i->SetFseRate(DataRate::Zero());
  }
  return sum_cwnd_priorities;
}

//TODO: currently does not leave any extra for SCTP flows if all RTP flows are DR limited
void FseV2::AllocateToRateFlows(int sum_priorities, DataRate leftover_rate) {
  DataRate aggregate_rate = DataRate::Zero();
  // while there is more rate to distribute
  // (we check against 1 because the divison might lead to uneven number)
  while (leftover_rate - aggregate_rate > DataRate::BitsPerSec(1) 
          && sum_priorities > 0) {
    aggregate_rate = DataRate::Zero();
    for (const auto& i : rate_flows_) {

      // if the current fse rate is less than desired
      if (i->FseRate() < i->DesiredRate()) {
        DataRate flow_rate = leftover_rate * i->Priority() / sum_priorities;
        

        // if the flow can get more than it desires
        if (flow_rate >= i->DesiredRate()) {
          leftover_rate -= i->DesiredRate();
          i->SetFseRate(i->DesiredRate());
          sum_priorities -= i->Priority();
        } 
        else {
          i->SetFseRate(flow_rate);
          aggregate_rate += flow_rate;
        }
      }
    }
  }
}

void FseV2::AllocateToCwndFlows(int sum_priorities, DataRate sum_cwnd_rates) {
  for (const auto& i : cwnd_flows_) {
    DataRate cwnd_flow_rate = (i->Priority() * sum_cwnd_rates) / sum_priorities;
    i->SetFseRate(cwnd_flow_rate);
  }
}

DataRate FseV2::SumAllocatedRates() {
  DataRate total_alllocated_rate = DataRate::Zero();
  for (const auto& i : rate_flows_) {
    total_alllocated_rate += i->FseRate();
  }

  for (const auto& i : cwnd_flows_) {
    total_alllocated_rate += i->FseRate();
  }
  return total_alllocated_rate;
}


void FseV2::DistributeToRateFlows(Timestamp at_time) {
  // d. Distribute FSE_R to all the flows 
  for (const auto& i : rate_flows_) {
    RTC_LOG(LS_INFO) 
        << "PLOT_THIS_RTP" << i->Id() 
        << " rate=" << i->FseRate().kbps();

    i->UpdateLossBasedCc(at_time);
  }
}


void FseV2::DistributeToCwndFlows(std::shared_ptr<ActiveCwndFlow> update_caller) {
  for (const auto& i : cwnd_flows_) {
    RTC_LOG(LS_INFO) 
        << "PLOT_THIS_SCTP" << i->Id()  
        << " rate=" << i->FseRate().kbps();
    if (i == update_caller) {
      continue;
    }

    uint32_t new_cwnd = Flow::RateToCwnd(last_rtt_, i->FseRate());
    mutex_.unlock();
    i->UpdateCc(new_cwnd);
    mutex_.lock();
  }
}

void FseV2::DeRegisterRateFlow(std::shared_ptr<GccRateFlow> flow) {
  mutex_.lock();
  RTC_LOG(LS_INFO) << "deregistering flow with id" << flow->Id();

  rate_flows_.erase(flow);

  mutex_.unlock();
}


void FseV2::DeRegisterCwndFlow(std::shared_ptr<ActiveCwndFlow> flow) {
  mutex_.lock();
  RTC_LOG(LS_INFO) << "deregistering flow with id" << flow->Id();

  cwnd_flows_.erase(flow);

  mutex_.unlock();
}


void FseV2::UpdateRttValues(TimeDelta last_rtt) {
  if (last_rtt.IsFinite()) {
    last_rtt_ = last_rtt;
  }
}

bool FseV2::UpdateLossBasedEstimateIsEnabled() const {
  return true;
}

FseV2& FseV2::Instance() {
  CR_DEFINE_STATIC_LOCAL(FseV2, s, ());
  return s;
}

}  // namespace webrtc
