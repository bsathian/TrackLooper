#ifndef StudyPixelTrackletOuterSegmentOccupancy_h
#define StudyTrackletOuterSegmentOccupancy_h

#include "AnalysisInterface/EventForAnalysisInterface.h"
#include "Study.h"
#include <vector>
#include <tuple>

#include "TString.h"
#include "trktree.h"
#include "constants.h"

#include "AnalysisConfig.h"

class StudyPixelTrackletOccupancy : public Study
{

public:
    const char * studyname;

    std::vector<float> BarrelLayerOccupancy;
    std::vector<float> EndcapLayerOccupancy;
    std::vector<float> EndcapRingOccupancy;

    std::vector<std::vector<float>> EndcapLayerRingTrackletOccupancy;

    StudyTrackletOccupancy(const char* studyName);
    virtual void bookStudy();
    void resetArrays();
    virtual void doStudy(SDL::EventForAnalysisInterface& recoevent, std::vector<std::tuple<unsigned int, SDL::EventForAnalysisInterface*>> simtrkevents);

};

#endif
