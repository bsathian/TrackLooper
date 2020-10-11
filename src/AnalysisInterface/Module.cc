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
    side_ = (SDL::Module::Side)side;
}

SDL::Module::Module(unsigned int detId)
{
    detId_ = detId;
    
}
unsigned short SDL::Module::parseSubdet(unsigned int detId)
{
    return (detId & (7 << 25)) >> 25;
}

unsigned short SDL::Module::parseSide(unsigned int detId)
{
    if (parseSubdet(detId) == SDL::Module::Endcap)
    {
        return (detId & (3 << 23)) >> 23;
    }
    else if (parseSubdet(detId) == SDL::Module::Barrel)
    {
        return (detId & (3 << 18)) >> 18;
    }
    else
    {
        return 0;
    }
}

unsigned short SDL::Module::parseLayer(unsigned int detId)
{
    if (parseSubdet(detId) == SDL::Module::Endcap)
    {
        return (detId & (7 << 18)) >> 18;
    }
    else if (parseSubdet(detId) == SDL::Module::Barrel)
    {
        return (detId & (7 << 20)) >> 20;
    }
    else
    {
        return 0;
    }
}

unsigned short SDL::Module::parseRod(unsigned int detId)
{
    if (parseSubdet(detId) == SDL::Module::Endcap)
    {
        return 0;
    }
    else if (parseSubdet(detId) == SDL::Module::Barrel)
    {
        return (detId & (127 << 10)) >> 10;
    }
    else
    {
        return 0;
    }
}

unsigned short SDL::Module::parseRing(unsigned int detId)
{
    if (parseSubdet(detId) == SDL::Module::Endcap)
    {
        return (detId & (15 << 12)) >> 12;
    }
    else if (parseSubdet(detId) == SDL::Module::Barrel)
    {
        return 0;
    }
    else
    {
        return 0;
    }

}

unsigned short SDL::Module::parseModule(unsigned int detId)
{
    return (detId & (127 << 2)) >> 2;
}

unsigned short SDL::Module::parseIsLower(unsigned int detId)
{
    return ((parseIsInverted(detId)) ? !(detId & 1) : (detId & 1));
}

bool SDL::Module::parseIsInverted(unsigned int detId)
{
    if (parseSubdet(detId) == SDL::Module::Endcap)
    {
        if (parseSide(detId) == SDL::Module::NegZ)
        {
            return parseModule(detId) % 2 == 1;
        }
        else if (parseSide(detId) == SDL::Module::PosZ)
        {
            return parseModule(detId) % 2 == 0;
        }
        else
        {
            return 0;
        }
    }
    else if (parseSubdet(detId) == SDL::Module::Barrel)
    {
        if (parseSide(detId) == SDL::Module::Center)
        {
            if (parseLayer(detId) <= 3)
            {
                return parseModule(detId) % 2 == 1;
            }
            else if (parseLayer(detId) >= 4)
            {
                return parseModule(detId) % 2 == 0;
            }
            else
            {
                return 0;
            }
        }
        else if (parseSide(detId) == SDL::Module::NegZ or parseSide(detId) == SDL::Module::PosZ)
        {
            if (parseLayer(detId) <= 2)
            {
                return parseModule(detId) % 2 == 1;
            }
            else if (parseLayer(detId) == 3)
            {
                return parseModule(detId) % 2 == 0;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

unsigned int SDL::Module::parsePartnerDetId(unsigned int detId)
{
    if (parseIsLower(detId))
        return ((parseIsInverted(detId)) ? detId - 1 : detId + 1);
    else
        return ((parseIsInverted(detId)) ? detId + 1 : detId - 1);
}

SDL::Module::ModuleType SDL::Module::parseModuleType(unsigned int detId)
{
    if (parseSubdet(detId) == SDL::Module::Barrel)
    { 
        if (parseLayer(detId) <= 3)
            return SDL::Module::PS;
        else
            return SDL::Module::TwoS;
    }
    else
    {
        if (parseLayer(detId) <= 2)
        {
            if (parseRing(detId) <= 10)
                return SDL::Module::PS;
            else
                return SDL::Module::TwoS;
        }
        else
        {
            if (parseRing(detId) <= 7)
                return SDL::Module::PS;
            else
                return SDL::Module::TwoS;
        }
    }
}

SDL::Module::ModuleLayerType SDL::Module::parseModuleLayerType(unsigned int detId)
{
    if (parseModuleType(detId) == SDL::Module::TwoS)
        return SDL::Module::Strip;
    if (parseIsInverted(detId))
    {
        if (parseIsLower(detId))
            return SDL::Module::Strip;
        else
            return SDL::Module::Pixel;
    }
    else
    {
        if (parseIsLower(detId))
            return SDL::Module::Pixel;
        else
            return SDL::Module::Strip;
    }
}

void SDL::Module::setDerivedQuantities()
{
    subdet_ = parseSubdet(detId_);
    side_ = parseSide(detId_);
    layer_ = parseLayer(detId_);
    rod_ = parseRod(detId_);
    ring_ = parseRing(detId_);
    module_ = parseModule(detId_);
    isLower_ = parseIsLower(detId_);
    isInverted_ = parseIsInverted(detId_);
    partnerDetId_ = parsePartnerDetId(detId_);
    moduleType_ = parseModuleType(detId_);
    moduleLayerType_ = parseModuleLayerType(detId_);
}

const unsigned int& SDL::Module::detId() const
{
    return detId_;
}

const short& SDL::Module::subdet() const
{
    return subdet_;
}

const short& SDL::Module::side() const
{
    return side_;
}

const short& SDL::Module::layer() const
{
    return layer_;
}

const short& SDL::Module::rod() const
{
    return rod_;
}

const short& SDL::Module::ring() const
{
    return ring_;
}

const short& SDL::Module::module() const
{
    return module_;
}

const bool& SDL::Module::isLower() const
{
    return isLower_;
}

const bool& SDL::Module::isInverted() const
{
    return isInverted_;
}

const SDL::Module::ModuleType& SDL::Module::moduleType() const
{
    return moduleType_;
}

const SDL::Module::ModuleLayerType& SDL::Module::moduleLayerType() const
{
    return moduleLayerType_;
}

const std::vector<SDL::Hit*>& SDL::Module::getHitPtrs() const
{
    return hits_;
}

const std::vector<SDL::MiniDoublet*>& SDL::Module::getMiniDoubletPtrs() const
{
    return miniDoublets_;
}

const std::vector<SDL::Segment*>& SDL::Module::getSegmentPtrs() const
{
    return segments_;
}

const std::vector<SDL::Tracklet*>& SDL::Module::getTrackletPtrs() const
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

