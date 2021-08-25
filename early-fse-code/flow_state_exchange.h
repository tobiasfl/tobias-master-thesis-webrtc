//
// Created by tobias on 20.04.2021.
//

#ifndef MODULES_CONGESTION_CONTROLLER_GOOG_CC_FLOW_STATE_EXCHANGE_H
#define MODULES_CONGESTION_CONTROLLER_GOOG_CC_FLOW_STATE_EXCHANGE_H

#include <memory>
#include <unordered_set>

#include "absl/strings/match.h"
#include "api/units/data_rate.h"
#include "api/units/timestamp.h"
#include "modules/congestion_controller/goog_cc/send_side_bandwidth_estimation.h"
//#include "modules/congestion_controller/goog_cc/fse_flow.h"
//#include "modules/congestion_controller/goog_cc/fse_flow_group.h"

namespace webrtc {

class FseFlow {
 public:
  FseFlow(int priority, DataRate fse_rate, DataRate desired_rate);
  ~FseFlow();
  DataRate GetFseRate() const;
  void SetFseRate(DataRate new_rate);
  DataRate GetDesiredRate() const;
  void SetDesiredRate(DataRate new_rate);
  int GetPriority() const;

 private:
  int priority_;
  DataRate fse_rate_;
  DataRate desired_rate_;
  // SendSideBandwidthEstimation& cc_;
};

class FseFlowGroup {
 public:
  FseFlowGroup();
  ~FseFlowGroup();
  void AddFlow(std::shared_ptr<FseFlow> flow);
  void RemoveFlow(std::shared_ptr<FseFlow> flow);
  void OnFlowUpdated(std::shared_ptr<FseFlow> flow,
                     DataRate cc_rate,
                     DataRate desired_rate,
                     Timestamp at_time);

 private:
  DataRate sum_calculated_rates_;
  std::unordered_set<std::shared_ptr<FseFlow>> flows_;
};

class FlowStateExchange {
 public:
  ~FlowStateExchange();
  static FlowStateExchange& GetInstance();
  // creates a Flow object, assigns to FG and returns the flow id
  std::shared_ptr<FseFlow> Register(DataRate initial_bit_rate_,
                                    absl::optional<DataRate> desired_rate,
                                    int priority,
                                    SendSideBandwidthEstimation& cc);
  void Unregister(std::shared_ptr<FseFlow> flow);
  void Update(std::shared_ptr<FseFlow> flow,
              DataRate cc_rate,
              DataRate desired_rate,
              Timestamp at_time);

 private:
  FlowStateExchange();

  std::shared_ptr<FseFlowGroup> flow_group_;
};

}  // namespace webrtc

#endif  // MODULES_CONGESTION_CONTROLLER_GOOG_CC_CHROMIUM_FLOW_STATE_EXCHANGE_H
