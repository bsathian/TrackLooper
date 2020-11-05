#include "TrackletBase.h"

SDL::TrackletBase::~TrackletBase()
{
}

SDL::TrackletBase::TrackletBase()
{
}

SDL::TrackletBase::TrackletBase(SDL::Segment* innerSegmentPtr, SDL::Segment* outerSegmentPtr)
{
    innerSegmentPtr_ = innerSegmentPtr;
    outerSegmentPtr_ = outerSegmentPtr;
}

SDL::Segment* SDL::TrackletBase::innerSegmentPtr() const
{
    return innerSegmentPtr_;
}

SDL::Segment* SDL::TrackletBase::outerSegmentPtr() const
{
    return outerSegmentPtr_;
}

