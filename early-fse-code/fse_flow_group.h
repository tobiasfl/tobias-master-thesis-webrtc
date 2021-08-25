//
// Created by tobias on 21.04.2021.
//

#ifndef MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_FLOW_GROUP_H
#define MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_FLOW_GROUP_H

#include <unordered_set>

#include "absl/strings/match.h"
#include "api/units/data_rate.h"
#include "api/units/timestamp.h"
#include "modules/congestion_controller/goog_cc/fse_flow.h"

namespace webrtc {

class FseFlowGroup {
public:
    FseFlowGroup();
    ~FseFlowGroup();
    void AddFlow(std::shared_ptr<FseFlow> flow);
    void RemoveFlow(std::shared_ptr<FseFlow> flow);
    void OnFlowUpdated(std::shared_ptr<FseFlow> flow, DataRate cc_rate, DataRate desired_rate, Timestamp at_time);
private:
    DataRate sum_calculated_rates_;
    std::unordered_set<std::shared_ptr<FseFlow>> flows_;
};

}

#endif //MODULES_CONGESTION_CONTROLLER_GOOG_CC_FSE_FLOW_GROUP_H
