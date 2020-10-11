#include "Segment.h"

SDL::Segment::Segment(float zIn, float zOut, float rtIn, float rtOut, float dphi, float dphiMin, float dphiMax, float dphichange, float dphichangeMin, float dphichnageMax, float dAlphaInnerMDSegment, float dAlphaOuterMDSegment, float dAlphaInnerMDOuterMD, SDL::Minidoublet* innerMDPtr, SDL::MiniDoublet* outerMDPtr)
{
    zOut_ = zOut;
    zIn_ = zIn;
    rtOut_ = rtOut;
    dphi_ = dphi;
    dphiMin_ = dphiMin;
    dphiMax_ = dphiMax;
    dphichange_ = dphichange;
    dphichangeMin_ = dphichangeMin;
    dphichangeMax_ = dphichangeMax;
    dAlphaInnerMDSegment_ = dAlphaInnerMDSegment;
    dAlphaOuterMDSegment_ = dAlphaOuterMDSegment;
    dAlphaInnerMDOuterMD_ = dAlphaInnerMDOuterMD;
    innerMDPtr_ = innerMDPtr;
    outerMDPtr_ = outerMDPtr;
}

SDL::MiniDoublet* SDL::Segment::innerMiniDoubletPtr()
{
    return innerMDPtr_;
}

SDL::MiniDoublet SDL::Segment::outerMiniDoubletPtr()
{
    return outerMDPtr_;
}

const float& SDL::Segment::getRtOut()
{
    return rtOut_;
}

const float& SDL::Segment::getRtIn()
{
    return rtIn_;
}

const float& SDL::Segment::getDeltaPhi()
{
    return dphi_;
}

const float& SDL::Segment::getDeltaPhiMin()
{
    return dphiMin_;
}

const float& SDL::Segment::getDeltaPhiChange()
{
    return dphichange_;
}

const float& SDL::Segment::getDeltaPhiMinChange()
{
    return dphichangeMin_;
}

const float& SDL::Segment::getDeltaPhiMaxChange()
{
    return dphichangeMax_;
}

const float& SDL::Segment::getZOut()
{
    return zOut_;
}

const float& SDL::Segment::getZIn()
{
    return zIn_;
}

