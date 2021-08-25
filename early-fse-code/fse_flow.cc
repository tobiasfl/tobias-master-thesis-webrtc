
#include "modules/congestion_controller/goog_cc/fse_flow.h"

#include "api/units/data_rate.h"
#include "modules/congestion_controller/goog_cc/fse_flow_group.h"


namespace webrtc {

FseFlow::FseFlow(int priority,
        DataRate fse_rate,
        DataRate desired_rate,
        SendSideBandwidthEstimation& cc)
        : priority_(priority),
        fse_rate_(fse_rate),
        desired_rate_(desired_rate),
        cc_(cc)
        {}

FseFlow::~FseFlow() {}

DataRate FseFlow::GetFseRate() const {
    return fse_rate_;
}

void FseFlow::SetFseRate(DataRate new_rate) {
    fse_rate_ = new_rate;
}

int FseFlow::GetPriority() const {
    return priority_;
}

DataRate FseFlow::GetDesiredRate() const {
    return desired_rate_;
}

void FseFlow::SetDesiredRate(DataRate new_rate) {
    desired_rate_ = new_rate;
}

}