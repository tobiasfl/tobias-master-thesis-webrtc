//
// Created by tobias on 15.03.2022.
//

#ifndef MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_V2_H
#define MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_V2_H

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

class RateFlow;
class ActiveCwndFlow;

#define CR_DEFINE_STATIC_LOCAL(type, name, arguments) \
  static type& name = *new type arguments

class FseV2 {
 public:
  static FseV2& Instance();

  std::shared_ptr<RateFlow> RegisterRateFlow(DataRate initial_bit_rate,
                                     std::function<void(DataRate)> update_callback);
  std::shared_ptr<ActiveCwndFlow> RegisterCwndFlow(
      uint32_t initial_cwnd,
      uint64_t last_rtt,
      std::function<void(uint32_t)> update_callback);

  void DeRegisterRateFlow(std::shared_ptr<RateFlow> flow);
  void UpdateRateFlow(std::shared_ptr<RateFlow> flow,
              DataRate new_rate);

 private:
  FseV2();
  ~FseV2();
  std::mutex mutex_;
  int rate_flow_id_counter_;
  int cwnd_flow_id_counter_;
  DataRate sum_calculated_rates_;
  std::unordered_set<std::shared_ptr<RateFlow>> rate_flows_;
  std::unordered_set<std::shared_ptr<ActiveCwndFlow>> cwnd_flows_;

  void OnFlowUpdated(std::shared_ptr<RateFlow> flow,
                     DataRate cc_rate);
};

}  // namespace webrtc

#endif  // MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_V2_H
