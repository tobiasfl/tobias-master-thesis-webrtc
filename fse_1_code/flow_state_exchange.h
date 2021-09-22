//
// Created by tobias on 20.04.2021.
//

#ifndef MODULES_CONGESTION_CONTROLLER_GOOG_CC_FLOW_STATE_EXCHANGE_H
#define MODULES_CONGESTION_CONTROLLER_GOOG_CC_FLOW_STATE_EXCHANGE_H

#include <memory>
#include <mutex>
#include <unordered_set>

#include "absl/strings/match.h"
#include "absl/types/optional.h"
#include "api/units/data_rate.h"
#include "api/units/timestamp.h"

namespace webrtc {

class SendSideBandwidthEstimation;

#define CR_DEFINE_STATIC_LOCAL(type, name, arguments) \
  static type& name = *new type arguments

class FseFlow {
 public:
  FseFlow(int id,
          int priority,
          double fse_rate,
          double desired_rate,
          SendSideBandwidthEstimation& flow_cc);
  ~FseFlow();
  double FseRate() const;
  void SetFseRate(double new_rate);
  double DesiredRate() const;
  void SetDesiredRate(double new_rate);
  int Id() const;
  int Priority() const;
  SendSideBandwidthEstimation& GetFlowCc() const;

 private:
  int id_;
  int priority_;
  double fse_rate_;
  double desired_rate_;
  SendSideBandwidthEstimation& flow_cc_;
};

class FseFlowGroup {
 public:
  FseFlowGroup();
  ~FseFlowGroup();
  void AddFlow(std::shared_ptr<FseFlow> flow);
  void RemoveFlow(std::shared_ptr<FseFlow> flow);
  void OnFlowUpdated(std::shared_ptr<FseFlow> flow,
                     double cc_rate,
                     double desired_rate,
                     Timestamp at_time);
  int GroupSize();

 private:
  double sum_calculated_rates_;
  std::unordered_set<std::shared_ptr<FseFlow>> flows_;
  void PrintFseGroupState();
};

class FlowStateExchange {
 public:
  static FlowStateExchange& Instance();

  // creates a Flow object, assigns to FG and returns the flow
  std::shared_ptr<FseFlow> Register(DataRate initial_bit_rate_,
                                    DataRate desired_rate,
                                    int priority,
                                    SendSideBandwidthEstimation& cc);
  void Unregister(std::shared_ptr<FseFlow> flow);
  void Update(std::shared_ptr<FseFlow> flow,
              DataRate cc_rate,
              DataRate desired_rate,
              Timestamp at_time);

 private:
  FlowStateExchange();
  ~FlowStateExchange();
  std::shared_ptr<FseFlowGroup> flow_group_;
  std::mutex fse_mutex_;
  int flow_id_counter_;
};

}  // namespace webrtc

#endif  // MODULES_CONGESTION_CONTROLLER_GOOG_CC_CHROMIUM_FLOW_STATE_EXCHANGE_H
