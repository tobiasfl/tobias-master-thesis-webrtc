//
// Created by tobias on 03.10.2021.
//

#ifndef MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_NG_H
#define MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_NG_H

#include <cstdint>
#include <list>
#include <memory>
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

#define MOVING_AVG_N 5
#define SCTP_MSS 536
// TODO: this could probably be in config file instead
#define MIN_RTT 10

class FseNg {
 public:
  static FseNg& Instance();
  void RosieeeSrtpUpdate(DataRate prev_rate,
                         DataRate new_rate,
                         TimeDelta last_rtt);

  void FseNgSrtpUpdate(std::shared_ptr<FseNgRateFlow> flow,
                       DataRate prev_rate,
                       DataRate new_rate,
                       DataRate max_rate,
                       TimeDelta last_rtt,
                       Timestamp at_time);

  std::shared_ptr<FseNgWindowBasedFlow> RegisterWindowBasedFlow(
      cricket::UsrsctpTransport& transport);
  std::shared_ptr<FseNgRateFlow> RegisterRateFlow(
      DataRate min_rate,
      int priority,
      SendSideBandwidthEstimation& cc);

 private:
  FseNg();
  ~FseNg();
  //ROSIEEE stuff
  std::list<DataRate> previous_max_rates_;
  DataRate previous_smoothed_max_rate_;
  long previous_rate_change_;

  //COMMON stuff
  TimeDelta base_rtt_;

  //FSE-NG stuff
  DataRate sum_calculated_rates_;

  uint64_t update_call_num;
  int flow_id_counter_;

  std::unordered_set<std::shared_ptr<FseNgWindowBasedFlow>> cwnd_flows_;
  std::unordered_set<std::shared_ptr<FseNgRateFlow>> rate_flows_;

  void OnSrtpFlowUpdate(std::shared_ptr<FseNgRateFlow> flow,
                        DataRate prev_rate,
                        DataRate new_rate,
                        DataRate max_rate,
                        TimeDelta last_rtt,
                        Timestamp at_time);

  uint32_t RosieeeMaxCwnd(long rate_change, TimeDelta last_rtt);
  DataRate RosieeeMaxRate(long rate_change);
  DataRate RosieeeSmoothedMaxRate(const std::list<DataRate>& max_rates);
  int SumPriorities() const;
  int SumCwndPriorities() const;
  void LogPreviousMaxRates();
};

}  // namespace webrtc

#endif  // MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_NG_H
