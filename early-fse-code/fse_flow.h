//
// Created by tobias on 21.04.2021.
//

#ifndef MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_FLOW_H
#define MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_FLOW_H

#include "api/units/data_rate.h"
#include "modules/congestion_controller/goog_cc/fse_flow_group.h"
#include "modules/congestion_controller/goog_cc/send_side_bandwidth_estimation.h"


namespace webrtc {

class FseFlow {
public:
    FseFlow(int priority,
            DataRate fse_rate,
            DataRate desired_rate,
            SendSideBandwidthEstimation& cc);
    ~FseFlow();
    DataRate GetFseRate() const;
    void SetFseRate(DataRate new_rate);
    DataRate GetDesiredRate() const;
    void SetDesiredRate(DataRate new_rate);
    int GetPriority() const;
private:
    int priority_;
    DataRate fse_rate_;
    DataRate desired_rate_;
    SendSideBandwidthEstimation& cc_;
};

}

#endif //MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_FLOW_H
