#include "Tracklet.h"

SDL::Tracklet::Tracklet(float zOut, float rtOut, float dPhiPos, float dPhi, float betaIn, float betaOut, SDL::Segment* innerSegment, SDL::Segment* outerSegment)
{
    TrackletBase(innerSegmentPtr, outerSegmentPtr);
    zOut_ = zOut;
    rtOut_ = rtOut;
    dPhiPos_ = dPhiPos;
    dPhi_ = dPhi;
    betaIn_ = betaIn;
    betaOut_ = betaOut;
    
}

const float& SDL::Tracklet::getDeltaBeta() const
{
    return fabs(betaIn_ - betaOut_);
}


const float& SDL::Tracklet::getBetaIn() const
{
    return betaIn_;
}

const float& SDL::Tracklet::getBetaOut() const
{
    return betaOut_;
}

