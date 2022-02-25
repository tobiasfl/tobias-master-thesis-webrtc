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

class FseNgCwndFlow : public FseFlow {
 public:
  FseNgCwndFlow(int id,
                       int priority,
                       uint32_t initial_max_cwnd,
                       cricket::UsrsctpTransport& flow_cc);
  ~FseNgCwndFlow() override;
  void UpdateCc(uint32_t max_cwnd);
  uint32_t GetInitialMaxCwnd();
    
 private:
  uint32_t initial_max_cwnd_;
  cricket::UsrsctpTransport& sctp_transport_;
};

class FseNgRateFlow : public FseFlow {
 public:
  FseNgRateFlow(int id,
                int priority,
                DataRate initial_bit_rate,
                DataRate initial_max_rate,
                std::function<void(DataRate)> update_callback);
  ~FseNgRateFlow() override;
  void UpdateFlow(DataRate new_fse_rate);
  DataRate FseRate() const;
  bool IsApplicationLimited();
  void SetCurrMaxRate(DataRate max_rate);
  DataRate CurrMaxRate() const;
  void SetCurrMinRate(DataRate min_rate);
  DataRate CurrMinRate() const;
 private:
  //The rate calculated by the FseNg in the previous update
  DataRate fse_rate_;
  //The max rate set in the flows last update calll
  DataRate curr_max_rate_;
  //Callback function registered by the congestion controller to apply updates with
  std::function<void(DataRate)> update_callback_;
};

}  // namespace webrtc
#endif  // MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_FLOW_H
