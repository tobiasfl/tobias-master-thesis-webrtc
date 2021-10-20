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

namespace cricket {
class UsrsctpTransport;
}

namespace webrtc {

class FseNgWindowBasedFlow;

#define CR_DEFINE_STATIC_LOCAL(type, name, arguments) \
  static type& name = *new type arguments

#define MOVING_AVG_N 5
#define SCTP_MSS 536

class FseNg {
 public:
  static FseNg& Instance();
  void RosieeeSrtpUpdate(DataRate prev_rate,
                         DataRate new_rate,
                         TimeDelta last_rtt);
  std::shared_ptr<FseNgWindowBasedFlow> RosieeeRegisterWindowBasedFlow(
      cricket::UsrsctpTransport& transport);

 private:
  FseNg();
  ~FseNg();
  std::list<DataRate> previous_max_rates_;
  DataRate previous_smoothed_max_rate_;
  long previous_rate_change_;
  TimeDelta base_rtt_;

  uint64_t rosieee_update_call_num;

  int flow_id_counter_;
  std::unordered_set<std::shared_ptr<FseNgWindowBasedFlow>> cwnd_flows_;
  uint32_t RosieeeMaxCwnd(long rate_change, TimeDelta last_rtt);
  DataRate RosieeeMaxRate(long rate_change);
  DataRate RosieeeSmoothedMaxRate(const std::list<DataRate>& max_rates);
  void LogPreviousMaxRates();
};

}  // namespace webrtc

#endif  // MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_NG_H
