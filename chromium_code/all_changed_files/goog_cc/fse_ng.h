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

namespace cricket {
class UsrsctpTransport;
}

namespace webrtc {

class SendSideBandwidthEstimation;

class FseNgWindowBasedFlow;
class FseNgRateFlow;

#define CR_DEFINE_STATIC_LOCAL(type, name, arguments) \
  static type& name = *new type arguments

class FseNg {
 public:
  static FseNg& Instance();

  void SrtpUpdate(std::shared_ptr<FseNgRateFlow> flow,
                       int64_t relative_rate_change,
                       DataRate new_rate,
                       DataRate max_rate,
                       TimeDelta last_rtt,
                       Timestamp at_time);

  std::shared_ptr<FseNgWindowBasedFlow> RegisterWindowBasedFlow(
      uint32_t initial_max_cwnd, 
      cricket::UsrsctpTransport& transport);
  std::shared_ptr<FseNgRateFlow> RegisterRateFlow(
      DataRate min_rate,
      DataRate max_rate,
      SendSideBandwidthEstimation& cc);
  void DeRegisterWindowBasedFlow(std::shared_ptr<FseNgWindowBasedFlow> flow);
  void DeRegisterRateFlow(std::shared_ptr<FseNgRateFlow> flow);

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

  std::unordered_set<std::shared_ptr<FseNgWindowBasedFlow>> cwnd_flows_;
  std::unordered_set<std::shared_ptr<FseNgRateFlow>> rate_flows_;

  std::mutex fse_mutex_;
  void OnSrtpFlowUpdate(std::shared_ptr<FseNgRateFlow> flow,
                        int64_t relative_rate_change,
                        DataRate new_rate,
                        TimeDelta last_rtt,
                        Timestamp at_time);

  bool AllSrtpFlowsApplicationLimited();
  int SumPriorities() const;
  int SumRatePriorities() const;
  int SumCwndPriorities() const;
  void LogPreviousMaxRates();
};

}  // namespace webrtc

#endif  // MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_NG_H
