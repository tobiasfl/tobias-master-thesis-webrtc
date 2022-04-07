//
// Created by tobias on 15.02.2022.
//
#include "modules/congestion_controller/goog_cc/fse_config.h"

#include "base/feature_list.h"

#include "rtc_base/diagnostic_logging.h"
#include "rtc_base/logging.h"

namespace webrtc {

/*
const struct FseNgExperimentConfig defaultExperimentConfig = {
    1, 
    1, 
    1, 
    1, 
    DataRate::KilobitsPerSec(1000000), 
    DataRate::KilobitsPerSec(1000000)};
*/

const base::Feature kFse {
  "Fse", base::FEATURE_DISABLED_BY_DEFAULT
};

const base::Feature kFseNg {
  "FseNg", base::FEATURE_DISABLED_BY_DEFAULT
};

const base::Feature kFseV2 {
  "FseV2", base::FEATURE_DISABLED_BY_DEFAULT
};

const base::Feature kFseNgV2 {
  "FseNgV2", base::FEATURE_DISABLED_BY_DEFAULT
};

const base::Feature kDesiredRateFseNgPaperCase {
  "DesiredRateFseNgPaper", base::FEATURE_DISABLED_BY_DEFAULT
};

const base::Feature kDesiredRateAllLimited {
  "DesiredRateAllLimited", base::FEATURE_DISABLED_BY_DEFAULT
};

const base::Feature kPriorityRateFlowDouble {
  "PriorityRateFlowDouble", base::FEATURE_DISABLED_BY_DEFAULT
};

const base::Feature kPriorityCwndFlowDouble {
  "PriorityCwndFlowDouble", base::FEATURE_DISABLED_BY_DEFAULT
};

const base::Feature kPriorityVaried {
  "PriorityVaried", base::FEATURE_DISABLED_BY_DEFAULT
};

const base::Feature kDesiredRateFseCase2 {
  "DesiredRateFseCase2", base::FEATURE_DISABLED_BY_DEFAULT
};

const base::Feature kPriorityFseCase3 {
  "PriorityFseCase3", base::FEATURE_DISABLED_BY_DEFAULT
};

FseConfig::FseConfig() {
  current_fse_ = none;
  if (base::FeatureList::IsEnabled(kFse)) {
    RTC_LOG(LS_INFO) << "Fse is enabled";
    current_fse_ = fse;
  }

  if (base::FeatureList::IsEnabled(kFseNg)) {
    RTC_LOG(LS_INFO) << "FseNg is enabled";
    current_fse_ = fse_ng;
  }

  if (base::FeatureList::IsEnabled(kFseV2)) {
    RTC_LOG(LS_INFO) << "FseV2 is enabled";
    current_fse_ = fse_v2;
  }

  if (base::FeatureList::IsEnabled(kFseNgV2)) {
    RTC_LOG(LS_INFO) << "FseNgV2 is enabled";
    current_fse_ = fse_ng_v2;
  }

  current_desired_rate_case_ = infinity;
  if (base::FeatureList::IsEnabled(kDesiredRateFseNgPaperCase)) {
    current_desired_rate_case_ = fse_ng_paper_case;
  }
  if (base::FeatureList::IsEnabled(kDesiredRateFseCase2)) {
    current_desired_rate_case_ = fse_case_2;
  }
  if (base::FeatureList::IsEnabled(kDesiredRateAllLimited)) {
    current_desired_rate_case_ = all_limited;
  }

  current_priority_case_ = equal;
  if (base::FeatureList::IsEnabled(kPriorityRateFlowDouble)) {
    current_priority_case_ = rate_flow_double;
  }
  if (base::FeatureList::IsEnabled(kPriorityCwndFlowDouble)) {
    current_priority_case_ = cwnd_flow_double;
  }
  if (base::FeatureList::IsEnabled(kPriorityVaried)) {
    current_priority_case_ = varied;
  }
  if (base::FeatureList::IsEnabled(kPriorityFseCase3)) {
    current_priority_case_ = fse_case_3;
  }

  RTC_LOG(LS_INFO) 
      << "FseConfig initialized with fse version=" << current_fse_
      << " desired rate case=" << current_desired_rate_case_
      << " priority case=" << current_priority_case_;
}

DataRate FseConfig::ResolveDesiredRate(int flow_id) {
  switch (current_desired_rate_case_) {
    case infinity:
      return DataRate::Infinity();
    case fse_ng_paper_case:
      return DataRate::KilobitsPerSec(1500);
    case all_limited:
      return DataRate::KilobitsPerSec(1000);
    case fse_case_2:
      switch (flow_id) {
        case 0:
          return DataRate::KilobitsPerSec(1000);
        case 1:
          return DataRate::Infinity();
        default:
          return DataRate::Infinity(); 
      }
  }
}

int FseConfig::ResolveRateFlowPriority(int flow_id) {
  switch (current_priority_case_) {
    case equal:
        //TODO: testing only
      return 1;
    case rate_flow_double:
      return 2;
    case varied:
      switch (flow_id) {
        case 0:
          return 1;
        case 1:
          return 3;
        default:
          return 1;
      }
    case fse_case_3:
      switch (flow_id) {
        case 0:
          return 1;
        case 1:
          return 2;
        default:
          return 1;
      }
    default:
      return 1;
  }
}

int FseConfig::ResolveCwndFlowPriority(int flow_id) {
  switch (current_priority_case_) {
    case equal:
      //TODO: testing only
      return 1;
    case cwnd_flow_double:
      return 2;
    case varied:
      return 2;
    default:
      return 1;
  }
}

FseVersion FseConfig::CurrentFse() {
  return current_fse_;
}

FseConfig& FseConfig::Instance() {
  CR_DEFINE_STATIC_LOCAL(FseConfig, s, ());
  return s;
}

}  // namespace webrtc
