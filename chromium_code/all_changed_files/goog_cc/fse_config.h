#ifndef MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_CONFIG_H
#define MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_CONFIG_H

#include <map>

namespace webrtc {

enum FseOpts { none, fse, fse_ng};


enum PriorityCases {fse_ng_case1, fse_ng_case2, normal};

enum FseNgVersions { original, reasonable };

class FseConfig {
  public:
    static FseOpts CurrentFse() {
      return webrtc::fse_ng;
    }

    static FseNgVersions CurrentFseNgVersion() {
      return webrtc::reasonable;
    }

    static int ResolveSrtpPriority(const int flow_id) {
      std::map<int, int>::const_iterator it;
      switch (CurrentPriorityCase()) {
        case fse_ng_case1:
          if (flow_id == 0) {
            return 2;
          }
          else {
            return 1;
          }
        case fse_ng_case2: 
          return 1;
        case normal:
        default:
          return 1;
      }
    }
    static int ResolveSctpPriority(const int flow_id) {
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
      return webrtc::normal;
    }
};

}
#endif  // MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_CONFIG_H
