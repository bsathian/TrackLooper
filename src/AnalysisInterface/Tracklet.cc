#include "Tracklet.h"

SDL::Tracklet::Tracklet(float zOut, float rtOut, float dPhiPos, float dPhi, float betaIn, float betaOut, float betaInCut, float betaOutCut, float deltaBetaCut, SDL::Segment* innerSegment, SDL::Segment* outerSegment)
{
    TrackletBase(innerSegmentPtr, outerSegmentPtr);
    zOut_ = zOut;
    rtOut_ = rtOut;
    dPhiPos_ = dPhiPos;
    dPhi_ = dPhi;
    betaIn_ = betaIn;
    betaOut_ = betaOut;
    betaInCut_ = betaInCut;
    betaOutCut_ = betaOutCut;
    deltaBetaCut_ = deltaBetaCut;
    
}

const float& SDL::Tracklet::getDeltaBeta() const
{
    return fabs(betaIn_ - betaOut_);
}

const float& SDL::Tracklet::getDeltaBetaCut() const
{
    return deltaBetaCut_;
}

const float& SDL::Tracklet::getBetaIn() const
{
    return betaIn_;
}

const float& SDL::Tracklet::getBetaOut() const
{
    return betaOut_;
}

const float& SDL::Tracklet::getBetaInCut() const
{
    return betaInCut_;
}

const float& SDL::Tracklet::getBetaOutCut() const
{
    return betaOutCut_;
}

