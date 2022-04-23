#ifndef MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_CONFIG_H
#define MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_CONFIG_H

#include <map>

#include "api/units/data_rate.h"
#include "modules/congestion_controller/goog_cc/fse_flow.h"

namespace webrtc {

/*
struct FseNgExperimentConfig {
      int rtp_flow1_prio;
      int rtp_flow2_prio;
      int sctp_flow1_prio;
      int sctp_flow2_prio;
      DataRate rtp_flow1_max_rate_kbps;
      DataRate rtp_flow2_max_rate_kbps;
};*/

#define CR_DEFINE_STATIC_LOCAL(type, name, arguments) \
  static type& name = *new type arguments

enum FseVersion { none, fse, fse_ng, fse_v2, fse_ng_v2};

enum DesiredRateCase { infinity, fse_ng_paper_case, fse_case_2, all_limited };

enum PriorityCase { equal, rate_flow_double, cwnd_flow_double, varied, fse_case_3 };

class FseConfig {
  public:
    static FseConfig& Instance();
    FseVersion CurrentFse();
    DataRate ResolveDesiredRate(int flow_id);
    int ResolveRateFlowPriority(int flow_id);
    int ResolveCwndFlowPriority(int flow_id);
    bool CoupleDcSctp();
  private:
    FseConfig();
    ~FseConfig();
    FseVersion current_fse_;
    DesiredRateCase current_desired_rate_case_;
    PriorityCase current_priority_case_;
    bool couple_dcsctp_;
};

}
#endif  // MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_CONFIG_H
