#include "TrackCandidate.h"

SDL::TrackCandidate::TrackCandidate(TrackletBase* innerTrackletPtr, TrackletBase* outerTrackletPtr, short TrackCandidateType)
{
    innerTrackletPtr_ = innerTrackletPtr;
    outerTrackletPtr_ = outerTrackletPtr;
    trackCandidateType_ = trackCandidateType;
}

SDL::TrackletBase* SDL::TrackCandidate::innerTrackletBasePtr() const
{
    return innerTrackletPtr;
}

SDL::TrackletBase* SDL::TrackCandidate::outerTrackletBasePtr()
{
    return outerTrackletPtr;
}

SDL::Tracklet* SDL::TrackCandidate::innerTrackletPtr() const
{
    if (dynamic_cast<Tracklet*>(innerTrackletPtr_))
    {
        return dynamic_cast<Tracklet*>(innerTrackletPtr_);
    }
    else
    {
       std::cout << "TrackCandidate::innerTrackletPtr() ERROR - asked for innerTracklet when this TrackCandidate doesn't have one. (maybe it has triplets?)" << std::endl;
        throw std::logic_error("");
    }

}

SDL::Tracklet* SDL::TrackCandidate::outerTrackletPtr() const
{
    if (dynamic_cast<Tracklet*>(outerTrackletPtr_))
    {
        return dynamic_cast<Tracklet*>(outerTrackletPtr_);
    }
    else
    {
        std::cout << "TrackCandidate::outerTrackletPtr() ERROR - asked for outerTracklet when this TrackCandidate doesn't have one. (maybe it has triplets?)" << std::endl;
        throw std::logic_error("");
    }

}

SDL::Triplet* SDL::TrackCandidate::innerTripletPtr() const
{
    if (dynamic_cast<Triplet*>(innerTrackletPtr_))
    {
        return dynamic_cast<Triplet*>(innerTrackletPtr_);
    }
    else
    {
        std::cout << "TrackCandidate::innerTripletPtr() ERROR - asked for innerTriplet when this TrackCandidate doesn't have one. (maybe it has tracklets?)" << std::endl;
        throw std::logic_error("");
    }
}

SDL::Triplet* SDL::TrackCandidate::outerTripletPtr() const
{
    if (dynamic_cast<Triplet*>(outerTrackletPtr_))
    {
        return dynamic_cast<Triplet*>(outerTrackletPtr_);
    }
    else
    {
        std::cout << "TrackCandidate::outerTripletPtr() ERROR - asked for outerTriplet when this TrackCandidate doesn't have one. (maybe it has tracklets?)" << std::endl;
        throw std::logic_error("");
    }
}

short SDL::trackCandidateType() const
{
    return trackCandidateType_;
}

