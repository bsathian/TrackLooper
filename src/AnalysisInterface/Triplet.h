#ifndef ANALYSIS_INTERFACE_TRIPLET_H
#define ANALYSIS_INTERFACE_TRIPLET_H

#include <vector>
#include <map>
#include <tuple>

#include "Module.h"
#include "TrackletBase.h"

namespace SDL
{
    class Module;
    class Triplet;
    class Segment;
    class TrackletBase;
}

namespace SDL
{

    class Triplet : public TrackletBase
    {
        private:
            float zOut_;
            float rtOut_;
            float dPhiPos_;
            float dPhi_;
            float betaIn_;
            float betaOut_;
        public:
            Triplet(float zOut, float rtOut, float dPhiPos, float dPhi, float betaIn, float betaOut, Segment* innerSegment, Segment* outerSegment);
            ~Triplet();

            const float& getDeltaBeta() const;
            const float& getBetaIn() const;
            const float& getBetaOut() const;

    };
}
#endif
