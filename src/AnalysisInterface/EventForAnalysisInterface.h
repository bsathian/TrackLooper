#ifndef ANALYSIS_INTERFACE_EVENT_H
#define ANALYSIS_INTERFACE_EVENT_H

#include <vector>
#include <map>
#include <tuple>

#include "Module.h"
#include "Hit.h"
#include "MiniDoublet.h"
#include "Segment.h"
#include "Tracklet.h"

#include "SDL/Module.cuh"
#include "SDL/Hit.cuh"
#include "SDL/MiniDoublet.cuh"
#include "SDL/Segment.cuh"
#include "SDL/Tracklet.cuh"


namespace SDL
{
    class EventForAnalysisInterface
    {
        private:
            std::map<unsigned int, Module*> moduleMapByIndex_;
            std::map<unsigned int, unsigned int> detIdToIndex_;
            std::map<unsigned int, Hit*> hits_;
            std::map<unsigned int, Hit*> hits_2s_edges_;
            std::map<unsigned int, MiniDoublet*> miniDoublets_;
            std::map<unsigned int, Segment*> segments_;
            std::map<unsigned int, Tracklet*> tracklets_;
        
            std::vector<Module*> modulePointers;
            std::vector<Module*> lowerModulePointers;
            std::vector<Hit*> hitPointers;
            std::vector<MiniDoublet*> mdPointers;
            std::vector<Segment*> segmentPointers;
            std::vector<Tracklet*> trackletPointers;

        public:

            void addModulesToAnalysisInterface(struct modules& modulesInGPU);
            void getModule(unsigned int detId);
            void addHitsToAnalysisInterface(struct hits& hitsInGPU);

            void addMDsToAnalysisInterface(struct miniDoublets& mdsInGPU);
            void addSegmentsToAnalysisInterface(struct segments& segmentsInGPU);
            void addTrackletsToAnalysisInterface(struct tracklets& trackletsInGPU);

        //add the get list of functions here
        const std::vector<Module*> getModulePtrs() const;
        const std::vector<Module*> getLowerModulePtrs() const;
    };
}
#endif
