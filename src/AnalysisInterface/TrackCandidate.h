#ifndef ANALYSIS_INTERFACE_TRACKCANDIDATE_H
#define ANALYSIS_INTERFACE_TRACKCANDIDATE_H
#include <stdexcept>
#include "Module.h"
#include "TrackletBase.h"
#include "Tracklet.h"
#include "Triplet.h"

namespace SDL
{
    class Module;
}

namespace SDL
{
    class TrackCandidate
    {
        private:
            TrackletBase* innerTrackletPtr_;
            TrackletBase* outerTrackletPtr_;
            short trackCandidateType_;

        public:
            TrackCandidate(TrackletBase* innerTrackletPtr, TrackletBase* outerTrackletPtr, short trackCandidateType);
            ~TrackCandidate();

            TrackletBase* innerTrackletBasePtr() const;
            TrackletBase* outerTrackletBasePtr() const;
            Tracklet* innerTrackletPtr() const;
            Tracklet* outerTrackletPtr() const;
            Triplet* innerTripletPtr() const;
            Triplet* outerTripletPtr() const;
            short trackCandidateType() const;

    };
}
#endif

