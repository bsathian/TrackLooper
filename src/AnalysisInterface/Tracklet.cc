#include "Tracklet.h"

SDL::Tracklet::Tracklet(float zOut, float rtOut, float dPhiPos, float dPhi, float betaIn, float betaOut, float betaInCut, float betaOutCut, float deltaBetaCut, SDL::Segment* innerSegment, SDL::Segment* outerSegment)
{
    zOut_ = zOut;
    rtOut_ = rtOut;
    dPhiPos_ = dPhiPos;
    dPhi_ = dPhi;
    betaIn_ = betaIn;
    betaOut_ = betaOut;
    betaInCut_ = betaInCut;
    betaOutCut_ = betaOutcut;
    deltaBetaCut_ = deltaBetaCut;
    innerSegment_ = innerSegment;
    outerSegment_ = outerSegment;
    
}

const float& SDL::Tracklet::getDeltaBeta()
{
    return fabs(betaIn_ - betaOut_);
}

const float& SDL::Tracklet::getDeltaBetaCut()
{
    return deltaBetaCut_;
}

const float& SDL::Tracklet::getBetaIn()
{
    return betaIn_;
}

const float& SDL::Tracklet::getBetaOut()
{
    return betaOut_;
}

const float& SDL::Tracklet::getBetaInCut()
{
    return betaInCut_;
}

const float& SDL::Tracklet::getBetaOutCut()
{
    return betaOutCut_;
}

const SDL::Segment* SDL::Tracklet::getInnerSegmentPtr()
{
    return innerSegment_;
}

const SDL::Segment* SDL::Tracklet::getOuterSegmentPtr()
{
    return outerSegment_;
}

