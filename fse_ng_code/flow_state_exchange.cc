//
// Created by tobias on 20.04.2021.
//

#include "modules/congestion_controller/goog_cc/flow_state_exchange.h"

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

namespace webrtc {

void FlowStateExchange::OnFlowUpdated(std::shared_ptr<RateFlow> flow,
                                      DataRate cc_rate,
                                      DataRate desired_rate,
                                      Timestamp at_time) {
  if (flows_.size() > 1) {
    // TODO: is it correct to set desired rate here????
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
  std::cout << "sum_calculated_rates: " << sum_calculated_rates_.bps() << "\n";
  for (const auto& i : flows_) {
    std::cout << "flow_id: " << i->Id() << "\n";
    std::cout << "flow fse_rate: " << i->FseRate().bps() << "\n";
    std::cout << "flow desired_rate: " << i->DesiredRate().bps() << "\n";
  }
}

FlowStateExchange::FlowStateExchange()
    : flow_id_counter_(0), sum_calculated_rates_(DataRate::Zero()) {}

FlowStateExchange::~FlowStateExchange() = default;

std::shared_ptr<WindowBasedFlow> FlowStateExchange::RegisterWindowBasedFlow(
    uint32_t initial_cwnd,
    uint64_t last_rtt,
    int priority,
    cricket::UsrsctpTransport& transport) {
  fse_mutex_.lock();

  std::cout << "FSE Registering new window- based flow with addr: "
            << &transport << " initial_cwnd: " << initial_cwnd << "\n";
  // TODO: more stuff will happen in this method
  std::shared_ptr<WindowBasedFlow> new_flow = std::make_shared<WindowBasedFlow>(
      flow_id_counter_++, priority, initial_cwnd, last_rtt, transport);
  cwnd_flows_.insert(new_flow);
  sum_calculated_rates_ += FseFlow::CwndToRate(initial_cwnd, last_rtt);

  fse_mutex_.unlock();

  return new_flow;
}

std::shared_ptr<RateFlow> FlowStateExchange::Register(
    DataRate initial_bit_rate,
    DataRate desired_rate,
    int priority,
    // TODO: maybe pass a callback update-method instead of the object
    SendSideBandwidthEstimation& cc) {
  fse_mutex_.lock();
  std::cout << "FSE Registering new flow with addr: " << &cc << "\n";

  switch (CURRENT_TEST_CASE) {
    case case1:
      break;
    case case2:
      desired_rate =
          flow_id_counter_ == 0 ? DataRate::KilobitsPerSec(1024) : desired_rate;
      break;
    case case3:
      if (flows_.size() == 1) {
        priority = 2;
      }
  }

  std::shared_ptr<RateFlow> newFlow = std::make_shared<RateFlow>(
      flow_id_counter_++, priority, initial_bit_rate, desired_rate, cc);
  flows_.insert(newFlow);
  sum_calculated_rates_ += newFlow->FseRate();

  fse_mutex_.unlock();

  return newFlow;
}

void FlowStateExchange::Unregister(std::shared_ptr<RateFlow> flow) {
  fse_mutex_.lock();
  flows_.erase(flow);
  sum_calculated_rates_ = sum_calculated_rates_ - flow->FseRate();
  fse_mutex_.unlock();
}

void FlowStateExchange::UpdateWindowBasedFlow(
    std::shared_ptr<WindowBasedFlow> flow,
    uint32_t new_cwnd,
    uint64_t last_rtt) {
  std::cout << "TOBIAS UpdateWindowBasedFlow was called\n";
}

void FlowStateExchange::Update(std::shared_ptr<RateFlow> flow,
                               DataRate cc_rate,
                               DataRate desired_rate,
                               Timestamp at_time) {
  fse_mutex_.lock();

  if (flows_.find(flow) == flows_.end()) {
    std::cout << "flow in FSE Update call is not registereded!\n";
    return;
  }

  switch (CURRENT_TEST_CASE) {
    case case1:
      break;
    case case2:
      desired_rate =
          flow->Id() == 0 ? DataRate::KilobitsPerSec(1024) : desired_rate;
      break;
    case case3:
      break;
  }

  OnFlowUpdated(flow, cc_rate, desired_rate, at_time);
  fse_mutex_.unlock();
}

FlowStateExchange& FlowStateExchange::Instance() {
  CR_DEFINE_STATIC_LOCAL(FlowStateExchange, s, ());
  return s;
}

}  // namespace webrtc
