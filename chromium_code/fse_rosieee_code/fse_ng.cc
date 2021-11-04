//
// Created by tobias on 03.10.2021.
//

#include "modules/congestion_controller/goog_cc/fse_ng.h"

#include <algorithm>
#include <cstdint>
#include <memory>

#include "api/units/data_rate.h"
#include "api/units/time_delta.h"
#include "modules/congestion_controller/goog_cc/fse_flow.h"
#include "rtc_base/diagnostic_logging.h"
#include "rtc_base/logging.h"

namespace cricket {
class UsrsctpTransport;
}

namespace webrtc {

FseNg::FseNg()
    : previous_smoothed_max_rate_(DataRate::BitsPerSec(1)),
      previous_rate_change_(0),
      base_rtt_(TimeDelta::Seconds(100000000)),
      rosieee_update_call_num(0),
      flow_id_counter_(0) {}

void FseNg::RosieeeSrtpUpdate(DataRate prev_rate,
                              DataRate new_rate,
                              TimeDelta last_rtt) {
  // TODO: double check that last_rtt is not also 0 when not going over lo
  // if so it is something wrong with how you get last_Rtt

  /*if (last_rtt.ms() == 0) {
    last_rtt = TimeDelta::Millis(1);
  }*/

  rosieee_update_call_num++;
  RTC_LOG(LS_INFO) << "rosieee_update_call_num: " << rosieee_update_call_num;

  if (previous_max_rates_.size() == MOVING_AVG_N) {
    uint32_t max_cwnd =
        RosieeeMaxCwnd(new_rate.bps() - prev_rate.bps(), last_rtt);

    // TODO: with ROSIEEE it should really only be one flow...
    for (const auto& i : cwnd_flows_) {
      i->UpdateCc(max_cwnd);
    }
  } else {
    // We just skip the whole thing if there are not enough updates to calculate
    // smoothed rate yet
    previous_max_rates_.push_back(
        RosieeeMaxRate(new_rate.bps() - prev_rate.bps()));
  }
}

std::shared_ptr<FseNgWindowBasedFlow> FseNg::RosieeeRegisterWindowBasedFlow(
    cricket::UsrsctpTransport& transport) {
  RTC_LOG(LS_INFO) << "RosieeeRegisterWindowBasedFlow was called";

  std::shared_ptr<FseNgWindowBasedFlow> new_flow =
      std::make_shared<FseNgWindowBasedFlow>(flow_id_counter_++, 1, transport);
  cwnd_flows_.insert(new_flow);

  return new_flow;
}

DataRate FseNg::RosieeeMaxRate(long rate_change) {
  // Since there seems to be a bug in ROSIEEE where a sudden large decrease
  // which gives a negative smoothed max rate we have an extra check here
  if (rate_change < 0 && previous_rate_change_ < 0) {
    return DataRate::BitsPerSec(previous_smoothed_max_rate_.bps() / 2);
  } else if (previous_smoothed_max_rate_.bps() + rate_change < 0) {
    return DataRate::Zero();
  } else {
    return DataRate::BitsPerSec(previous_smoothed_max_rate_.bps() +
                                rate_change);
  }
}

DataRate FseNg::RosieeeSmoothedMaxRate(const std::list<DataRate>& max_rates) {
  DataRate sum_moving_avg = DataRate::Zero();
  // TODO: double check whether most recent max rate also should be in list or
  // not
  for (DataRate i : max_rates) {
    sum_moving_avg += i;
  }
  return (1 / static_cast<double>(MOVING_AVG_N)) * sum_moving_avg;
}

uint32_t FseNg::RosieeeMaxCwnd(long rate_change, TimeDelta last_rtt) {
  RTC_LOG(LS_INFO) << "rate_change: " << rate_change
                   << " last_rtt:" << last_rtt.ms();

  DataRate max_rate = RosieeeMaxRate(rate_change);
  RTC_LOG(LS_INFO) << "max_rate: " << max_rate.bps();

  previous_max_rates_.push_back(max_rate);
  previous_max_rates_.pop_front();

  LogPreviousMaxRates();

  DataRate smoothed_max_rate = RosieeeSmoothedMaxRate(previous_max_rates_);
  RTC_LOG(LS_INFO) << "smoothed_max_rate: " << smoothed_max_rate.bps();

  // TODO: might want to clamp it to maximum 10ms or something here so that you
  // can actually test stuff properly, 1 or 0 is unrealistically low right?
  base_rtt_ = std::min(base_rtt_, last_rtt);
  RTC_LOG(LS_INFO) << "base_rtt us: " << base_rtt_.us();

  // TODO: This value always becomes SCTP_MSS when base_rtt is really low like 0
  // or 1,
  // so might want to hardcode it to always be higher or something
  uint32_t cwnd_max =
      std::max(FseFlow::RateToCwnd(base_rtt_, smoothed_max_rate),
               static_cast<uint32_t>(SCTP_MSS));

  // maintenance
  previous_smoothed_max_rate_ = smoothed_max_rate;
  previous_rate_change_ = rate_change;

  RTC_LOG(LS_INFO) << "RosieeeMaxCwnd end, cwnd_max: " << cwnd_max;
  return cwnd_max;
}

void FseNg::LogPreviousMaxRates() {
  RTC_LOG(LS_INFO) << "previous max rates:";

  for (DataRate i : previous_max_rates_) {
    RTC_LOG(LS_INFO) << i.bps();
  }
}

FseNg& FseNg::Instance() {
  CR_DEFINE_STATIC_LOCAL(FseNg, s, ());
  return s;
}

}  // namespace webrtc
