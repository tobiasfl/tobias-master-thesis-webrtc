#ifndef MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_CONFIG_H
#define MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_CONFIG_H

#include <map>

#include "api/units/data_rate.h"

namespace webrtc {

enum FseOpts { none, fse, fse_ng};

enum PriorityCases {fse_ng_case1, fse_ng_case2, normal, fse_case3 };

enum DesiredRateCases { infinity, fse_ng_default, fse_case2 };

enum FseNgVersions { original, extended };

enum FseNgUpdateValue { final_rate_only, delay_only };

//TODO: Use separate resolvers and option enums for each FSE- type
class FseConfig {
  public:
    static FseOpts CurrentFse() {
      return webrtc::fse_ng;
    }

    static FseNgVersions CurrentFseNgVersion() {
      return webrtc::extended;
    }

    static FseNgUpdateValue CurrentFseNgUpdateValue() {
      return webrtc::final_rate_only;
    }

    static DataRate CurrentRateFlowDesiredRate() {
      return DataRate::KilobitsPerSec(1500);
    }

    static DataRate ResolveRateFlowDesiredRate(const int flow_id) {
      switch (CurrentDesiredRateCase()) {
        case fse_ng_default:
          return DataRate::KilobitsPerSec(1500);
        case infinity:
          return DataRate::KilobitsPerSec(1000000);
        case fse_case2:
          if (flow_id == 0) {
            return DataRate::KilobitsPerSec(1000);
          }
          else {
            return DataRate::KilobitsPerSec(1000000);
          }
      }
    }
    
    static int ResolveRateFlowPriority(const int flow_id) {
      int priority;
      switch (CurrentPriorityCase()) {
        case fse_ng_case1:
          if (flow_id == 0) {
            priority = 2; 
          }
          else {
            priority = 1;
          }
          break;
        case fse_ng_case2: 
          priority = 1;
          break;
        case fse_case3:
          if (flow_id == 0) {
            priority = 1;
          }
          else {
            priority = 2;
          }
          break;
        case normal:
        default:
          priority = 1;
      }
      return priority;
    }

    static int ResolveCwndFlowPriority(const int flow_id) {
      switch (CurrentPriorityCase()) {
        case fse_ng_case1:
          return 1;
        case fse_ng_case2:
          return 2;
        case normal:
        default:
          return 1;   
      }
    }
  private:
    static PriorityCases CurrentPriorityCase() {
      return webrtc::fse_case3;
    }
    static DesiredRateCases CurrentDesiredRateCase() {
      return webrtc::infinity;
    }
};

}
#endif  // MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_CONFIG_H
