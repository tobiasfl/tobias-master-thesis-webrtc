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
#include "modules/congestion_controller/goog_cc/send_side_bandwidth_estimation.h"

namespace webrtc {

FseFlow::FseFlow(int id,
                 int priority,
                 double fse_rate,
                 double desired_rate,
                 SendSideBandwidthEstimation& flow_cc)
    : id_(id),
      priority_(priority),
      fse_rate_(fse_rate),
      desired_rate_(desired_rate),
      flow_cc_(flow_cc) {}

FseFlow::~FseFlow() = default;

SendSideBandwidthEstimation& FseFlow::GetFlowCc() const {
  return flow_cc_;
}

double FseFlow::FseRate() const {
  return fse_rate_;
}

void FseFlow::SetFseRate(double new_rate) {
  fse_rate_ = new_rate;
}

int FseFlow::Id() const {
  return id_;
}

int FseFlow::Priority() const {
  return priority_;
}

double FseFlow::DesiredRate() const {
  return desired_rate_;
}

void FseFlow::SetDesiredRate(double new_rate) {
  desired_rate_ = new_rate;
}

FseFlowGroup::FseFlowGroup() : sum_calculated_rates_(0.0) {}

FseFlowGroup::~FseFlowGroup() = default;

void FseFlowGroup::AddFlow(std::shared_ptr<FseFlow> flow) {
  flows_.insert(flow);
  sum_calculated_rates_ += flow->FseRate();
}

void FseFlowGroup::RemoveFlow(std::shared_ptr<FseFlow> flow) {
  flows_.erase(flow);
  sum_calculated_rates_ = sum_calculated_rates_ - flow->FseRate();
}

void FseFlowGroup::OnFlowUpdated(std::shared_ptr<FseFlow> flow,
                                 double cc_rate,
                                 double desired_rate,
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
      i->SetFseRate(0.0);
    }

    // c.Distribute S_CR among all flows, ensuring desired rate is not exceeded
    double total_leftover_rate = sum_calculated_rates_;
    double aggregate_rate = 0.0;
    // while there is more rate to distribute
    // TODO: might still be bug related to while check
    while (total_leftover_rate - aggregate_rate > 1 && sum_priorities > 0) {
      aggregate_rate = 0.0;
      for (const auto& i : flows_) {
        // if the current fse rate is less than desired
        if (i->FseRate() < i->DesiredRate()) {
          double flow_rate =
              total_leftover_rate * i->Priority() / sum_priorities;

          // if the flow can get more than it desires
          if (flow_rate >= i->DesiredRate()) {
            total_leftover_rate -= i->DesiredRate();
            i->SetFseRate(i->DesiredRate());
            sum_priorities -= i->Priority();
          } else {
            i->SetFseRate(flow_rate);
            aggregate_rate += flow_rate;
          }
        }
      }
    }
    // d. distribute FSE_R to all the flows
    for (const auto& i : flows_) {
      i->GetFlowCc().FseUpdateTargetBitrate(DataRate::BitsPerSec(i->FseRate()),
                                            at_time);
    }
  } else {
    // TODO: this might not be enough, e.g. for cases where two flows
    // are DR- limited then followed by one with DR==infinity
    flow->SetDesiredRate(desired_rate);
    // a. update S_CR
    sum_calculated_rates_ = sum_calculated_rates_ + cc_rate - flow->FseRate();

    flow->SetFseRate(sum_calculated_rates_);

    // c. distribute
    flow->GetFlowCc().FseUpdateTargetBitrate(
        DataRate::BitsPerSec(flow->FseRate()), at_time);
  }
}

int FseFlowGroup::GroupSize() {
  return flows_.size();
}

void FseFlowGroup::PrintFseGroupState() {
  std::cout << "sum_calculated_rates: " << sum_calculated_rates_ << "\n";
  for (const auto& i : flows_) {
    std::cout << "flow_id: " << i->Id() << "\n";
    std::cout << "flow fse_rate: " << i->FseRate() << "\n";
    std::cout << "flow desired_rate: " << i->DesiredRate() << "\n";
  }
}

FlowStateExchange::FlowStateExchange()
    : flow_group_(std::make_shared<FseFlowGroup>()) {
  flow_id_counter_ = 0;
}

FlowStateExchange::~FlowStateExchange() = default;

std::shared_ptr<FseFlow> FlowStateExchange::Register(
    DataRate initial_bit_rate,
    DataRate desired_rate,
    int priority,
    // TODO: maybe pass a callback update-method instead of the object
    SendSideBandwidthEstimation& cc) {
  fse_mutex_.lock();
  std::cout << "FSE Registering new flow with addr: " << &cc << "\n";

  /* ONLY FOR TESTING CASE 3
  if (flow_group_->GroupSize() == 1) {
    priority = 2;
  }*/

  // ONLY FOR TESTING CASE 2
  desired_rate =
      flow_id_counter_ == 0 ? DataRate::KilobitsPerSec(1024) : desired_rate;
  //

  std::shared_ptr<FseFlow> newFlow =
      std::make_shared<FseFlow>(flow_id_counter_++, priority,
                                initial_bit_rate.bps(), desired_rate.bps(), cc);
  flow_group_->AddFlow(newFlow);

  fse_mutex_.unlock();

  return newFlow;
}

void FlowStateExchange::Unregister(std::shared_ptr<FseFlow> flow) {
  fse_mutex_.lock();
  flow_group_->RemoveFlow(flow);
  fse_mutex_.unlock();
}

void FlowStateExchange::Update(std::shared_ptr<FseFlow> flow,
                               DataRate cc_rate,
                               DataRate desired_rate,
                               Timestamp at_time) {
  fse_mutex_.lock();
  // ONLY FOR TESTING CASE 2
  desired_rate =
      flow->Id() == 0 ? DataRate::KilobitsPerSec(1024) : desired_rate;
  //

  flow_group_->OnFlowUpdated(flow, cc_rate.bps(), desired_rate.bps(), at_time);
  fse_mutex_.unlock();
}

FlowStateExchange& FlowStateExchange::Instance() {
  CR_DEFINE_STATIC_LOCAL(FlowStateExchange, s, ());
  return s;
}

}  // namespace webrtc
