//
// Created by tobias on 20.04.2021.
//

#ifndef MODULES_CONGESTION_CONTROLLER_GOOG_CC_FLOW_STATE_EXCHANGE_H
#define MODULES_CONGESTION_CONTROLLER_GOOG_CC_FLOW_STATE_EXCHANGE_H

#include <memory>
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
  FseFlow(int priority,
          DataRate fse_rate,
          DataRate desired_rate,
          SendSideBandwidthEstimation& flow_cc);
  ~FseFlow();
  DataRate GetFseRate() const;
  void SetFseRate(DataRate new_rate);
  DataRate GetDesiredRate() const;
  void SetDesiredRate(DataRate new_rate);
  int GetPriority() const;
  SendSideBandwidthEstimation& GetFlowCc() const;

 private:
  int priority_;
  DataRate fse_rate_;
  DataRate desired_rate_;
  SendSideBandwidthEstimation& flow_cc_;
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
 protected:
  // making the singletion friend of this class to allow
  // Singleton<FlowStateExchange> to create an object of FlowStateExchange
  // friend class TSingleton<FlowStateExchange>;
  // Standard constructor protected one can't create another object

 public:
  static FlowStateExchange& Instance();

  // creates a Flow object, assigns to FG and returns the flow
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
  ~FlowStateExchange();
  std::shared_ptr<FseFlowGroup> flow_group_;
};

}  // namespace webrtc

#endif  // MODULES_CONGESTION_CONTROLLER_GOOG_CC_CHROMIUM_FLOW_STATE_EXCHANGE_H
