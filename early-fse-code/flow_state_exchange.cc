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
#include "rtc_base/logging.h"

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

FseFlowGroup::~FseFlowGroup() = default;

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
  // TODO: is it correct to set desired rate here????
  flow->SetDesiredRate(desired_rate);

  // a. update S_CR
  sum_calculated_rates_ = sum_calculated_rates_ + cc_rate - flow->GetFseRate();

  // b. calculate the sum of all priorities and initialize FSE_R
  int sum_priorities = 0;
  for (const auto& i : flows_) {
    sum_priorities += i->GetPriority();
    i->SetFseRate(DataRate::Zero());
  }

  // c.Distribute S_CR among all flows, ensuring desired rate is not exceeded
  DataRate total_leftover_rate =
      DataRate::BitsPerSec(sum_calculated_rates_.bps());
  DataRate aggregate_rate = DataRate::Zero();

  while (total_leftover_rate - aggregate_rate > DataRate::Zero() &&
         sum_priorities > 0) {
    aggregate_rate = DataRate::Zero();

    for (const auto& i : flows_) {
      if (i->GetFseRate() < i->GetDesiredRate()) {
        if (total_leftover_rate.bps() * (i->GetPriority()) / sum_priorities >=
            i->GetDesiredRate().bps()) {
          total_leftover_rate -= i->GetDesiredRate();
          i->SetFseRate(i->GetDesiredRate());
          sum_priorities -= i->GetPriority();
        } else {
          i->SetFseRate(DataRate::BitsPerSec(
              total_leftover_rate.bps() * (i->GetPriority()) / sum_priorities));
          aggregate_rate = DataRate::BitsPerSec(
              aggregate_rate.bps() +
              (total_leftover_rate.bps() * i->GetPriority()) / sum_priorities);
        }
      }
    }
  }
  // d. distribute FSE_R to all the flows
  for (const auto& i : flows_) {
    i->GetFlowCc().FseUpdateTargetBitrate(i->GetFseRate(), at_time);
  }
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
  RTC_LOG(LS_INFO) << "FSE Registering new flow";
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

FlowStateExchange& FlowStateExchange::Instance() {
  CR_DEFINE_STATIC_LOCAL(FlowStateExchange, s, ());
  return s;
}

}  // namespace webrtc
