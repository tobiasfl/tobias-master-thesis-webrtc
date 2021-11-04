//
// Created by tobias on 03.10.2021.
//

#include "modules/congestion_controller/goog_cc/fse_ng.h"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <chrono>

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
    : base_rtt_(TimeDelta::Seconds(100000000)),
      sum_calculated_rates_(DataRate::Zero()),
      update_call_num(0),
      flow_id_counter_(0) {}

std::shared_ptr<FseNgWindowBasedFlow> FseNg::RegisterWindowBasedFlow(
    uint32_t initial_max_cwnd,
    cricket::UsrsctpTransport& transport) {
  fse_mutex_.lock();

  RTC_LOG(LS_INFO) << "FseNgRegisterWindowBasedFlow was called";

  std::shared_ptr<FseNgWindowBasedFlow> new_flow =
      std::make_shared<FseNgWindowBasedFlow>(flow_id_counter_++, 1, 
              initial_max_cwnd, transport);
  cwnd_flows_.insert(new_flow);

  fse_mutex_.unlock();

  return new_flow;
}

void FseNg::SrtpUpdate(std::shared_ptr<FseNgRateFlow> flow,
                            DataRate prev_rate,
                            DataRate new_rate,
                            DataRate max_rate,
                            TimeDelta last_rtt,
                            Timestamp at_time) {
  fse_mutex_.lock();

  if (last_rtt.ms() < MIN_RTT) {
    last_rtt = TimeDelta::Millis(MIN_RTT);
  }

  update_call_num++;
  RTC_LOG(LS_INFO) << "fse_ng_update_call_num: " << update_call_num << " flow id: " << flow->Id();

  at_time = Timestamp::Millis(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());

  FseNg::OnSrtpFlowUpdate(flow, prev_rate, new_rate, max_rate, last_rtt, at_time);

  fse_mutex_.unlock();
}

// Must be called while owning the mutex
void FseNg::OnSrtpFlowUpdate(std::shared_ptr<FseNgRateFlow> flow,
                             DataRate prev_rate,
                             DataRate new_rate,
                             DataRate max_rate,
                             TimeDelta last_rtt,
                             Timestamp at_time) {

  UpdateSumCalculatedRates(flow->FseRate(), new_rate, prev_rate);

  RTC_LOG(LS_INFO) << "PLOT_THIS(sum_calculated_rates) " << "total_rate" << "-FSE_RATE "
    << at_time.ms() 
    << " " << sum_calculated_rates_.kbps();

  RTC_LOG(LS_INFO) << "max_rate: " << max_rate.bps();
  DataRate sum_rtp_rates = DataRate::Zero();
  for (const auto& rate_flow : rate_flows_) {
    // TODO: now using max rate of update caller, 
    // might want to store the current max rate of each flow instead
    DataRate flow_fse_rate = std::min((rate_flow->Priority() * sum_calculated_rates_) 
            / SumPriorities(), max_rate);
    rate_flow->UpdateFlow(flow_fse_rate, at_time);

    sum_rtp_rates += flow_fse_rate;
  }

  DataRate sum_sctp_rates = sum_calculated_rates_ - sum_rtp_rates;

  base_rtt_ = std::min(base_rtt_, last_rtt);
  for(const auto& cwnd_flow : cwnd_flows_) {
    uint32_t flow_max_cwnd = 
        (cwnd_flow->Priority() * FseFlow::RateToCwnd(base_rtt_, sum_sctp_rates)) / SumCwndPriorities();
    //For plotting
    RTC_LOG(LS_INFO) << "PLOT_THIS(fse_r_update) " << cwnd_flow->Id() << "-MAX_FSE_RATE "
                   << at_time.ms() << " " << (FseFlow::CwndToRate(flow_max_cwnd, base_rtt_.us())).kbps();

    cwnd_flow->UpdateCc(flow_max_cwnd);
  }
}

// Must be called while owning the mutex
void FseNg::UpdateSumCalculatedRates(DataRate curr_fse_rate,
                                  DataRate new_cc_rate, 
                                  DataRate prev_cc_rate) {
  RTC_LOG(LS_INFO) << " curr_fse_rate:"<< 
      curr_fse_rate.bps() << " new_cc_rate:" << 
      new_cc_rate.bps() << " prev_cc_rate:" << 
      prev_cc_rate.bps();
  RTC_LOG(LS_INFO) << "sum_calculated_rates_ before adding: " << sum_calculated_rates_.bps();

  if (cwnd_flows_.empty()) {
    sum_calculated_rates_ = (sum_calculated_rates_ + new_cc_rate) - curr_fse_rate;
  } else {
    int64_t rate_change = new_cc_rate.bps() - prev_cc_rate.bps();
    sum_calculated_rates_ = DataRate::BitsPerSec(((sum_calculated_rates_ + new_cc_rate) 
        - curr_fse_rate).bps() + rate_change);
  }
    RTC_LOG(LS_INFO) << "sum_calculated_rates_ after adding: " << sum_calculated_rates_.bps();
}


std::shared_ptr<FseNgRateFlow> FseNg::RegisterRateFlow(
    DataRate initial_rate,
    int priority,
    SendSideBandwidthEstimation& cc) {
  fse_mutex_.lock();

  RTC_LOG(LS_INFO) << "registering rate flow in FseNg with init_rate: " << initial_rate.bps();
  std::shared_ptr<FseNgRateFlow> new_flow = std::make_shared<FseNgRateFlow>(
      flow_id_counter_++, priority, initial_rate, cc);
  rate_flows_.insert(new_flow);
  sum_calculated_rates_ += initial_rate;
  
  fse_mutex_.unlock();

  return new_flow;
}

void FseNg::DeRegisterWindowBasedFlow(std::shared_ptr<FseNgWindowBasedFlow> flow) {
  fse_mutex_.lock();
  RTC_LOG(LS_INFO) << "DeRegisterWindowBasedFlow was called";
  //TODO: do we need to set max_cwnd here or not???????
  cwnd_flows_.erase(flow);
  fse_mutex_.unlock();
}

void FseNg::DeRegisterRateFlow(std::shared_ptr<FseNgRateFlow> flow) {
  fse_mutex_.lock();
  RTC_LOG(LS_INFO) << "DeRegisterRateFlow was called";
  //TODO: Either this or remove the current CC rate instead 
  sum_calculated_rates_ -= flow->FseRate();

  rate_flows_.erase(flow);

  // If last RTP flow leaves, the window limits of all window based flows are removed
  // Since no congestion control info is available
  if (rate_flows_.empty()){
    for (const auto& cwnd_flow : cwnd_flows_) {
      cwnd_flow->UpdateCc(cwnd_flow->GetInitialMaxCwnd());
    }
  }

  fse_mutex_.unlock();
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

FseNg& FseNg::Instance() {
  CR_DEFINE_STATIC_LOCAL(FseNg, s, ());
  return s;
}

}  // namespace webrtc
