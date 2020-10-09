# include "Module.h"

SDL::Module::Module()
{

}

SDL::Module::Module(unsigned int detId, short layer, short ring, short rod, short module, bool isInverted, bool isLower, SubDet subdet, ModuleType moduleType, short side)
{
    detId_ = detId;
    layer_ = layer;
    module_ = module;
    rod_ = rod;
    ring_ = ring;
    isInverted_ = isInverted;
    isLower_ = isLower;
    subdet_ = subdet;
    moduleType_ = moduleType;
    side_ = side;
}

const unsigned int& SDL::Module::detId()
{
    return detId_;
}

const short& SDL::Module::subdet()
{
    return subdet_;
}

const short& SDL::Module::side()
{
    return side_;
}

const short& SDL::Module::layer()
{
    return layer_;
}

const short& SDL::Module::rod()
{
    return rod_;
}

const short& SDL::Module::ring()
{
    return ring_;
}

const short& SDL::Module::module()
{
    return module_;
}

const bool& SDL::Module::isLower()
{
    return isLower_;
}

const bool& SDL::Module::isInverted()
{
    return isInverted_;
}

const SDL::Module::ModuleType& SDL::Module::moduleType()
{
    return moduleType_;
}

const SDL::Module::ModuleLayerType& SDL::Module::moduleLayerType()
{
    return moduleLayerType_;
}

const std::vector<Hit*>& SDL::Module::getHitPtrs()
{
    return hits_;
}

const std::vector<MiniDoublet*>& SDL::Module::getMiniDoubletPtrs()
{
    return miniDoublets_;
}

const std::vector<Segment*>& SDL::Module::getSegmentPtrs()
{
    return segments_;
}

const std::vector<Tracklet*>& SDL::Module::getTrackletPtrs()
{
    return tracklets_;
}

void SDL::Module::addHit(Hit* hit)
{
    hits_.push_back(hit);
}

void SDL::Module::addMiniDoublet(MiniDoublet* md)
{
    miniDoublets_.push_back(md);
}

void SDL::Module::addSegment(Segment* sg)
{
    segments_.push_back(sg);
}

void SDL::Module::addTracklet(Tracklet* tp)
{
    tracklets_.push_back(tp);
}

