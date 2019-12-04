#ifndef StudySimTrackMatchedSegmentCuts_h
#define StudySimTrackMatchedSegmentCuts_h

# include "Study.h"
# include "SDL/Event.h"
# include "SDL/Segment.h"
# include <vector>
# include <tuple>

# include "TString.h"
# include "trktree.h"
# include "constants.h"

# include "AnalysisConfig.h"

class StudySimtrackMatchedSegmentCuts : public Study
{
    public:
        const char * studyName;

        std::vector<float> dzDiffLowValues;
        std::vector<float> dzDiffHighValues;
        std::vector<float> dPhiValues;
        std::vector<float> dPhiChangeValues;
        std::vector<float> dAlphaInnerMDSegmentValues;
        std::vector<float> dAlphaOuterMDSegmentValues;
        std::vector<float> dAlphaInnerMDOuterMDValues;
        //one vector per layer
        std::vector<std::vector<float>> layerdzDiffLowValues;
        std::vector<std::vector<float>> layerdzDiffHighValues;
        std::vector<std::vector<float>> layerdPhiValues;
        std::vector<std::vector<float>> layerdPhiChangeValues;
        std::vector<std::vector<float>> layerdAlphaInnerMDSegmentValues;
        std::vector<std::vector<float>> layerdAlphaOuterMDSegmentValues;
        std::vector<std::vector<float>> layerdAlphaInnerMDOuterMDValues;

        std::vector<float> barreldzDiffLowValues;
        std::vector<float> barreldzDiffHighValues;
        std::vector<float> barreldPhiValues;
        std::vector<float> barreldPhiChangeValues;
        std::vector<float> barreldAlphaInnerMDSegmentValues;
        std::vector<float> barreldAlphaOuterMDSegmentValues;
        std::vector<float> barreldAlphaInnerMDOuterMDValues;
        //one vector per layer
        std::vector<std::vector<float>> layerBarreldzDiffLowValues;
        std::vector<std::vector<float>> layerBarreldzDiffHighValues;
        std::vector<std::vector<float>> layerBarreldPhiValues;
        std::vector<std::vector<float>> layerBarreldPhiChangeValues;
        std::vector<std::vector<float>> layerBarreldAlphaInnerMDSegmentValues;
        std::vector<std::vector<float>> layerBarreldAlphaOuterMDSegmentValues;
        std::vector<std::vector<float>> layerBarreldAlphaInnerMDOuterMDValues;

        std::vector<float> endcapdrtDiffLowValues;
        std::vector<float> endcapdrtDiffHighValues;
        std::vector<float> endcapdPhiValues;
        std::vector<float> endcapdPhiChangeValues;
        std::vector<float> endcapdAlphaInnerMDSegmentValues;
        std::vector<float> endcapdAlphaOuterMDSegmentValues;
        std::vector<float> endcapdAlphaInnerMDOuterMDValues;
        //one vector per layer
        std::vector<std::vector<float>> layerEndcapdrtDiffLowValues;
        std::vector<std::vector<float>> layerEndcapdrtDiffHighValues;
        std::vector<std::vector<float>> layerEndcapdPhiValues;
        std::vector<std::vector<float>> layerEndcapdPhiChangeValues;
        std::vector<std::vector<float>> layerEndcapdAlphaInnerMDSegmentValues;
        std::vector<std::vector<float>> layerEndcapdAlphaOuterMDSegmentValues;
        std::vector<std::vector<float>> layerEndcapdAlphaInnerMDOuterMDValues;
        //ring
        std::vector<std::vector<float>> ringEndcapdrtDiffLowValues;
        std::vector<std::vector<float>> ringEndcapdrtDiffHighValues;
        std::vector<std::vector<float>> ringEndcapdPhiValues;
        std::vector<std::vector<float>> ringEndcapdPhiChangeValues;
        std::vector<std::vector<float>> ringEndcapdAlphaInnerMDSegmentValues;
        std::vector<std::vector<float>> ringEndcapdAlphaOuterMDSegmentValues;
        std::vector<std::vector<float>> ringEndcapdAlphaInnerMDOuterMDValues;
 
        

        StudySimtrackMatchedSegmentCuts(const char * studyName);
        virtual void bookStudy();
        virtual void resetVariables();
        virtual void doStudy(SDL::Event &event, std::vector<std::tuple<unsigned int, SDL::Event*>> simtrkevents);

};

#endif
