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

#include "system_wrappers/include/clock.h"

namespace cricket {

class UsrsctpTransport;
}

namespace webrtc {

class RateFlow;
class ActiveCwndFlow;
class GccRateFlow;

#define CR_DEFINE_STATIC_LOCAL(type, name, arguments) \
  static type& name = *new type arguments

class FseV2 {
 public:
  static FseV2& Instance();

  std::shared_ptr<GccRateFlow> RegisterRateFlow(
      DataRate initial_bit_rate,
      std::function<void(DataRate, Timestamp, bool)> update_callback);

  std::shared_ptr<ActiveCwndFlow> RegisterCwndFlow(
      uint32_t initial_cwnd,
      uint64_t last_rtt,
      std::function<void(uint32_t)> update_callback);

  void DeRegisterRateFlow(std::shared_ptr<GccRateFlow> flow);
  void DeRegisterCwndFlow(std::shared_ptr<ActiveCwndFlow> flow);
  void RateFlowUpdate(std::shared_ptr<GccRateFlow> flow,
              DataRate new_rate,
              TimeDelta last_rtt, 
              Timestamp at_time,
              bool update_loss_only);

  uint32_t CwndFlowUpdate(std::shared_ptr<ActiveCwndFlow> flow,
          uint32_t new_cwnd,
          uint64_t last_rtt);

  bool CoupleDcSctpLib();

 private:
  FseV2();
  ~FseV2();

  std::mutex mutex_;

  int rate_flow_id_counter_;
  int cwnd_flow_id_counter_;

  DataRate sum_calculated_rates_;

  std::unordered_set<std::shared_ptr<GccRateFlow>> rate_flows_;
  std::unordered_set<std::shared_ptr<ActiveCwndFlow>> cwnd_flows_;

  TimeDelta last_rtt_;

  Clock* clock_;

  void UpdateRttValues(TimeDelta last_rtt);
  void OnFlowUpdated();
  void OnFlowUpdatedSimple();
  int SumPrioritiesAndInitializeRateFlowRates();
  int SumPrioritiesAndInitializeCwndFlowRates();
  DataRate CalculateRateFlowRatePortion(int rate_priorities, int total_priotities);
  void AllocateToRateFlows(int sum_priorities, DataRate leftover_rate);
  void AllocateToCwndFlows(int sum_priorities, DataRate sum_cwnd_rates);
  void DistributeToRateFlows(Timestamp at_time, bool update_loss_only);
  void DistributeToCwndFlows(std::shared_ptr<ActiveCwndFlow> update_caller);
  void DistributeToCwndFlowsDcSctp(std::shared_ptr<ActiveCwndFlow> update_caller);
  DataRate SumDesiredRates();
  DataRate SumAllocatedRates();
  void UpdateSumCalculatedRates(DataRate prev_rate, DataRate new_rate);

  void LogFseState(const char* id_string, DataRate old_s_cr);
};

}  // namespace webrtc

#endif  // MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_V2_H
