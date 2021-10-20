#ifndef MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_FLOW_H
#define MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_FLOW_H

#include <cstdint>

#include "api/units/data_rate.h"
#include "api/units/time_delta.h"
#include "api/units/timestamp.h"

namespace cricket {

class UsrsctpTransport;

}

namespace webrtc {

class SendSideBandwidthEstimation;

class FseFlow {
 public:
  FseFlow(int id, int priority);
  virtual ~FseFlow();
  int Id() const;
  int Priority() const;
  static DataRate CwndToRate(uint32_t cwnd, uint64_t rtt);
  static uint32_t RateToCwnd(TimeDelta rtt, DataRate rate);

 protected:
  int id_;
  int priority_;
};

class RateFlow : public FseFlow {
 public:
  RateFlow(int id,
           int priority,
           DataRate fse_rate,
           DataRate desired_rate,
           SendSideBandwidthEstimation& flow_cc);
  ~RateFlow() override;
  void UpdateCc(Timestamp at_time);
  DataRate FseRate() const;
  void SetFseRate(DataRate new_rate);
  DataRate DesiredRate() const;
  void SetDesiredRate(DataRate new_rate);

 private:
  DataRate fse_rate_;
  DataRate desired_rate_;
  SendSideBandwidthEstimation& flow_cc_;
};

class WindowBasedFlow : public FseFlow {
 public:
  WindowBasedFlow(int id,
                  int priority,
                  uint32_t initial_cwnd,
                  uint64_t last_rtt,
                  cricket::UsrsctpTransport& flow_cc);
  ~WindowBasedFlow() override;
  void UpdateCc(uint32_t cwnd);

 private:
  uint32_t cwnd_;
  uint64_t last_rtt_;
  cricket::UsrsctpTransport& sctp_transport_;
};

class FseNgWindowBasedFlow : public FseFlow {
 public:
  FseNgWindowBasedFlow(int id,
                       int priority,
                       cricket::UsrsctpTransport& flow_cc);
  ~FseNgWindowBasedFlow() override;
  void UpdateCc(uint32_t max_cwnd);

 private:
  cricket::UsrsctpTransport& sctp_transport_;
};

}  // namespace webrtc
#endif  // MODULES_CONGESTION_CONTROLLER_GOOG_CC_CHROMIUM_FLOW_STATE_EXCHANGE_H
