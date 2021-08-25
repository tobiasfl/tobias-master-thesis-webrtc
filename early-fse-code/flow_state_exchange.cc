//
// Created by tobias on 20.04.2021.
//

#include "modules/congestion_controller/goog_cc/flow_state_exchange.h"

#include <stdint.h>

#include <memory>
#include <unordered_set>

#include "absl/strings/match.h"
#include "api/units/data_rate.h"
#include "api/units/timestamp.h"
#include "modules/congestion_controller/goog_cc/send_side_bandwidth_estimation.h"

namespace webrtc {

FseFlow::FseFlow(int priority, DataRate fse_rate, DataRate desired_rate)
    : priority_(priority), fse_rate_(fse_rate), desired_rate_(desired_rate) {}

FseFlow::~FseFlow() {}

DataRate FseFlow::GetFseRate() const {
  return fse_rate_;
}

void FseFlow::SetFseRate(DataRate new_rate) {
  fse_rate_ = new_rate;
}

int FseFlow::GetPriority() const {
  return priority_;
}

DataRate FseFlow::GetDesiredRate() const {
  return desired_rate_;
}

void FseFlow::SetDesiredRate(DataRate new_rate) {
  desired_rate_ = new_rate;
}

FseFlowGroup::FseFlowGroup() : sum_calculated_rates_(DataRate::Zero()) {}

FseFlowGroup::~FseFlowGroup() {}

void FseFlowGroup::AddFlow(std::shared_ptr<FseFlow> flow) {
  flows_.insert(flow);  // TODO: check if this is correct
  sum_calculated_rates_ = sum_calculated_rates_ + flow->GetFseRate();
}

void FseFlowGroup::RemoveFlow(std::shared_ptr<FseFlow> flow) {
  flows_.erase(flow);
  sum_calculated_rates_ = sum_calculated_rates_ - flow->GetFseRate();
}

void FseFlowGroup::OnFlowUpdated(std::shared_ptr<FseFlow> flow,
                                 DataRate cc_rate,
                                 DataRate desired_rate,
                                 Timestamp at_time) {
  // a. update S_CR
  sum_calculated_rates_ = sum_calculated_rates_ + cc_rate - flow->GetFseRate();
  // b. calculate the sum of all priorities and initialize FSE_R
  int sum_priorities = 0;
  for (auto i = flows_.begin(); i != flows_.end(); ++i) {
    sum_priorities += (*i)->GetPriority();
    (*i)->SetFseRate(DataRate::Zero());
  }
  // c.Distribute S_CR among all flows, ensuring desired rate is not exceeded
  DataRate total_leftover_rate = sum_calculated_rates_;
  DataRate aggregate_rate = DataRate::Zero();  // TODO: What should this value
                                               // really be in the beginning?
  while (total_leftover_rate - aggregate_rate > DataRate::Zero() &&
         sum_priorities > 0) {
    aggregate_rate = DataRate::Zero();
    for (auto i = flows_.begin(); i != flows_.end(); ++i) {
      if ((*i)->GetFseRate() < (*i)->GetDesiredRate()) {
        if (total_leftover_rate.kbps() * ((*i)->GetPriority()) /
                sum_priorities >=
            (*i)->GetDesiredRate().kbps()) {
          total_leftover_rate -= (*i)->GetDesiredRate();
          (*i)->SetFseRate((*i)->GetDesiredRate());
          sum_priorities -= (*i)->GetPriority();
        } else {
          (*i)->SetFseRate(DataRate::KilobitsPerSec(total_leftover_rate.kbps() *
                                                    ((*i)->GetPriority()) /
                                                    sum_priorities));
          aggregate_rate = DataRate::KilobitsPerSec(aggregate_rate.kbps() +
                                                    total_leftover_rate.kbps() *
                                                        ((*i)->GetPriority()) /
                                                        sum_priorities);
        }
      }
    }
  }
  // d. distribute FSE_R to all the flows
  for (auto i = flows_.begin(); i != flows_.end(); ++i) {
    // TODO: how to send it????
  }
}

FlowStateExchange::FlowStateExchange()
    : flow_group_(std::make_shared<FseFlowGroup>()) {}

FlowStateExchange::~FlowStateExchange() {}

FlowStateExchange& FlowStateExchange::GetInstance() {
  static FlowStateExchange instance;
  return instance;
}

std::shared_ptr<FseFlow> FlowStateExchange::Register(
    DataRate initial_bit_rate,
    absl::optional<DataRate> desired_rate,
    int priority,
    SendSideBandwidthEstimation& cc) {
  if (!desired_rate) {
    std::shared_ptr<FseFlow> flow = std::make_shared<FseFlow>(
        priority, initial_bit_rate, initial_bit_rate, cc);
    flow_group_->AddFlow(flow);
    return flow;
  } else {
    std::shared_ptr<FseFlow> flow = std::make_shared<FseFlow>(
        priority, initial_bit_rate, *desired_rate, cc);
    flow_group_->AddFlow(flow);
    return flow;
  }
}

void FlowStateExchange::Unregister(std::shared_ptr<FseFlow> flow) {
  flow_group_->RemoveFlow(flow);
}

void FlowStateExchange::Update(std::shared_ptr<FseFlow> flow,
                               DataRate cc_rate,
                               DataRate desired_rate,
                               Timestamp at_time) {
  flow_group_->OnFlowUpdated(flow, cc_rate, desired_rate, at_time);
}

}  // namespace webrtc
