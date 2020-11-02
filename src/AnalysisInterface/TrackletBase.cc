#include "TrackletBase.h"

SDL::TrackletBase::~TrackletBase()
{
}

SDL::trackletBase::TrackletBase(SDL::Segment* innerSegmentPtr, SDL::Segment* outerSegmentPtr)
{
    innerSegmentPtr_(innerSegmentPtr);
    outerSegmentPtr_(outerSegmentPtr);
}

SDL::Segment::innerSegmentPtr() const
{
    return innerSegmentPtr_;
}

SDL::Segment::outerSegmentPtr() const
{
    return outerSegmentPtr_;
}

