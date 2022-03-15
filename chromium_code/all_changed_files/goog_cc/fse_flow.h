#ifndef MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_FLOW_H
#define MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_FLOW_H

#include <cstdint>

#include "api/units/data_rate.h"
#include "api/units/time_delta.h"
#include "api/units/timestamp.h"

namespace webrtc {

class SendSideBandwidthEstimation;

class Flow {
 public:
  Flow(int id, int priority);
  virtual ~Flow();
  int Id() const;
  int Priority() const;
  static DataRate CwndToRate(uint32_t cwnd, uint64_t rtt);
  static uint32_t RateToCwnd(TimeDelta rtt, DataRate rate);

 protected:
  int id_;
  int priority_;
};

class RateFlow : public Flow {
 public:
  RateFlow(int id,
           int priority,
           DataRate fse_rate,
           DataRate desired_rate,
           std::function<void(DataRate)> update_callback);
  ~RateFlow() override;
  void UpdateCc();
  DataRate FseRate() const;
  void SetFseRate(DataRate new_rate);
  DataRate DesiredRate() const;
  void SetDesiredRate(DataRate new_rate);
  bool IsApplicationLimited();
 private:
  DataRate fse_rate_;
  DataRate desired_rate_;
  //Callback function registered by the congestion controller to apply updates with
  std::function<void(DataRate)> update_callback_;
};

class PassiveCwndFlow : public Flow {
 public:
  PassiveCwndFlow(int id,
           int priority,
           uint32_t initial_max_cwnd,
           std::function<void(uint32_t)> update_callback);
  ~PassiveCwndFlow() override;
  void UpdateCc(uint32_t max_cwnd);
  uint32_t GetInitialMaxCwnd();
    
 private:
  uint32_t initial_max_cwnd_;
  std::function<void(uint32_t)> update_callback_;
};

class ActiveCwndFlow : public Flow {
 public:
  ActiveCwndFlow(int id,
           int priority,
           uint32_t initial_cwnd,
           uint64_t last_rtt,
           std::function<void(uint32_t)> update_callback);
  ~ActiveCwndFlow() override;
  void UpdateCc();
  uint32_t FseCwnd() const;
  void SetFseCwnd(uint32_t new_cwnd);
  uint64_t LastRtt() const;
  void SetLastRtt(uint64_t new_last_rtt);
    
 private:
  uint32_t fse_cwnd_;
  uint64_t last_rtt_;
  //Callback function registered by the congestion controller to apply updates with
  std::function<void(uint32_t)> update_callback_;
};

}  // namespace webrtc
#endif  // MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_FLOW_H
