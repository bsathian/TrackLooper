# include "Hit.h"

SDL::Hit::Hit(float x, float y, float z, float phi, float rt, int idx, const Module* modulePtr)
{
    x_ = x;
    y_ = y;
    z_ = z;
    phi_ = phi;
    rt_ = rt;
    idx_ = idx;
    modulePtr_ = modulePtr;
}

SDL::Hit::~Hit()
{

}

const int& SDL::Hit::idx() const
{
    return idx_;
}

const float& SDL::Hit::x() const
{
    return x_;
}

const float& SDL::Hit::y() const
{
    return y_;
}

const float& SDL::Hit::z() const
{
    return z_;
}

const float& SDL::Hit::rt() const
{
    return rt_;
}

const float& SDL::Hit::phi() const
{
    return phi_;
}

const SDL::Module& SDL::Hit::getModule() const
{
    return *modulePtr_;
}

const SDL::Hit* SDL::Hit::getHitHighEdgePtr() const
{
    return hit_high_edge_;
}

const SDL::Hit* SDL::Hit::getHitLowEdgePtr() const
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

