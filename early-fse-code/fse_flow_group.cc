
#include "modules/congestion_controller/goog_cc/fse_flow_group.h"

#include <unordered_set>

#include "absl/strings/match.h"
#include "api/units/data_rate.h"
#include "api/units/timestamp.h"
#include "modules/congestion_controller/goog_cc/fse_flow.h"

namespace webrtc {

FseFlowGroup::FseFlowGroup()
    : sum_calculated_rates_(DataRate::Zero())
    {}

FseFlowGroup::~FseFlowGroup() {}

void FseFlowGroup::AddFlow(std::shared_ptr<FseFlow> flow) {
    flows_.insert(flow); //TODO: check if this is correct
    sum_calculated_rates_ = sum_calculated_rates_ + flow->GetFseRate();
}

void FseFlowGroup::RemoveFlow(std::shared_ptr<FseFlow> flow) {
    flows_.erase(flow);
    sum_calculated_rates_ = sum_calculated_rates_ - flow->GetFseRate();
}

void FseFlowGroup::OnFlowUpdated(std::shared_ptr<FseFlow> flow, DataRate cc_rate, DataRate desired_rate, Timestamp at_time) {
    //a. update S_CR
    sum_calculated_rates_ = sum_calculated_rates_ + cc_rate - flow->GetFseRate();
    //b. calculate the sum of all priorities and initialize FSE_R
    int sum_priorities = 0;
    for (auto i = flows_.begin(); i != flows_.end(); ++i) {
        sum_priorities += (*i)->GetPriority();
        (*i)->SetFseRate(DataRate::Zero());
    }
    //c.Distribute S_CR among all flows, ensuring desired rate is not exceeded
    DataRate total_leftover_rate = sum_calculated_rates_;
    DataRate aggregate_rate = DataRate::Zero(); //TODO: What should this value really be in the beginning?
    while(total_leftover_rate - aggregate_rate > DataRate::Zero() && sum_priorities > 0) {
        aggregate_rate = DataRate::Zero();
        for (auto i = flows_.begin(); i != flows_.end(); ++i) {
            if((*i)->GetFseRate() < (*i)->GetDesiredRate()) {
                if(total_leftover_rate.kbps() * ((*i)->GetPriority()) / sum_priorities >= (*i)->GetDesiredRate().kbps()) {
                    total_leftover_rate -= (*i)->GetDesiredRate();
                    (*i)->SetFseRate((*i)->GetDesiredRate());
                    sum_priorities -= (*i)->GetPriority();
                }
                else {
                    (*i)->SetFseRate(DataRate::KilobitsPerSec(total_leftover_rate.kbps() * ((*i)->GetPriority()) / sum_priorities));
                    aggregate_rate = DataRate::KilobitsPerSec(aggregate_rate.kbps() + total_leftover_rate.kbps() * ((*i)->GetPriority()) / sum_priorities);
                }
            }
        }
    }
    //d. distribute FSE_R to all the flows
    for (auto i = flows_.begin(); i != flows_.end(); ++i) {
        //TODO: how to send it????
    }
}

}