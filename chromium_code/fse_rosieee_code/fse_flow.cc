//
// Created by tobias on 30.09.2021.
//

#include "modules/congestion_controller/goog_cc/fse_flow.h"

#include <cstdint>

#include "api/units/data_rate.h"
#include "media/sctp/usrsctp_transport.h"
#include "modules/congestion_controller/goog_cc/send_side_bandwidth_estimation.h"
#include "rtc_base/diagnostic_logging.h"
#include "rtc_base/logging.h"

namespace webrtc {

FseFlow::FseFlow(int id, int priority) : id_(id), priority_(priority) {}

FseFlow::~FseFlow() = default;

int FseFlow::Id() const {
  return id_;
}

int FseFlow::Priority() const {
  return priority_;
}

DataRate FseFlow::CwndToRate(uint32_t cwnd, uint64_t rtt_us) {
  if (rtt_us == 0) {
    return DataRate::Zero();
  }
  return DataRate::BitsPerSec(static_cast<uint32_t>(
      (1000000 * (static_cast<uint64_t>(cwnd)) << 3) / rtt_us));
}

uint32_t FseFlow::RateToCwnd(TimeDelta rtt, DataRate rate) {
  return (rtt * rate).bytes();
}

RateFlow::RateFlow(int id,
                   int priority,
                   DataRate fse_rate,
                   DataRate desired_rate,
                   SendSideBandwidthEstimation& flow_cc)
    : FseFlow(id, priority),
      fse_rate_(fse_rate),
      desired_rate_(desired_rate),
      flow_cc_(flow_cc) {}

RateFlow::~RateFlow() = default;

void RateFlow::UpdateCc(Timestamp at_time) {
  flow_cc_.FseUpdateTargetBitrate(fse_rate_, at_time);
}

DataRate RateFlow::FseRate() const {
  return fse_rate_;
}

void RateFlow::SetFseRate(DataRate new_rate) {
  fse_rate_ = new_rate;
}

DataRate RateFlow::DesiredRate() const {
  return desired_rate_;
}

void RateFlow::SetDesiredRate(DataRate new_rate) {
  desired_rate_ = new_rate;
}

WindowBasedFlow::WindowBasedFlow(int id,
                                 int priority,
                                 uint32_t initial_cwnd,
                                 uint64_t last_rtt,
                                 cricket::UsrsctpTransport& flow_cc)
    : FseFlow(id, priority),
      cwnd_(initial_cwnd),
      last_rtt_(last_rtt),
      sctp_transport_(flow_cc) {
  RTC_LOG(LS_INFO) << "creating a WindowBasedFlow";
}

WindowBasedFlow::~WindowBasedFlow() = default;

void WindowBasedFlow::UpdateCc(uint32_t cwnd) {
  RTC_LOG(LS_INFO) << "WindowBasedFlow gonna call FseUpdate" << cwnd_ << " "
                   << last_rtt_;
  sctp_transport_.SetCwnd(cwnd);
}

FseNgWindowBasedFlow::FseNgWindowBasedFlow(int id,
                                           int priority,
                                           cricket::UsrsctpTransport& transport)
    : FseFlow(id, priority), sctp_transport_(transport) {
  RTC_LOG(LS_INFO) << "creating a FseNgWindowBasedFlow";
}

FseNgWindowBasedFlow::~FseNgWindowBasedFlow() = default;

void FseNgWindowBasedFlow::UpdateCc(uint32_t max_cwnd) {
  RTC_LOG(LS_INFO) << "FseNgWindowBasedFlow gonna call FseNgUpdate" << max_cwnd;
  sctp_transport_.SetMaxCwnd(max_cwnd);
}

}  // namespace webrtc
