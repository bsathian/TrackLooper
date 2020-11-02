#ifndef ANALYSIS_INTERFACE_TRACKLET_BASE_H
#define ANALYSIS_INTERFACE_TRACKLET_BASE_H

#include <iomanip>
#include <functional>

#include "Module.h"
#include "Segment.h"

namespace SDL
{
    class Module;
    class TrackletBase;
    class Segment;

    class TrackletBase
    {
        //abstract class of segments - can be triplet or tracklet

        protected: //so that we can public inherit these
            Segment* innerSegmentPtr_;
            Segment* outerSegmentPtr_;

        public:
            TrackletBase();
            TrackletBase(Segment* innerSegmentPtr, Segment* outerSegmentPtr);
            virtual ~TrackletBse();

            Segment* innerSegmentPtr() const;
            Segment* outerSegmentPtr() const;
    };
}
#endif
