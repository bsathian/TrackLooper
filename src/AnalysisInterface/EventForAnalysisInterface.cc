#include "EventForAnalysisInterface.h"

void SDL::EventForAnalysisInterface::addModulesToAnalysisInterface(struct modules& modulesInGPU)
{
    for(unsigned int idx = 0; idx < modulesInGPU.nModules; idx++)
    {
        moduleMapByIndex_[idx] = new SDL::Module(modulesInGPU.detIds[idx],modulesInGPU.layers[idx], modulesInGPU.rings[idx], modulesInGPU.rods[idx], modulesInGPU.isInverted[idx], modulesInGPU.isLower[idx], modulesInGPU.subdets[idx], modulesInGPU.moduleType[idx], modulesInGPU.moduleLayerType[idx]);

        modulePointers.push_back(moduleMapByIndex_[idx]);
        if(modulesInGPU.isLower[idx])
        {
            lowerModulePointers.push_back(moduleMapByIndex_[idx]);
        }
        detIdToIndex_[modulesInGPU.detIds[idx]] = idx;
    }
}

void SDL::EventForAnalysisInterface::addHitsToAnalysisInterface(struct hits&  hitsInGPU)
{
    for(unsigned int idx = 0; idx < hitsInGPU.nHits; idx++)
    {
        Module* lowerModule = moduleMapByIndex_[moduleIndices[idx]];

        hits_[idx] = new SDL::Hit(hitsInGPU.xs[idx],hitsInGPU.ys[idx],hitsInGPU.zs[idx],hitsInGPU.rts[idx],hitsInGPU.phis[idx], lowerModule);
        hitPointers.push_back(hits_[idx]);
    }
}

void SDL::EventForAnalysisInterface::addMDsToAnalysisInterface(struct miniDoublets& mdsInGPU)
{
    for(unsigned int idx = 0; idx < modulePointers.size(); idx++) 
    {
        for(unsigned int jdx = 0; jdx < mdsInGPU.nMDs[idx]; jdx++)
        {

            unsigned int mdIndex = idx * N_MAX_MDS_PER_MODULE + jdx;
            Hit* lowerHitPtr = hits_[mdsInGPU.hitIndices[2 * mdIndex]];
            Hit* upperHitPtr = hits_[mdsInGPU.hitIndices[2 * mdIndex + 1]];

            miniDoublets_[mdIndex] = new SDL::MiniDoublet(mdsInGPU.dzs[mdIndex], mdsInGPU.drts[mdIndex], mdsInGPU.dphis[mdIndex], mdsInGPU.dphichanges[mdIndex], mdsInGPU.dphinoshifts[mdIndex], mdsInGPU.dphichangenoshifts[mdIndex], lowerHitPtr, upperHitPtr);

            mdPointers.push_back(miniDoublets_[mdIndex]);
        }
    }
}

void SDL::EventForAnalysisInterface::addSegmentsToAnalysisInterface(struct segments& segmentsInGPU)
{
    for(unsigned int idx = 0; idx < modulePointers.size(); idx++)
    {
        for(unsigned int jdx = 0; jdx < segmentsInGPU.nSegments[idx]; idx++)
        {
            unsigned int segmentIndex = idx * N_MAX_SEGMENTS_PER_MODULE + jdx;
            MiniDoublet* lowerMD =miniDoublets_[segmentsInGPU.mdIndices[segmentsIndex * 2]];
            MiniDoublet* upperMD = miniDoublets_[segmentsInGPU.mdIndices[segmentIndex * 2 + 1]];

            segments_[segmentIndex] = new SDL::Segment(segmentsInGPU.zIns[segmentIndex], segmentsInGPU.zOuts[segmentIndex], segmentsInGPU.rtIns[segmentIndex], segmentsInGPU.rtOuts[segmentIndex], segmentsInGPU.dPhis[segmentIndex], segmentsInGPU.dPhiMins[segmentIndex], segmentsInGPU.dPhiMaxs[segmentIndex], segmentsInGPU.dPhiChanges[segmentIndex], segmentsInGPU.dPhiChangeMins[segmentIndex], segmentsInGPU.dAlphaInnerMDSegments[segmentIndex], segmentsInGPU.dAlphaOuterMDSegments[segmentIndex], segmentsInGPU.dAlphaInnerMDOuterMDs[segmentIndex], segmentsInGPU.dPhiChangeMaxs[segmentIndex], lowerMD, upperMD);

            segmentPointers.push_back(segments_[segmentIndex]);
        }
    }
}

void addTrackletsToAnalysisInterface(struct tracklets& trackletsInGPU)
{
    for(unsigned int idx = 0; idx < lowerModulePointers.size(); idx++)
    {
        for(unsigned int jdx = 0; jdx < trackletsInGPU.nTracklets[idx]; jdx++)
        {
            unsigned int trackletIndex = idx * N_MAX_TRACKLETS_PER_MODULE * idx + jdx;
            Segment* innerSegment = segments_[trackletsInGPU.segmentIndices[2 * trackletIndex]];
            Segment* outerSegment = segments_[trackletsInGPU.segmentIndices[2 * trackletIndex + 1]];

            tracklets_[trackletIndex] = new SDL::Tracklet(trackletsInGPU.zOut[trackletIndex], trackletsInGPU.rtOut[trackletIndex], trackletsInGPU.dPhiPos[trackletIndex], trackletsInGPU.dPhi[trackletIndex], trackletsInGPU.betaIn[trackletIndex], trackletsInGPU.betaOut[trackletIndex], trackletsInGPU.betaInCut[trackletIndex], trackletsInGPU.betaOutcut[trackletIndex], trackletsInGPU.deltaBetaCut[trackletIndex], innerSegment, outerSegment);

            trackletPointers.push_back(tracklets_[trackletIndex]);
        }
    }
}

SDL::EventForAnalysisInterface::EventForAnalysisInterface(struct modules& modulesInGPU, struct hits& hitsInGPU, struct miniDoublets& mdsInGPU, struct segments& segmentsInGPU, struct tracklets& trackletsInGPU)
{
    addModulesToAnalysisInterface(modulesInGPU);
    addHitsToAnalysisInterface(hitsInGPU);
    addMDsToAnalysisInterface(mdsInGPU);
    addSegmentsToAnalysisInterface(segmentsInGPU);
    addTrackletsToAnalysisInterface(trackletsInGPU);
}

const std::vector<Module*> SDL::EventsForAnalysisInterface::getModulePtrs()
{
    return modulePointers;
}

const std::vector<Module*> SDL::EventsForAnalysisInterface::getLowerModulePtrs()
{
    return lowerModulePointers;
}
