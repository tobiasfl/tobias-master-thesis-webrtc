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
// TODO: include header file or forward- declaration like this?
class FseFlow;
class RateFlow;
class WindowBasedFlow;

#define CR_DEFINE_STATIC_LOCAL(type, name, arguments) \
  static type& name = *new type arguments

#define CURRENT_TEST_CASE case3

enum TestCases { case1, case2, case3 };

class FlowStateExchange {
 public:
  static FlowStateExchange& Instance();

  // creates a Flow object, assigns to FG and returns the flow
  std::shared_ptr<RateFlow> Register(DataRate initial_bit_rate,
                                     DataRate desired_rate,
                                     int priority,
                                     SendSideBandwidthEstimation& cc);
  std::shared_ptr<WindowBasedFlow> RegisterWindowBasedFlow(
      uint32_t initial_cwnd,
      uint64_t last_rtt,
      int priority,
      cricket::UsrsctpTransport& transport);
  void Unregister(std::shared_ptr<RateFlow> flow);
  void Update(std::shared_ptr<RateFlow> flow,
              DataRate cc_rate,
              DataRate desired_rate,
              Timestamp at_time);
  void UpdateWindowBasedFlow(std::shared_ptr<WindowBasedFlow> flow,
                             uint32_t new_cwnd,
                             uint64_t last_rtt);

 private:
  FlowStateExchange();
  ~FlowStateExchange();
  std::mutex fse_mutex_;
  int flow_id_counter_;
  DataRate sum_calculated_rates_;
  std::unordered_set<std::shared_ptr<RateFlow>> flows_;
  std::unordered_set<std::shared_ptr<WindowBasedFlow>> cwnd_flows_;

  void PrintFseGroupState();
  void OnFlowUpdated(std::shared_ptr<RateFlow> flow,
                     DataRate cc_rate,
                     DataRate desired_rate,
                     Timestamp at_time);
};

}  // namespace webrtc

#endif  // MODULES_CONGESTION_CONTROLLER_GOOG_CC_CHROMIUM_FLOW_STATE_EXCHANGE_H
