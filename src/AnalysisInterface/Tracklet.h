#ifndef ANALYSIS_INTERFACE_TRACKLET_H
#define ANALYSIS_INTERFACE_TRACKLET_H

#include <vector>
#include <map>
#include <tuple>

#include "Module.h"
#include "Segment.h"
#include "TrackletBase.h"

namespace SDL
{
    class Module;
    class Tracklet;
    class Segment;
    class TrackletBase;
}

namespace SDL
{

    class Tracklet : public TrackletBase
    {
        private:
            float zOut_;
            float rtOut_;
            float dPhiPos_;
            float dPhi_;
            float betaIn_;
            float betaOut_;
            
            float zLo_;
            float zHi_;
            float zLoPointed_;
            float zHiPointed_;
            float sdlCut_;
            float betaInCut_;
            float betaOutCut_;
            float deltaBetaCut_;
            float rtLo_;
            float rtHi_;
            float kZ_;

        public:
            Tracklet(float zOut, float rtOut, float dPhiPos, float dPhi, float betaIn, float betaOut, float zLo, float zHi, float zLoPointed, float zHiPointed, float sdlCut, float betaInCut, float betaOutCut, float deltaBetaCut, float rtLo, float rtHi, float kZ, Segment* innerSegment, Segment* outerSegment);
            ~Tracklet();

            const float& getDeltaBeta() const;
            const float& getBetaIn() const;
            const float& getBetaOut() const;

    };
}

#endif
