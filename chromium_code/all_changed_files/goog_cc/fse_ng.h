//
// Created by tobias on 03.10.2021.
//

#ifndef MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_NG_H
#define MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_NG_H

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

class FseNgCwndFlow;
class FseNgRateFlow;

#define CR_DEFINE_STATIC_LOCAL(type, name, arguments) \
  static type& name = *new type arguments

class FseNg {
 public:
  static FseNg& Instance();

  void RateUpdate(std::shared_ptr<FseNgRateFlow> flow,
                       DataRate new_rate,
                       TimeDelta last_rtt);

  std::shared_ptr<FseNgCwndFlow> RegisterCwndFlow(
      uint32_t initial_max_cwnd, 
      cricket::UsrsctpTransport& transport);
  std::shared_ptr<FseNgRateFlow> RegisterRateFlow(
      DataRate initial_rate,
      std::function<void(DataRate)> update_callback);
  void DeRegisterWindowBasedFlow(std::shared_ptr<FseNgCwndFlow> flow);
  void DeRegisterRateFlow(std::shared_ptr<FseNgRateFlow> flow);

  bool UpdateValFinalRate() const;

 private:
  FseNg();
  ~FseNg();

  // The minimum rtt observed during the session
  TimeDelta base_rtt_;

  // An estimate of the bottleneck's capacity
  // calculated by summing SRTP rates
  DataRate sum_calculated_rates_;

  uint64_t update_call_num;
  int rate_flow_id_counter_;
  int cwnd_flow_id_counter_;

  std::unordered_set<std::shared_ptr<FseNgCwndFlow>> cwnd_flows_;
  std::unordered_set<std::shared_ptr<FseNgRateFlow>> rate_flows_;

  std::mutex fse_mutex_;

  bool update_val_final_rate_;

  void OnRateFlowUpdate(
         std::shared_ptr<FseNgRateFlow> flow,
         int64_t relative_rate_change_bps,
         DataRate cc_rate,
         TimeDelta last_rtt);
  void UpdateSumCalculatedRates(
          int64_t relative_rate_change_bps, 
          DataRate cc_rate, 
          DataRate prev_fse_rate);
  //Allocates rate to all the rtp flows, returns the sum og allocated rate
  DataRate UpdateRateFlows(int sum_priorities);
  void UpdateCwndFlows(DataRate sum_cwnd_rates);

  bool AllRateFlowsApplicationLimited() const;
  int SumPriorities() const;
  int SumRatePriorities() const;
  int SumCwndPriorities() const;
};

}  // namespace webrtc

#endif  // MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_NG_H
