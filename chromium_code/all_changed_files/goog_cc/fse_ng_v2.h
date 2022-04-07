//
// Created by tobias on 03.10.2021.
//

#ifndef MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_NG_V2_H
#define MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_NG_V2_H

#include <cstdint>
#include <list>
#include <memory>
#include <mutex>
#include <unordered_set>

#include "api/units/data_rate.h"
#include "api/units/time_delta.h"
#include "api/units/timestamp.h"

#include "rtc_base/experiments/field_trial_parser.h"

namespace cricket {
class UsrsctpTransport;
}

namespace webrtc {

class SendSideBandwidthEstimation;

class HybridCwndFlow;
class RateFlow;

#define CR_DEFINE_STATIC_LOCAL(type, name, arguments) \
  static type& name = *new type arguments

class FseNgV2 {
 public:
  static FseNgV2& Instance();

  void RateUpdate(std::shared_ptr<RateFlow> flow,
                       DataRate new_rate);

  uint32_t CwndFlowUpdate(std::shared_ptr<HybridCwndFlow> flow,
                       uint32_t new_cwnd,
                       uint64_t last_rtt);

  std::shared_ptr<HybridCwndFlow> RegisterCwndFlow(
      uint32_t initial_cwnd,
      uint32_t initial_max_cwnd, 
      uint64_t inital_rtt_us,
      std::function<void(uint32_t)> update_callback);
  std::shared_ptr<RateFlow> RegisterRateFlow(
      DataRate initial_rate,
      std::function<void(DataRate)> update_callback);
  void DeRegisterWindowBasedFlow(std::shared_ptr<HybridCwndFlow> flow);
  void DeRegisterRateFlow(std::shared_ptr<RateFlow> flow);

 private:
  FseNgV2();
  ~FseNgV2();

  // The minimum rtt observed during the session
  TimeDelta base_rtt_;
  TimeDelta last_rtt_;

  // An estimate of the bottleneck's capacity
  // calculated by summing SRTP rates
  DataRate sum_calculated_rates_;
  DataRate cwnd_sum_calculated_rates_;

  int rate_flow_id_counter_;
  int cwnd_flow_id_counter_;

  std::unordered_set<std::shared_ptr<HybridCwndFlow>> cwnd_flows_;
  std::unordered_set<std::shared_ptr<RateFlow>> rate_flows_;

  std::mutex fse_mutex_;

  void OnRateFlowUpdate(
         std::shared_ptr<RateFlow> flow,
         int64_t relative_rate_change_bps,
         DataRate cc_rate
         );
  void UpdateSumCalculatedRates(
          int64_t relative_rate_change_bps, 
          DataRate cc_rate, 
          DataRate prev_fse_rate);
  void UpdateCwndSumCalculatedRates(
          DataRate cc_rate,
          DataRate prev_cc_rate);
  //Allocates rate to all the rtp flows, returns the sum og allocated rate
  DataRate UpdateRateFlows(int sum_priorities);
  void UpdateCwndFlows(DataRate sum_cwnd_rates);

  bool AllRateFlowsApplicationLimited() const;
  int SumPriorities() const;
  int SumRatePriorities() const;
  int SumCwndPriorities() const;
  void UpdateRttValues(TimeDelta last_rtt);
  bool UseCwndBasedSumCalculatedRates() const;
};

}  // namespace webrtc

#endif  // MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_NG_V2_H
