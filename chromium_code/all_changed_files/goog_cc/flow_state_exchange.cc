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

#include "absl/strings/match.h"
#include "api/units/data_rate.h"
#include "api/units/timestamp.h"
#include "modules/congestion_controller/goog_cc/fse_flow.h"
#include "modules/congestion_controller/goog_cc/send_side_bandwidth_estimation.h"
#include "rtc_base/logging.h"

namespace webrtc {

void FlowStateExchange::OnFlowUpdated(std::shared_ptr<RateFlow> flow,
                                      DataRate cc_rate,
                                      DataRate desired_rate,
                                      Timestamp at_time) {
  if (flows_.size() > 1) {
    flow->SetDesiredRate(desired_rate);

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
    // TODO: might still be bug related to while check CHECK THAT IT WORKS
    // CORRECTLY WITH DATARATE NOW BTW
    while (total_leftover_rate - aggregate_rate > 1 && sum_priorities > 0) {
      aggregate_rate = 0.0;
      for (const auto& i : flows_) {
        // if the current fse rate is less than desired
        if (i->FseRate() < i->DesiredRate()) {
          double flow_rate =
              total_leftover_rate * i->Priority() / sum_priorities;

          // if the flow can get more than it desires
          if (flow_rate >= i->DesiredRate().bps()) {
            total_leftover_rate -= i->DesiredRate().bps();
            i->SetFseRate(i->DesiredRate());
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
      i->UpdateCc(at_time);
    }
  } else {
    // TODO: this might not be enough, e.g. for cases where two flows
    // are DR- limited then followed by one with DR==infinity
    flow->SetDesiredRate(desired_rate);
    // a. update S_CR
    sum_calculated_rates_ = sum_calculated_rates_ + cc_rate - flow->FseRate();

    flow->SetFseRate(sum_calculated_rates_);

    // c. distribute
    flow->UpdateCc(at_time);
  }
}

void FlowStateExchange::PrintFseGroupState() {
    //TODO: implement with RTC_LOG
}

FlowStateExchange::FlowStateExchange()
    : flow_id_counter_(0), sum_calculated_rates_(DataRate::Zero()) {}

FlowStateExchange::~FlowStateExchange() = default;

std::shared_ptr<RateFlow> FlowStateExchange::Register(
    DataRate initial_bit_rate,
    //TODO: don't need desired rate when registering
    DataRate desired_rate,
    //TODO: don't need priority when registering
    int priority,
    // TODO: maybe pass a callback update-method instead of the object
    SendSideBandwidthEstimation& cc) {
  fse_mutex_.lock();
  
  int flow_id = flow_id_counter_++;
  std::shared_ptr<RateFlow> newFlow = std::make_shared<RateFlow>(
      flow_id, 
      FseConfig::ResolveRateFlowPriority(flow_id), 
      initial_bit_rate, 
      FseConfig::ResolveRateFlowDesiredRate(flow_id), 
      cc);

  RTC_LOG(LS_INFO) << "FSE Registering new flow with id: " << flow_id;

  flows_.insert(newFlow);
  sum_calculated_rates_ += newFlow->FseRate();

  fse_mutex_.unlock();

  return newFlow;
}

void FlowStateExchange::DeRegister(std::shared_ptr<RateFlow> flow) {
  RTC_LOG(LS_INFO) << "deregistering flow with id" << flow->Id();
  fse_mutex_.lock();
  flows_.erase(flow);
  fse_mutex_.unlock();
}

void FlowStateExchange::Update(std::shared_ptr<RateFlow> flow,
                               DataRate cc_rate,
                               DataRate desired_rate,
                               Timestamp at_time) {
  fse_mutex_.lock();

  if (flows_.find(flow) == flows_.end()) {
    RTC_LOG(LS_INFO) << "flow in FSE Update call is not registereded!\n";
    return;
  }

  OnFlowUpdated(flow, cc_rate, desired_rate, at_time);
  fse_mutex_.unlock();
}

FlowStateExchange& FlowStateExchange::Instance() {
  CR_DEFINE_STATIC_LOCAL(FlowStateExchange, s, ());
  return s;
}

}  // namespace webrtc
