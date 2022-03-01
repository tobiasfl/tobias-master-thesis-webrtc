//
// Created by tobias on 20.04.2021.
//

#include "modules/congestion_controller/goog_cc/flow_state_exchange.h"
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
#include "modules/congestion_controller/goog_cc/send_side_bandwidth_estimation.h"
#include "rtc_base/logging.h"

namespace webrtc {

FlowStateExchange::FlowStateExchange()
    : flow_id_counter_(0), 
      sum_calculated_rates_(DataRate::Zero()) {
  RTC_LOG(LS_INFO) << "FSE created";
}

FlowStateExchange::~FlowStateExchange() = default;

std::shared_ptr<RateFlow> FlowStateExchange::Register(
    DataRate initial_bit_rate,
    std::function<void(DataRate)> update_callback) {
  fse_mutex_.lock();
  
  int flow_id = flow_id_counter_++;
  std::shared_ptr<RateFlow> newFlow = std::make_shared<RateFlow>(
      flow_id, 
      FseConfig::Instance().ResolveRateFlowPriority(flow_id), 
      initial_bit_rate, 
      FseConfig::Instance().ResolveDesiredRate(flow_id), 
      update_callback);

  RTC_LOG(LS_INFO) 
      << "FSE Registering new flow with id: " 
      << flow_id;

  flows_.insert(newFlow);
  sum_calculated_rates_ += newFlow->FseRate();
  RTC_LOG(LS_INFO)
      << "PLOT_THISFSE after registration sum_calculated_rates_="
      << sum_calculated_rates_.kbps();


  fse_mutex_.unlock();

  return newFlow;
}

void FlowStateExchange::Update(std::shared_ptr<RateFlow> flow,
                               DataRate new_rate) {
  fse_mutex_.lock();
   RTC_LOG(LS_INFO) 
       << "PLOT_THISFSE" << flow->Id()
       << " new_rate=" << new_rate.kbps();

  OnFlowUpdated(flow, new_rate);

  RTC_LOG(LS_INFO) 
      << "PLOT_THISFSE sum_calculated_rates_=" 
      << sum_calculated_rates_.kbps();

  fse_mutex_.unlock();
}

void FlowStateExchange::OnFlowUpdated(std::shared_ptr<RateFlow> flow,
                                      DataRate cc_rate) {
  // a. update S_CR
  sum_calculated_rates_ = sum_calculated_rates_ + cc_rate - flow->FseRate();

  // b. calculate the sum of all priorities and initialize FSE_R
  int sum_priorities = 0;
  for (const auto& i : flows_) {
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
    for (const auto& i : flows_) {

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
  for (const auto& i : flows_) {
    i->UpdateCc();
  }

  //Extension to avoid S_CR leftover buildup, make sure S_CR==total allocated rate
  auto fold = [](DataRate acc, std::shared_ptr<RateFlow> x){ return acc + x->FseRate();};
  sum_calculated_rates_ = std::accumulate(flows_.begin(), flows_.end(), DataRate::Zero(), fold);
}

void FlowStateExchange::DeRegister(std::shared_ptr<RateFlow> flow) {
  fse_mutex_.lock();
  RTC_LOG(LS_INFO) << "deregistering flow with id" << flow->Id();

  flows_.erase(flow);

  fse_mutex_.unlock();
}



FlowStateExchange& FlowStateExchange::Instance() {
  CR_DEFINE_STATIC_LOCAL(FlowStateExchange, s, ());
  return s;
}

}  // namespace webrtc
