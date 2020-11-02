#ifndef ANALYSIS_INTERFACE_TRACKLET_H
#define ANALYSIS_INTERFACE_TRACKLET_H

#include <vector>
#include <map>
#include <tuple>

#include "Module.h"
#include "Hit.h"
#include "MiniDoublet.h"
#include "TrackletBase.h"

namespace SDL
{
    class Module;
    class Hit;
    class MiniDoublet;
    class Segment;
    class TrackletBase;

    class Tracklet : public TrackletBase
    {
        private:
            float zOut_;
            float rtOut_;
            float dPhiPos_;
            float dPhi_;
            float betaIn_;
            float betaOut_;
        public:
            Tracklet(float zOut, float rtOut, float dPhiPos, float dPhi, float betaIn, float betaOut, Segment* innerSegment, Segment* outerSegment);
            ~Tracklet();

            const float& getDeltaBeta() const;
            const float& getBetaIn() const;
            const float& getBetaOut() const;

    };
}

#endif
