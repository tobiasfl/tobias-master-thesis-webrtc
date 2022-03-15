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
      sum_calculated_rates_(DataRate::Zero()) {
  RTC_LOG(LS_INFO) << "FSE_V2 created";
}

FseV2::~FseV2() = default;

std::shared_ptr<RateFlow> FseV2::RegisterRateFlow(
    DataRate initial_bit_rate,
    std::function<void(DataRate)> update_callback) {
  mutex_.lock();
  
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

  int id = cwnd_flow_id_counter_++;
  std::shared_ptr<ActiveCwndFlow> new_flow =
      std::make_shared<ActiveCwndFlow>(id, 
              FseConfig::Instance().ResolveCwndFlowPriority(id), 
              initial_cwnd, 
              last_rtt,
              update_callback);
  cwnd_flows_.insert(new_flow);

  mutex_.unlock();

  return new_flow;
}

void FseV2::UpdateRateFlow(std::shared_ptr<RateFlow> flow,
                               DataRate new_rate) {
  mutex_.lock();
  RTC_LOG(LS_INFO) 
      << "PLOT_THISFSE" << flow->Id()
      << " new_rate=" << new_rate.kbps();

  OnFlowUpdated(flow, new_rate);

  RTC_LOG(LS_INFO) 
      << "PLOT_THISFSE sum_calculated_rates_=" 
      << sum_calculated_rates_.kbps();

  mutex_.unlock();
}

void FseV2::OnFlowUpdated(std::shared_ptr<RateFlow> flow,
                                      DataRate cc_rate) {
  // a. update S_CR
  sum_calculated_rates_ = sum_calculated_rates_ + cc_rate - flow->FseRate();

  // b. calculate the sum of all priorities and initialize FSE_R
  int sum_priorities = 0;
  for (const auto& i : rate_flows_) {
    sum_priorities += i->Priority();
    i->SetFseRate(DataRate::Zero());
  }

  // c.Distribute S_CR among all flows, ensuring desired rate is not exceeded
  double total_leftover_rate = sum_calculated_rates_.bps();
  double aggregate_rate = 0.0;
  // while there is more rate to distribute
  // TODO: refactor to not use doubles
  while (total_leftover_rate - aggregate_rate > 1 && sum_priorities > 0) {
    aggregate_rate = 0.0;
    for (const auto& i : rate_flows_) {

      DataRate desired_rate_i = i->DesiredRate();
      // if the current fse rate is less than desired
      if (i->FseRate() < desired_rate_i) {
        double flow_rate =
            total_leftover_rate * i->Priority() / sum_priorities;

        // if the flow can get more than it desires
        if (flow_rate >= desired_rate_i.bps()) {
          total_leftover_rate -= desired_rate_i.bps();
          i->SetFseRate(desired_rate_i);
          sum_priorities -= i->Priority();
        } else {
          i->SetFseRate(DataRate::BitsPerSec(flow_rate));
          aggregate_rate += flow_rate;
        }
      }
    }
  }
  // d. distribute FSE_R to all the flows
  for (const auto& i : rate_flows_) {
    i->UpdateCc();
  }

  //Extension to avoid S_CR leftover buildup, make sure S_CR==total allocated rate
  auto fold = [](DataRate acc, std::shared_ptr<RateFlow> x){ return acc + x->FseRate();};
  sum_calculated_rates_ = std::accumulate(rate_flows_.begin(), rate_flows_.end(), DataRate::Zero(), fold);
}

void FseV2::DeRegisterRateFlow(std::shared_ptr<RateFlow> flow) {
  mutex_.lock();
  RTC_LOG(LS_INFO) << "deregistering flow with id" << flow->Id();

  rate_flows_.erase(flow);

  mutex_.unlock();
}



FseV2& FseV2::Instance() {
  CR_DEFINE_STATIC_LOCAL(FseV2, s, ());
  return s;
}

}  // namespace webrtc
