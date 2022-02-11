//
// Created by tobias on 20.04.2021.
//

#ifndef MODULES_CONGESTION_CONTROLLER_GOOG_CC_FLOW_STATE_EXCHANGE_H
#define MODULES_CONGESTION_CONTROLLER_GOOG_CC_FLOW_STATE_EXCHANGE_H

#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_set>

#include "absl/strings/match.h"
#include "absl/types/optional.h"
#include "api/units/data_rate.h"
#include "api/units/timestamp.h"

namespace cricket {

class UsrsctpTransport;
}

namespace webrtc {

class SendSideBandwidthEstimation;
class FseFlow;
class RateFlow;

#define CR_DEFINE_STATIC_LOCAL(type, name, arguments) \
  static type& name = *new type arguments

class FlowStateExchange {
 public:
  static FlowStateExchange& Instance();

  // creates a Flow object, assigns to FG and returns the flow
  std::shared_ptr<RateFlow> Register(DataRate initial_bit_rate,
                                     DataRate desired_rate,
                                     int priority,
                                     SendSideBandwidthEstimation& cc);
  void DeRegister(std::shared_ptr<RateFlow> flow);
  void Update(std::shared_ptr<RateFlow> flow,
              DataRate cc_rate,
              DataRate desired_rate,
              Timestamp at_time);

 private:
  FlowStateExchange();
  ~FlowStateExchange();
  std::mutex fse_mutex_;
  int flow_id_counter_;
  DataRate sum_calculated_rates_;
  std::unordered_set<std::shared_ptr<RateFlow>> flows_;

  void PrintFseGroupState();
  void OnFlowUpdated(std::shared_ptr<RateFlow> flow,
                     DataRate cc_rate,
                     DataRate desired_rate,
                     Timestamp at_time);
};

}  // namespace webrtc

#endif  // MODULES_CONGESTION_CONTROLLER_GOOG_CC_CHROMIUM_FLOW_STATE_EXCHANGE_H
