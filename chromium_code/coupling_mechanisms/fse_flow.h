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

//TODO: this one can be combined with RateFlow instead most likely
class GccRateFlow : public Flow {
  public:
    GccRateFlow(int id,
           int priority,
           DataRate fse_rate,
           DataRate desired_rate,
           std::function<void(DataRate, Timestamp, bool)> update_callback);
    ~GccRateFlow() override;
  void UpdateCc(Timestamp at_time, bool update_loss_only);
  DataRate FseRate() const;
  void SetFseRate(DataRate new_rate);
  DataRate DesiredRate() const;
  void SetDesiredRate(DataRate new_rate);

  private:
    DataRate fse_rate_;
    DataRate desired_rate_;

    std::function<void(DataRate, Timestamp, bool)> update_callback_;

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
           DataRate initial_rate,
           std::function<void(uint32_t)> update_callback);
  ~ActiveCwndFlow() override;
  void UpdateCc(uint32_t new_cwnd);
  DataRate FseRate() const;
  void SetFseRate(DataRate new_rate);
    
 private:
  DataRate fse_rate_;
  std::function<void(uint32_t)> update_callback_;
};

class HybridCwndFlow : public Flow {
 public:
  HybridCwndFlow(int id,
           int priority,
           uint32_t initial_max_cwnd,
           DataRate inital_cwnd_as_rate,
           std::function<void(uint32_t)> update_callback);
  ~HybridCwndFlow() override;
  void UpdateCcMaxCwnd(uint32_t new_max_cwnd);
  uint32_t GetInitialMaxCwnd();
  DataRate GetPrevCwnd() const;
  void SetPrevCwnd(DataRate cwnd_as_rate);

 private:
  uint32_t initial_max_cwnd_;
  std::function<void(uint32_t)> update_callback_;
  DataRate prev_cwnd_as_rate_;
};

}  // namespace webrtc
#endif  // MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_FLOW_H
