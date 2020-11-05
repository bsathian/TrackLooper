#ifndef StudyTrackCandidateccupancyT4T4_h
#define StudyTrackCandidateccupancyT4T4_h

#include "AnalysisInterface/EventForAnalysisInterface.h"
#include "Study.h"
#include <vector>
#include <tuple>

#include "TString.h"
#include "trktree.h"
#include "constants.h"

#include "AnalysisConfig.h"

class StudyTrackCandidateOccupancyT4T4 : public Study
{

public:
    const char * studyname;
    std::vector<float> occupancyInBarrel;
    std::vector<float> occupancyInEndcap;
    float averageOccupancyInBarrel;
    float averageOccupancyInEndcap;
    std::vector<float> averageLayerOccupancy;
    std::vector<float> averageBarrelLayerOccupancy;
    std::vector<float> averageEndcapLayerOccupancy;
    std::vector<float> averageEndcapRingOccupancy;

    std::vector<std::vector<float>> LayerOccupancy;
    std::vector<std::vector<float>> BarrelLayerOccupancy;
    std::vector<std::vector<float>> EndcapLayerOccupancy;
    std::vector<std::vector<float>> EndcapRingOccupancy;

    std::vector<std::vector<std::vector<float>>> EndcapLayerRingTrackCandidateT4T4Occupancy;

    StudyTrackCandidateOccupancyT4T4(const char* studyName);
    virtual void bookStudy();
    virtual void doStudy(SDL::EventForAnalysisInterface& recoevent, std::vector<std::tuple<unsigned int, SDL::EventForAnalysisInterface*>> simtrkevents);

};

#endif
