#ifndef StudySegmentOccupancy_h
#define StudySegmentOccupancy_h

#include "SDL/Event.h"
#include "Study.h"
#include <vector>
#include <tuple>

#include "TString.h"
#include "trktree.h"
#include "constants.h"

#include "AnalysisConfig.h"

class StudyChargeClassification : public Study
{
    public:
        const char* studyName;        
        std::vector<float> ptbins;

        //Vectors contain the Pts so that histograms can be used to represent the fractions
        std::vector<float> truePositives; 
        std::vector<float> trueNegatives;
        std::vector<float> falsePositives;
        std::vector<float> falseNegatives;

        std::vector<float> truePositivesBarrel;
        std::vector<float> trueNegativesBarrel;
        std::vector<float> falsePositivesBarrel;
        std::vector<float> falseNegativesBarrel;

        std::vector<float> truePositivesEndcap;
        std::vector<float> trueNegativesEndcap;
        std::vector<float> falsePositivesEndcap;
        std::vector<float> falseNegativesEndcap;


        std::vector<std::vector<float>> truePositivesByLayer;
        std::vector<std::vector<float>> trueNegativesByLayer;
        std::vector<std::vector<float>> falsePositivesByLayer;
        std::vector<std::vector<float>> falseNegativesByLayer;

        std::vector<std::vector<float>> truePositivesByBarrelLayer;
        std::vector<std::vector<float>> trueNegativesByBarrelLayer;
        std::vector<std::vector<float>> falsePositivesByBarrelLayer;
        std::vector<std::vector<float>> falseNegativesByBarrelLayer;

        std::vector<std::vector<float>> truePositivesByEndcapLayer;
        std::vector<std::vector<float>> trueNegativesByEndcapLayer;
        std::vector<std::vector<float>> falsePositivesByEndcapLayer;
        std::vector<std::vector<float>> falseNegativesByEndcapLayer;

        std::vector<std::vector<std::vector<float>>> truePositivesByEndcapLayerRing;
        std::vector<std::vector<std::vector<float>>> trueNegativesByEndcapLayerRing;
        std::vector<std::vector<std::vector<float>>> falsePositivesByEndcapLayerRing;
        std::vector<std::vector<std::vector<float>>> falseNegativesByEndcapLayerRing;


        StudyChargeClassification(const char* studyName);
        virtual void bookStudy();
        virtual void resetVariables();
        virtual void doStudy(SDL::Event& recoevent, std::vector<std::tuple<unsigned int, SDL::Event*>> simtrkevents);
};

#endif
