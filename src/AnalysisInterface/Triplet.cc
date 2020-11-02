#include "Triplet.h"

SDL::Triplet::Triplet(float zOut, float rtOut, float dPhiPos, float dPhi, float betaIn, float betaOut, SDL::Segment* innerSegment, SDL::Segment* outerSegment)
{
    zOut_ = zOut;
    rtOut_ = rtOut;
    dPhiPos_ = dPhiPos;
    dPhi_ = dPhi;
    betaIn_ = betaIn;
    betaOut_ = betaOut;
    innerSegment_ = innerSegment;
    outerSegment_ = outerSegment;
}

const float& SDL::Triplet::getDeltaBeta() const
{
    return fabs(betaIn_ - betaOut_);
}


const float& SDL::Triplet::getBetaIn() const
{
    return betaIn_;
}

const float& SDL::Triplet::getBetaOut() const
{
    return betaOut_;
}

const SDL::Segment* SDL::Triplet::innerSegmentPtr() const
{
    return innerSegment_;
}

const SDL::Segment* SDL::Triplet::outerSegmentPtr() const
{
    return outerSegment_;
}

