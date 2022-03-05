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
                   std::function<void(DataRate)> update_callback)
    : FseFlow(id, priority),
      fse_rate_(fse_rate),
      desired_rate_(desired_rate),
      update_callback_(update_callback) {
  RTC_LOG(LS_INFO) << "Creating a RateFlow";
}

RateFlow::~RateFlow() = default;

void RateFlow::UpdateCc() {
  RTC_LOG(LS_INFO) << "PLOT_THIS_RTP_FSE_RATE_KBPS" << id_ << " rate=" << FseRate().kbps();
  update_callback_(FseRate());
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

bool RateFlow::IsApplicationLimited() {
    return FseRate() >= desired_rate_;
}

CwndFlow::CwndFlow(int id,
                   int priority,
                   uint32_t initial_max_cwnd,
                   std::function<void(uint32_t)> update_callback)
    : FseFlow(id, priority), 
    initial_max_cwnd_(initial_max_cwnd), 
    update_callback_(update_callback) {
  RTC_LOG(LS_INFO) << "creating a FseNgWindowBasedFlow";
}

CwndFlow::~CwndFlow() = default;

void CwndFlow::UpdateCc(uint32_t max_cwnd) {
  update_callback_(max_cwnd);
}

uint32_t CwndFlow::GetInitialMaxCwnd() {
  return initial_max_cwnd_;
}

}  // namespace webrtc
