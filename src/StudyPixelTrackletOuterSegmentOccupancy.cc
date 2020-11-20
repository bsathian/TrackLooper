# include "StudyTrackletOccupancy.h"

StudyTrackletOccupancy::StudyTrackletOccupancy(const char* studyName)
{
    studyname = studyName;
}

void StudyTrackletOccupancy::bookStudy()
{

    for(int i = 0; i<6; i++)
    {
        ana.histograms.addHistogram(TString::Format("Pixel_Tracklet_occupancy_in_barrel_for_layer_%d",i+1),5000,0,50000,[&,i](){return BarrelLayerOccupancy.at(i);});

        ana.histograms.addHistogram(TString::Format("Pixel_Tracklet_occupancy_in_endcap_for_layer_%d",i+1),5000,0,50000,[&,i](){return EndcapLayerOccupancy.at(i);});

        for(int j = 0; j<15; j++)
        {
            ana.histograms.addHistogram(TString::Format("Pixel_Tracklet_occupancy_layer_%d_ring_%d",i+1,j+1),5000,0,50000,[&,i,j](){return EndcapLayerRingTrackletOccupancy[i][j];});
        }

    }

    for(int i=0;i<15;i++)
    {
      ana.histograms.addHistogram(TString::Format("Pixel_Tracklet_occupancy_in_endcap_for_ring_%d",i+1),5000,0,50000,[&,i](){return EndcapRingOccupancy.at(i);});
    }

}

void StudyPixelTrackletOccupancy::resetArrays()
{
    BarrelLayerOccupancy.clear();
    BarrelLayerOccupancy = {0,0,0,0,0,0};
    EndcapLayerOccupany.clear();
    EndcapLayerOccupancy = {0,0,0,0,0};

    EndcapRingOccupancy.clear();
    EndcapLayerRingOccupancy.clear();
    for(size_t i = 0; i< 15; i++)
    {
        EndcapRingOccupancy.push_back(0);
        EndcapLayerRingOccupancy.push_back(std::vector<float>());
        for(size_t j = 0; j < 5; j++)
        {
            EndcapLayerRingOccupancy[i] = 0;
        }
    }
}


void StudyTrackletOccupancy::doStudy(SDL::EventForAnalysisInterface&event, std::vector<std::tuple<unsigned int, SDL::EventForAnalysisInterface*>> simtrkevents)
{
    resetArrays();
    SDL::Layer& pixelLayer = event.getPixelLayer();


    for(auto & trackletPtr : pixelLayer.getTrackletPtrs())
    {
        Module* module = &(trackletPtr()->outerSegmentPtr()->innerMiniDoubletPtr()->lowerHitPtr()->getModule());
        if(module->subdet() == SDL::Module::Barrel)
        {
            ++BarrelLayerOccupancy.at(module->layer()-1);
        }
        else if(module->subdet() == SDL::Module::Endcap)
        {
            ++EndcapLayerRingTrackletOccupancy.at[module->layer()-1][module->ring()-1];
            ++EndcapLayerOccupancy.at(module->layer()-1);
            ++EndcapRingOccupancy.at(module->ring()-1);
        }

    }
}
