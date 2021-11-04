//
// Created by tobias on 03.10.2021.
//

#include "modules/congestion_controller/goog_cc/fse_ng.h"

#include <algorithm>
#include <cstdint>
#include <memory>

#include "api/units/data_rate.h"
#include "api/units/time_delta.h"
#include "api/units/timestamp.h"
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
      sum_calculated_rates_(DataRate::Zero()),
      update_call_num(0),
      flow_id_counter_(0) {}

void FseNg::RosieeeSrtpUpdate(DataRate prev_rate,
                              DataRate new_rate,
                              TimeDelta last_rtt) {
  if (last_rtt.ms() < MIN_RTT) {
    last_rtt = TimeDelta::Millis(MIN_RTT);
  }

  update_call_num++;
  RTC_LOG(LS_INFO) << "rosieee_update_call_num: " << update_call_num;

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

std::shared_ptr<FseNgWindowBasedFlow> FseNg::RegisterWindowBasedFlow(
    cricket::UsrsctpTransport& transport) {
  RTC_LOG(LS_INFO) << "FseNgRegisterWindowBasedFlow was called";

  std::shared_ptr<FseNgWindowBasedFlow> new_flow =
      std::make_shared<FseNgWindowBasedFlow>(flow_id_counter_++, 1, transport);
  cwnd_flows_.insert(new_flow);

  return new_flow;
}

void FseNg::FseNgSrtpUpdate(std::shared_ptr<FseNgRateFlow> flow,
                            DataRate prev_rate,
                            DataRate new_rate,
                            DataRate max_rate,
                            TimeDelta last_rtt,
                            Timestamp at_time) {
  if (last_rtt.ms() < MIN_RTT) {
    last_rtt = TimeDelta::Millis(MIN_RTT);
  }

  update_call_num++;
  RTC_LOG(LS_INFO) << "fse_ng_update_call_num: " << update_call_num;
  FseNg::OnSrtpFlowUpdate(flow, prev_rate, new_rate, max_rate, last_rtt, at_time);
}

void FseNg::OnSrtpFlowUpdate(std::shared_ptr<FseNgRateFlow> flow,
                             DataRate prev_rate,
                             DataRate new_rate,
                             DataRate max_rate,
                             TimeDelta last_rtt,
                             Timestamp at_time) {
  RTC_LOG(LS_INFO) << "OnSrtpFlowUpdate called: " << prev_rate.bps() << " " << new_rate.bps() << " " << max_rate.bps(); 
  // TODO: should also actually check if all are application limited as well
  // TODO: ASK SAFIQUL ABOUT WHAT THEY MEAN WITH APPLICATION LIMITED HERE
  RTC_LOG(LS_INFO) << "sum_calculated_rates_ before adding: " << sum_calculated_rates_.bps();
  if (cwnd_flows_.empty()) {
    sum_calculated_rates_ += new_rate - flow->PrevFseRate();
  } else {
    int64_t rate_change = new_rate.bps() - prev_rate.bps();
    sum_calculated_rates_ = DataRate::BitsPerSec(sum_calculated_rates_.bps() + new_rate.bps() 
        - (flow->PrevFseRate().bps() + rate_change));
  }
  RTC_LOG(LS_INFO) << "sum_calculated_rates_ after adding: " << sum_calculated_rates_.bps();

  DataRate sum_rtp_rates = DataRate::Zero();
  for (const auto& i : rate_flows_) {
    // TODO: now using max rate of update caller, 
    // might want to store the current max rate of each flow instead
    DataRate flow_rate = std::min((i->Priority() * sum_calculated_rates_) 
            / SumPriorities(), max_rate);
    i->SetPrevFseRate(i->FseRate());
    i->SetFseRate(flow_rate);
    RTC_LOG(LS_INFO) << "updating an SRTP flow with kbps rate: " << flow_rate.kbps();
   
    i->UpdateCc(at_time);

    sum_rtp_rates += flow_rate;
  }
  DataRate sum_sctp_rates = sum_calculated_rates_ - sum_rtp_rates;
  base_rtt_ = std::min(base_rtt_, last_rtt);
  for(const auto& i : cwnd_flows_) {
    //TODO: Might want to double check if sctp should be updated here
    uint32_t flow_max_cwnd = (i->Priority() * 
            FseFlow::RateToCwnd(base_rtt_, sum_sctp_rates)) 
            / SumCwndPriorities();
    i->UpdateCc(flow_max_cwnd);
    RTC_LOG(LS_INFO) << "flow_max_cwnd for " << i->Id() << 
        " " << flow_max_cwnd;
  }
}

std::shared_ptr<FseNgRateFlow> FseNg::RegisterRateFlow(
    DataRate initial_rate,
    int priority,
    SendSideBandwidthEstimation& cc) {
  // TODO: add locking??????
  RTC_LOG(LS_INFO) << "registering rate flow in FseNg with init_rate: " << initial_rate.bps();
  std::shared_ptr<FseNgRateFlow> new_flow = std::make_shared<FseNgRateFlow>(
      flow_id_counter_++, priority, initial_rate, cc);
  rate_flows_.insert(new_flow);
  sum_calculated_rates_ += initial_rate;

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

  // TODO: might want to clamp it to maximum 10ms or something here so that
  // you can actually test stuff properly, 1 or 0 is unrealistically low
  // right?
  base_rtt_ = std::min(base_rtt_, last_rtt);
  RTC_LOG(LS_INFO) << "base_rtt us: " << base_rtt_.us();

  // TODO: This value always becomes SCTP_MSS when base_rtt is really low like
  // 0 or 1, so might want to hardcode it to always be higher or something
  uint32_t cwnd_max =
      std::max(FseFlow::RateToCwnd(base_rtt_, smoothed_max_rate),
               static_cast<uint32_t>(SCTP_MSS));

  // maintenance
  previous_smoothed_max_rate_ = smoothed_max_rate;
  previous_rate_change_ = rate_change;

  RTC_LOG(LS_INFO) << "RosieeeMaxCwnd end, cwnd_max: " << cwnd_max;
  return cwnd_max;
}


int FseNg::SumPriorities() const {
  int sum_priorities = 0;
  for (const auto& i : rate_flows_) {
    sum_priorities += i->Priority();
  }
  for (const auto& i : cwnd_flows_) {
    sum_priorities += i->Priority();
  }
  return sum_priorities;
}

int FseNg::SumCwndPriorities() const {
  int sum_priorities = 0;
  for (const auto& i : cwnd_flows_) {
      sum_priorities += i->Priority();
    }
    return sum_priorities;
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
