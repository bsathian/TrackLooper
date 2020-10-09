# include "Hit.h"

SDL::Hit::Hit(float x, float y, float z, float phi, float rt, const Module* modulePtr)
{
    x_ = x;
    y_ = y;
    z_ = z;
    phi_ = phi;
    rt_ = rt;
    modulePtr_ = modulePtr;
}

SDL::Hit::~Hit()
{

}

const float SDL::Hit::x()
{
    return x_;
}

const float SDL::Hit::y()
{
    return y_;
}

const float SDL::Hit::z()
{
    return z_;
}

const float SDL::Hit::rt()
{
    return rt_;
}

const float SDL::Hit::phi()
{
    return phi_;
}

const SDL::Module& SDL::Hit::getModule()
{
    return modulePtr_;
}

const Hit* SDL::Hit::getHighEdgePtr()
{
    return hit_high_edge_;
}

const Hit* SDL::Hit::getLowEdgePtr()
{
    return hit_low_edge_;
}

void SDL::Hit::setHighEdgePtr(SDL::Hit* hitHighEdge)
{
    hit_high_edge_ = hitHighEdge;
}

void SDL::Hit::setLowEdgePtr(SDL::Hit* hitLowEdge)
{
    hit_low_edge_ = hitLowEdge;
}

