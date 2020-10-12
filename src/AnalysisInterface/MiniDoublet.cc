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

SDL::Hit* SDL::MiniDoublet::lowerHitPtr() const
{
    return lowerHitPtr_;
}

SDL::Hit* SDL::MiniDoublet::upperHitPtr() const
{
    return upperHitPtr_;
}

const float& SDL::MiniDoublet::getDz() const
{
    return dz_;
}

const float& SDL::MiniDoublet::getDrt() const
{
    return drt_;
}

const float& SDL::MiniDoublet::getDeltaPhi() const
{
    return dphi_;
}

const float& SDL::MiniDoublet::getDeltaPhiChange() const
{
    return dphichange_;
}

const float& SDL::MiniDoublet::getDeltaPhiNoShift() const
{
    return dphinoshift_;
}

const float& SDL::MiniDoublet::getDeltaPhiChangeNoShift() const
{
    return dphichangenoshift_;
}

void SDL::MiniDoublet::setAnchorHit()
{
    const SDL::Module& lowerModule = lowerHitPtr()->getModule();

    // Assign anchor hit pointers based on their hit type
    if (lowerModule.moduleType() == SDL::Module::PS)
    {
        if (lowerModule.moduleLayerType() == SDL::Module::Pixel)
        {
            anchorHitPtr_ = lowerHitPtr();
        }
        else
        {
            anchorHitPtr_ = upperHitPtr();
        }
    }
    else
    {
        anchorHitPtr_ = lowerHitPtr();
    }
}
