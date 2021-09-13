//
// Created by tobias on 20.04.2021.
//

#include "modules/congestion_controller/goog_cc/flow_state_exchange.h"

#include <stdint.h>

#include <iostream>
#include <memory>
#include <mutex>
#include <unordered_set>

#include "absl/strings/match.h"
#include "api/units/data_rate.h"
#include "api/units/timestamp.h"
#include "modules/congestion_controller/goog_cc/send_side_bandwidth_estimation.h"

namespace webrtc {

FseFlow::FseFlow(int priority,
                 DataRate fse_rate,
                 DataRate desired_rate,
                 SendSideBandwidthEstimation& flow_cc)
    : priority_(priority),
      fse_rate_(fse_rate),
      desired_rate_(desired_rate),
      flow_cc_(flow_cc) {}

FseFlow::~FseFlow() = default;

SendSideBandwidthEstimation& FseFlow::GetFlowCc() const {
  return flow_cc_;
}

DataRate FseFlow::FseRate() const {
  return fse_rate_;
}

void FseFlow::SetFseRate(DataRate new_rate) {
  fse_rate_ = new_rate;
}

int FseFlow::Priority() const {
  return priority_;
}

DataRate FseFlow::DesiredRate() const {
  return desired_rate_;
}

void FseFlow::SetDesiredRate(DataRate new_rate) {
  desired_rate_ = new_rate;
}

FseFlowGroup::FseFlowGroup() : sum_calculated_rates_(DataRate::Zero()) {}

FseFlowGroup::~FseFlowGroup() = default;

void FseFlowGroup::AddFlow(std::shared_ptr<FseFlow> flow) {
  flows_.insert(flow);
  sum_calculated_rates_ = sum_calculated_rates_ + flow->FseRate();
}

void FseFlowGroup::RemoveFlow(std::shared_ptr<FseFlow> flow) {
  flows_.erase(flow);
  sum_calculated_rates_ = sum_calculated_rates_ - flow->FseRate();
}

void FseFlowGroup::OnFlowUpdated(std::shared_ptr<FseFlow> flow,
                                 DataRate cc_rate,
                                 DataRate desired_rate,
                                 Timestamp at_time) {
  std::cout << "\nFSE update call, flow_cc: " << &flow->GetFlowCc()
            << "\ncc_rate: " << cc_rate.kbps()
            << "\ndesired_rate: " << desired_rate.kbps()
            << "\nat_time: " << at_time.ms();

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
  DataRate total_leftover_rate = sum_calculated_rates_;
  DataRate aggregate_rate = DataRate::Zero();
  // while there is more rate to distribute
  while (total_leftover_rate - aggregate_rate > DataRate::Zero() &&
         sum_priorities > 0) {
    aggregate_rate = DataRate::Zero();
    for (const auto& i : flows_) {
      // if the current fse rate is less than desired
      if (i->FseRate() < i->DesiredRate()) {
        // if the flow can get more than it desires
        if ((total_leftover_rate.bps() * i->Priority()) / sum_priorities >=
            i->DesiredRate().bps()) {
          total_leftover_rate -= i->DesiredRate();
          i->SetFseRate(i->DesiredRate());
          sum_priorities -= i->Priority();
          // otherwise
        } else {
          i->SetFseRate(DataRate::BitsPerSec(
              (total_leftover_rate.bps() * i->Priority()) / sum_priorities));
          aggregate_rate += DataRate::BitsPerSec(
              (total_leftover_rate.bps() * i->Priority()) / sum_priorities);
        }
      }
    }
  }
  // d. distribute FSE_R to all the flows
  for (const auto& i : flows_) {
    i->GetFlowCc().FseUpdateTargetBitrate(i->FseRate(), at_time);
  }
}

int FseFlowGroup::GroupSize() {
  return flows_.size();
}

FlowStateExchange::FlowStateExchange()
    : flow_group_(std::make_shared<FseFlowGroup>()) {}

FlowStateExchange::~FlowStateExchange() = default;

std::shared_ptr<FseFlow> FlowStateExchange::Register(
    DataRate initial_bit_rate,
    absl::optional<DataRate> desired_rate,
    int priority,
    // TODO: maybe pass a callback update-method instead of the object
    SendSideBandwidthEstimation& cc) {
  fse_mutex_.lock();

  std::cout << "FSE Registering new flow with addr: " << &cc << "\n";

  // ONLY FOR TESTING PRIOTITIES
  if (flow_group_->GroupSize() == 1) {
    // priority = 2;
  }
  // ONLY FOR TESTING PRIOTITIES

  std::shared_ptr<FseFlow> newFlow;
  if (!desired_rate) {
    newFlow = std::make_shared<FseFlow>(priority, initial_bit_rate,
                                        initial_bit_rate, cc);
    flow_group_->AddFlow(newFlow);
  } else {
    newFlow = std::make_shared<FseFlow>(priority, initial_bit_rate,
                                        *desired_rate, cc);
    flow_group_->AddFlow(newFlow);
  }

  fse_mutex_.unlock();

  return newFlow;
}

void FlowStateExchange::Unregister(std::shared_ptr<FseFlow> flow) {
  flow_group_->RemoveFlow(flow);
}

void FlowStateExchange::Update(std::shared_ptr<FseFlow> flow,
                               DataRate cc_rate,
                               DataRate desired_rate,
                               Timestamp at_time) {
  fse_mutex_.lock();
  flow_group_->OnFlowUpdated(flow, cc_rate, desired_rate, at_time);
  fse_mutex_.unlock();
}

FlowStateExchange& FlowStateExchange::Instance() {
  CR_DEFINE_STATIC_LOCAL(FlowStateExchange, s, ());
  return s;
}

}  // namespace webrtc
