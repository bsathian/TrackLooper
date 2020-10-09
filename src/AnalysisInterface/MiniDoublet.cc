# include "MiniDoublet.h"

SDL::MiniDoublet::MiniDoublet(float dz, float drt, float dphi, float dphichange, float dphinoshift, float dphichangenoshift, SDL::Hit* lowerHitPtr, SDL::Hit* upperHitPtr)
{
    dz_ = dz;
    drt_ = drt;
    dphi_ = dphi;
    dphichange_ = dphichange;
    dphinoshift_ = dphinoshift;
    dphichangenoshift_ = dphichangenoshift;
    lowerHitPtr_ = lowerHitPtr;
    upperHitPtr_ = upperHitPtr;
}

SDL::Hit* SDL::MiniDoublet::lowerHitPtr()
{
    return lowerHitPtr_;
}

SDL::Hit* SDL::MiniDoublet::upperHitPtr()
{
    return upperHitPtr_;
}

const float& SDL::MiniDoublet::getDz()
{
    return dz_;
}

const float& SDL::MiniDoublet::getDrt()
{
    return drt_;
}

const float& SDL::MiniDoublet::getDeltaPhi()
{
    return dphi_;
}

const float& SDL::MiniDoublet::getDeltaPhiChange()
{
    return dphichange_;
}

const float& SDL::MiniDoublet::getDeltaPhiNoShift()
{
    return dphinoshift_;
}

const float& SDL::MiniDoublet::getDeltaPhiChangeNoShift()
{
    return dphichangenoshift_;
}

