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

namespace webrtc {

FseV2::FseV2()
    : rate_flow_id_counter_(0), 
      cwnd_flow_id_counter_(0),
      sum_calculated_rates_(DataRate::Zero()),
      base_rtt_(TimeDelta::PlusInfinity()){
  RTC_LOG(LS_INFO) << "FSE_V2 created";
}

FseV2::~FseV2() = default;

std::shared_ptr<RateFlow> FseV2::RegisterRateFlow(
    DataRate initial_bit_rate,
    std::function<void(DataRate)> update_callback) {
  RTC_LOG(LS_INFO) << "trying to lock mutex";
  mutex_.lock();
  RTC_LOG(LS_INFO) << "locked mutex";
  
  int flow_id = rate_flow_id_counter_++;
  std::shared_ptr<RateFlow> newFlow = std::make_shared<RateFlow>(
      flow_id, 
      FseConfig::Instance().ResolveRateFlowPriority(flow_id), 
      initial_bit_rate, 
      FseConfig::Instance().ResolveDesiredRate(flow_id), 
      update_callback);

  RTC_LOG(LS_INFO) 
      << "FSE Registering new flow with id: " 
      << flow_id;

  rate_flows_.insert(newFlow);
  sum_calculated_rates_ += newFlow->FseRate();
  RTC_LOG(LS_INFO)
      << "PLOT_THISFSE_V2 after registration sum_calculated_rates_="
      << sum_calculated_rates_.kbps();


  mutex_.unlock();
  RTC_LOG(LS_INFO) << "unlocked mutex";

  return newFlow;
}

std::shared_ptr<ActiveCwndFlow> FseV2::RegisterCwndFlow(
      uint32_t initial_cwnd,
      uint64_t last_rtt,
      std::function<void(uint32_t)> update_callback) {
  RTC_LOG(LS_INFO) << "trying to lock mutex";
  mutex_.lock();
  RTC_LOG(LS_INFO) << "locked mutex";

  RTC_LOG(LS_INFO) 
      << "RegisterCwndFlow was called with initial_cwnd=" 
      << initial_cwnd 
      << " last_rtt=" 
      << last_rtt/1000;

  base_rtt_ = std::min(base_rtt_, TimeDelta::Micros(last_rtt));

  int id = cwnd_flow_id_counter_++;
  std::shared_ptr<ActiveCwndFlow> new_flow =
      std::make_shared<ActiveCwndFlow>(id, 
              FseConfig::Instance().ResolveCwndFlowPriority(id), 
              initial_cwnd, 
              last_rtt,
              update_callback);
  cwnd_flows_.insert(new_flow);

  mutex_.unlock();
  RTC_LOG(LS_INFO) << "unlocked mutex";

  return new_flow;
}

void FseV2::RateFlowUpdate(std::shared_ptr<RateFlow> flow, DataRate new_rate, TimeDelta last_rtt) {
  RTC_LOG(LS_INFO) << "trying to lock mutex";
  mutex_.lock();
  RTC_LOG(LS_INFO) << "locked mutex";
  RTC_LOG(LS_INFO) 
      << "PLOT_THISFSE" << flow->Id()
      << " newrate=" << new_rate.kbps();

  base_rtt_ = std::min(base_rtt_, last_rtt);

  // a. update S_CR
  sum_calculated_rates_ = sum_calculated_rates_ + new_rate - flow->FseRate();

  OnFlowUpdated();

  RTC_LOG(LS_INFO) 
      << "PLOT_THISFSE sum_calculated_rates_=" 
      << sum_calculated_rates_.kbps();

  mutex_.unlock();
  RTC_LOG(LS_INFO) << "unlocked mutex";
}

void FseV2::CwndFlowUpdate(
        std::shared_ptr<ActiveCwndFlow> flow,
        uint32_t new_cwnd,
        uint64_t last_rtt) {
  RTC_LOG(LS_INFO) << "trying to lock mutex";
  mutex_.lock();
  RTC_LOG(LS_INFO) << "locked mutex";

  //TODO: still have to make sure race conditions do not happen in sctp_set_cwnd
  base_rtt_ = std::min(base_rtt_, TimeDelta::Micros(last_rtt));

  //TODO: Need to be sure about which RTT to use in these calculations
  DataRate new_rate = Flow::CwndToRate(new_cwnd, last_rtt);
  DataRate fse_rate = Flow::CwndToRate(flow->FseCwnd(), last_rtt);

  sum_calculated_rates_ = sum_calculated_rates_ + new_rate - fse_rate;
  
  OnFlowUpdated();

  mutex_.unlock();
  RTC_LOG(LS_INFO) << "unlocked mutex";
}


void FseV2::OnFlowUpdated() {

  // b. calculate the sum of all priorities and initialize FSE_R
  int sum_rate_priorities = SumPrioritiesAndInitializeRates();
  int sum_cwnd_priorities = SumPrioritiesAndInitializeCwnds();
  int sum_total_priorities = sum_rate_priorities + sum_cwnd_priorities;

  // c. Distribute S_CR among all flows, ensuring desired rate is not exceeded
  DataRate rate_flows_share = 
      sum_calculated_rates_ * sum_rate_priorities / sum_total_priorities; 
  AllocateToRateFlows(sum_rate_priorities, rate_flows_share);

  DataRate cwnd_flows_share = sum_calculated_rates_ - rate_flows_share;
  AllocateToCwndFlows(sum_cwnd_priorities, cwnd_flows_share);

  // d. Distribute FSE_R to all the flows
  for (const auto& i : rate_flows_) {
    i->UpdateCc();
  }

  for (const auto& i : cwnd_flows_) {
    i->UpdateCc();
  }

  //Extension to avoid S_CR leftover buildup, make sure S_CR==total allocated rate
  DataRate total_alllocated_rate = DataRate::Zero();
  for (const auto& i : rate_flows_) {
    total_alllocated_rate += i->FseRate();
  }

  for (const auto& i : cwnd_flows_) {
    total_alllocated_rate += Flow::CwndToRate(i->FseCwnd(), base_rtt_.us());
  }
  sum_calculated_rates_ = total_alllocated_rate;
}

int FseV2::SumPrioritiesAndInitializeRates() {
  int sum_rate_priorities = 0;
  for (const auto& i : rate_flows_) {
    sum_rate_priorities += i->Priority();
    i->SetFseRate(DataRate::Zero());
  }
  return sum_rate_priorities;
}

int FseV2::SumPrioritiesAndInitializeCwnds() {
  int sum_cwnd_priorities = 0;
  for (const auto& i : cwnd_flows_) {
    sum_cwnd_priorities += i->Priority();
    i->SetFseCwnd(0);
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
        RTC_LOG(LS_INFO) 
        << "PLOT_THISFSE_V2_RTP" << i->Id() 
        << " rate=" << flow_rate.kbps();

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

    RTC_LOG(LS_INFO) 
        << "PLOT_THISFSE_V2_SCTP" 
        << i->Id() 
        << " rate=" 
        << cwnd_flow_rate.kbps();

    uint32_t flow_cwnd = Flow::RateToCwnd(base_rtt_, cwnd_flow_rate);
    i->SetFseCwnd(flow_cwnd);
  }
}

void FseV2::DeRegisterRateFlow(std::shared_ptr<RateFlow> flow) {
  RTC_LOG(LS_INFO) << "trying to lock mutex";
  mutex_.lock();
  RTC_LOG(LS_INFO) << "locked mutex";
  RTC_LOG(LS_INFO) << "deregistering flow with id" << flow->Id();

  rate_flows_.erase(flow);

  mutex_.unlock();
  RTC_LOG(LS_INFO) << "unlocked mutex";
}



FseV2& FseV2::Instance() {
  CR_DEFINE_STATIC_LOCAL(FseV2, s, ());
  return s;
}

}  // namespace webrtc
