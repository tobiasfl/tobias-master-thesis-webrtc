#ifndef MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_CONFIG_H
#define MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_CONFIG_H

namespace webrtc {

enum FseOpts { none, fse, fse_ng};

class FseConfig {
  public:
    static FseOpts CurrentFse() {return webrtc::fse;}
};

}
#endif  // MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_CONFIG_H
