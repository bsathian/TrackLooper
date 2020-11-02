#ifndef ANALYSIS_INTERFACE_TRACKLET_H
#define ANALYSIS_INTERFACE_TRACKLET_H

#include <vector>
#include <map>
#include <tuple>

#include "Module.h"
#include "Hit.h"
#include "MiniDoublet.h"

namespace SDL
{
    class Module;
    class Hit;
    class MiniDoublet;
    class Segment;

    class Tracklet
    {
        private:
            float zOut_;
            float rtOut_;
            float dPhiPos_;
            float dPhi_;
            float betaIn_;
            float betaOut_;
            Segment* innerSegment_;
            Segment* outerSegment_;
        public:
            Tracklet(float zOut, float rtOut, float dPhiPos, float dPhi, float betaIn, float betaOut, Segment* innerSegment, Segment* outerSegment);
            ~Tracklet();

            const float& getDeltaBeta() const;
            const float& getBetaIn() const;
            const float& getBetaOut() const;
            const Segment* innerSegmentPtr() const;
            const Segment* outerSegmentPtr() const;

    };
}

#endif
