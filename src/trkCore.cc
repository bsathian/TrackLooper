#include "trkCore.h"

//__________________________________________________________________________________________
float simhit_p(unsigned int simhitidx)
{

    // |momentum| calculation

    float px = trk.simhit_px()[simhitidx];
    float py = trk.simhit_py()[simhitidx];
    float pz = trk.simhit_pz()[simhitidx];
    return sqrt(px*px + py*py + pz*pz);
}

//__________________________________________________________________________________________
float hitAngle(unsigned int simhitidx)
{

    // This is the angle calculation between position vector and the momentum vector

    float x = trk.simhit_x()[simhitidx];
    float y = trk.simhit_y()[simhitidx];
    float z = trk.simhit_z()[simhitidx];
    float r3 = sqrt(x*x + y*y + z*z);
    float px = trk.simhit_px()[simhitidx];
    float py = trk.simhit_py()[simhitidx];
    float pz = trk.simhit_pz()[simhitidx];
    float p = sqrt(px*px + py*py + pz*pz);
    float rdotp = x*px + y*py + z*pz;
    rdotp = rdotp / r3;
    rdotp = rdotp / p;
    float angle = acos(rdotp);
    return angle;
}

//__________________________________________________________________________________________
bool isMuonCurlingHit(unsigned int isimtrk, unsigned int ith_hit)
{

    // To assess whether the ith_hit for isimtrk is a "curling" hit

    // Retrieve the sim hit idx
    unsigned int simhitidx = trk.sim_simHitIdx()[isimtrk][ith_hit];

    // We're only concerned about hits in the outer tracker
    // This is more of a sanity check
    if (not (trk.simhit_subdet()[simhitidx] == 4 or trk.simhit_subdet()[simhitidx] == 5))
        return false;

    // Retrieve the sim hit pdgId
    int simhit_particle = trk.simhit_particle()[simhitidx];

    // If the hit is not muon then we can't tell anything
    if (abs(simhit_particle) != 13)
        return false;

    // Get the angle
    float angle = hitAngle(simhitidx);

    // If the angle is quite different then it's the last hit
    if (abs(angle) > 1.6)
        return true;

    // Afterwards, we check the energy loss
    //
    // If it is the first hit without any previous hit present,
    // we can't tell whether it is last hit or not
    // so we just say false to be conservative
    if (ith_hit == 0)
        return false;

    // Retrieve the module where the hit lies
    int detid = trk.simhit_detId()[simhitidx];
    SDL::Module module = SDL::Module(detid);

    // Calculate the momentum
    float p = simhit_p(simhitidx);

    // Find the previous simhit that is on the lower side of the module
    int simhitidx_previous = -999;
    for (unsigned int ith_back = 1; ith_back <= ith_hit; ith_back++)
    {
        // Retrieve the hit idx of ith_hit - ith_back;
        unsigned int simhitidx_previous_candidate = trk.sim_simHitIdx()[isimtrk][ith_hit-ith_back];

        if (not (trk.simhit_subdet()[simhitidx_previous_candidate] == 4 or trk.simhit_subdet()[simhitidx_previous_candidate] == 5))
            continue;

        if (not (trk.simhit_particle()[simhitidx_previous_candidate] == 13))
            continue;

        // Retrieve the module where the previous candidate hit lies
        int detid = trk.simhit_detId()[simhitidx_previous_candidate];
        SDL::Module module = SDL::Module(detid);

        // If the module is lower, then we get the index
        if (module.isLower())
        {
            simhitidx_previous = simhitidx_previous_candidate;
            break;
        }

    }

    // If no previous layer is found then can't do much
    if (simhitidx_previous == -999)
        return false;

    // Get the previous layer momentum
    float p_previous = simhit_p(simhitidx_previous);

    // Calculate the momentum loss
    float loss = fabs(p_previous - p) / p_previous;

    // If the momentum loss is large it is the last hit
    if (loss > 0.35)
        return true;

    // If it reaches this point again, we're not sure what this hit is
    // So we return false 
    return false;

}

TVector3 r3FromPCA(const TVector3& p3, const float dxy, const float dz){
  const float pt = p3.Pt();
  const float p = p3.Mag();
  const float vz = dz*pt*pt/p/p;

  const float vx = -dxy*p3.y()/pt - p3.x()/p*p3.z()/p*dz;
  const float vy =  dxy*p3.x()/pt - p3.y()/p*p3.z()/p*dz;
  return TVector3(vx, vy, vz);
}



bool hasAll12HitsWithNBarrelUsingModuleMap(unsigned int isimtrk, int nbarrel, bool usesimhits)
{

    // Select only tracks that left all 12 hits in the barrel
    std::array<std::vector<SDL::Module>, 6> layers_modules_barrel; // Watch out for duplicates in this vector, do not count with this for unique count.
    std::array<std::vector<SDL::Module>, 6> layers_modules_endcap; // Watch out for duplicates in this vector, do not count with this for unique count.

    std::vector<float> ps;

    for (unsigned int ith_hit = 0; ith_hit < trk.sim_simHitIdx()[isimtrk].size(); ++ith_hit)
    {

        // Retrieve the sim hit idx
        unsigned int simhitidx = trk.sim_simHitIdx()[isimtrk][ith_hit];

        // Select only the hits in the outer tracker
        if (not (trk.simhit_subdet()[simhitidx] == 4 or trk.simhit_subdet()[simhitidx] == 5))
            continue;

        if (not (trk.simhit_particle()[simhitidx] == trk.sim_pdgId()[isimtrk]))
            continue;

        if (isMuonCurlingHit(isimtrk, ith_hit))
            break;

        if (not usesimhits)
        {

            // list of reco hit matched to this sim hit
            for (unsigned int irecohit = 0; irecohit < trk.simhit_hitIdx()[simhitidx].size(); ++irecohit)
            {
                // Get the recohit type
                int recohittype = trk.simhit_hitType()[simhitidx][irecohit];

                // Consider only ph2 hits (i.e. outer tracker hits)
                if (recohittype == 4)
                {

                    int ihit = trk.simhit_hitIdx()[simhitidx][irecohit];

                    if (trk.ph2_subdet()[ihit] == 5)
                    {
                        layers_modules_barrel[trk.ph2_layer()[ihit] - 1].push_back(SDL::Module(trk.ph2_detId()[ihit]));
                    }
                    if (trk.ph2_subdet()[ihit] == 4)
                    {
                        layers_modules_endcap[trk.ph2_layer()[ihit] - 1].push_back(SDL::Module(trk.ph2_detId()[ihit]));
                    }

                }

            }

        }
        else
        {

            if (trk.simhit_hitIdx()[simhitidx].size() > 0)
            {

                if (trk.simhit_subdet()[simhitidx] == 5)
                {
                    layers_modules_barrel[trk.simhit_layer()[simhitidx] - 1].push_back(SDL::Module(trk.simhit_detId()[simhitidx]));
                }
                if (trk.simhit_subdet()[simhitidx] == 4)
                {
                    layers_modules_endcap[trk.simhit_layer()[simhitidx] - 1].push_back(SDL::Module(trk.simhit_detId()[simhitidx]));
                }
            }

            // // list of reco hit matched to this sim hit
            // for (unsigned int irecohit = 0; irecohit < trk.simhit_hitIdx()[simhitidx].size(); ++irecohit)
            // {
            //     // Get the recohit type
            //     int recohittype = trk.simhit_hitType()[simhitidx][irecohit];

            //     // Consider only ph2 hits (i.e. outer tracker hits)
            //     if (recohittype == 4)
            //     {

            //         int ihit = trk.simhit_hitIdx()[simhitidx][irecohit];

            //         if (trk.ph2_subdet()[ihit] == 5)
            //         {
            //             layers_modules_barrel[trk.ph2_layer()[ihit] - 1].push_back(SDL::Module(trk.ph2_detId()[ihit]));
            //         }
            //         if (trk.ph2_subdet()[ihit] == 4)
            //         {
            //             layers_modules_endcap[trk.ph2_layer()[ihit] - 1].push_back(SDL::Module(trk.ph2_detId()[ihit]));
            //         }

            //     }

            // }

        }

    }

    // Aggregating good pair modules (i.e. a double module with each side having non-zero reco hits.)
    std::array<std::vector<unsigned int>, 6> layers_good_paired_modules; // Watch out for duplicates in this vector, do not count with this for unique count.

    for (int logical_ilayer = 0; logical_ilayer < 6; ++logical_ilayer)
    {
        // Raw layer number
        bool is_ilayer_barrel = logical_ilayer < nbarrel;
        int raw_ilayer = is_ilayer_barrel ? logical_ilayer: logical_ilayer - nbarrel;
        const std::array<std::vector<SDL::Module>, 6>& layers_modules = is_ilayer_barrel ? layers_modules_barrel : layers_modules_endcap;

        // Then get the module in the ilayer and check that it has a good module pair
        // Loop over modules in the given raw_ilayer and match the pairs and if a good pair of modules have hits in each module
        // then save the lower modules to layers_good_paired_modules.
        // NOTE there may be duplicates being pushed to layers_good_paired_modules
        // Do not count with these vectors
        for (unsigned imod = 0; imod < layers_modules[raw_ilayer].size(); ++imod)
        {
            for (unsigned jmod = imod + 1; jmod < layers_modules[raw_ilayer].size(); ++jmod)
            {
                // if two modules are a good pair
                if (layers_modules[raw_ilayer][imod].partnerDetId() == layers_modules[raw_ilayer][jmod].detId())
                {
                    // add the lower module one to the good_paired_modules list
                    if (layers_modules[raw_ilayer][imod].isLower())
                    {
                        if (std::find(
                                    layers_good_paired_modules[logical_ilayer].begin(),
                                    layers_good_paired_modules[logical_ilayer].end(),
                                    layers_modules[raw_ilayer][imod].detId()) == layers_good_paired_modules[logical_ilayer].end())
                            layers_good_paired_modules[logical_ilayer].push_back(layers_modules[raw_ilayer][imod].detId());
                    }
                    else
                    {
                        if (std::find(
                                    layers_good_paired_modules[logical_ilayer].begin(),
                                    layers_good_paired_modules[logical_ilayer].end(),
                                    layers_modules[raw_ilayer][imod].partnerDetId()) == layers_good_paired_modules[logical_ilayer].end())
                        layers_good_paired_modules[logical_ilayer].push_back(layers_modules[raw_ilayer][imod].partnerDetId());
                    }
                }
            }
        }
    }

    return checkModuleConnectionsAreGood(layers_good_paired_modules);

}



bool hasAll12HitsWithNBarrel(unsigned int isimtrk, int nbarrel)
{

    // Select only tracks that left all 12 hits in the barrel
    std::array<std::vector<SDL::Module>, 6> layers_modules_barrel;
    std::array<std::vector<SDL::Module>, 6> layers_modules_endcap;

    std::vector<float> ps;

    for (unsigned int ith_hit = 0; ith_hit < trk.sim_simHitIdx()[isimtrk].size(); ++ith_hit)
    {

        // Retrieve the sim hit idx
        unsigned int simhitidx = trk.sim_simHitIdx()[isimtrk][ith_hit];

        // Select only the hits in the outer tracker
        if (not (trk.simhit_subdet()[simhitidx] == 4 or trk.simhit_subdet()[simhitidx] == 5))
            continue;

        // if (not (trk.simhit_particle()[simhitidx] == trk.sim_pdgId()[isimtrk]))
        //     continue;

        if (isMuonCurlingHit(isimtrk, ith_hit))
            break;

        // list of reco hit matched to this sim hit
        for (unsigned int irecohit = 0; irecohit < trk.simhit_hitIdx()[simhitidx].size(); ++irecohit)
        {
            // Get the recohit type
            int recohittype = trk.simhit_hitType()[simhitidx][irecohit];

            // Consider only ph2 hits (i.e. outer tracker hits)
            if (recohittype == 4)
            {

                int ihit = trk.simhit_hitIdx()[simhitidx][irecohit];

                if (trk.ph2_subdet()[ihit] == 5)
                {
                    layers_modules_barrel[trk.ph2_layer()[ihit] - 1].push_back(SDL::Module(trk.ph2_detId()[ihit]));
                }
                if (trk.ph2_subdet()[ihit] == 4)
                {
                    layers_modules_endcap[trk.ph2_layer()[ihit] - 1].push_back(SDL::Module(trk.ph2_detId()[ihit]));
                }

            }

        }

    }

    std::array<bool, 6> has_good_pair_by_layer_barrel;
    has_good_pair_by_layer_barrel[0] = false;
    has_good_pair_by_layer_barrel[1] = false;
    has_good_pair_by_layer_barrel[2] = false;
    has_good_pair_by_layer_barrel[3] = false;
    has_good_pair_by_layer_barrel[4] = false;
    has_good_pair_by_layer_barrel[5] = false;

    std::array<bool, 6> has_good_pair_by_layer_endcap;
    has_good_pair_by_layer_endcap[0] = false;
    has_good_pair_by_layer_endcap[1] = false;
    has_good_pair_by_layer_endcap[2] = false;
    has_good_pair_by_layer_endcap[3] = false;
    has_good_pair_by_layer_endcap[4] = false;
    has_good_pair_by_layer_endcap[5] = false;

    bool has_good_pair_all_layer = true;

    for (int ilayer = 0; ilayer < 6; ++ilayer)
    {

        bool has_good_pair = false;

        if (ilayer < nbarrel)
        {

            for (unsigned imod = 0; imod < layers_modules_barrel[ilayer].size(); ++imod)
            {
                for (unsigned jmod = imod + 1; jmod < layers_modules_barrel[ilayer].size(); ++jmod)
                {
                    if (layers_modules_barrel[ilayer][imod].partnerDetId() == layers_modules_barrel[ilayer][jmod].detId())
                        has_good_pair = true;
                }
            }

        }
        else
        {

            int endcap_ilayer = ilayer - nbarrel;

            for (unsigned imod = 0; imod < layers_modules_endcap[endcap_ilayer].size(); ++imod)
            {
                for (unsigned jmod = imod + 1; jmod < layers_modules_endcap[endcap_ilayer].size(); ++jmod)
                {
                    if (layers_modules_endcap[endcap_ilayer][imod].partnerDetId() == layers_modules_endcap[endcap_ilayer][jmod].detId())
                        has_good_pair = true;
                }
            }

        }

        if (not has_good_pair)
            has_good_pair_all_layer = false;

        if (ilayer < nbarrel)
            has_good_pair_by_layer_barrel[ilayer] = has_good_pair;
        else
            has_good_pair_by_layer_endcap[ilayer] = has_good_pair;

    }


    float pt = trk.sim_pt()[isimtrk];
    float eta = trk.sim_eta()[isimtrk];

    // if (abs((trk.sim_pt()[isimtrk] - 0.71710)) < 0.00001)
    // {
    //     std::cout << std::endl;
    //     std::cout <<  " has_good_pair_by_layer[0]: " << has_good_pair_by_layer[0] <<  " has_good_pair_by_layer[1]: " << has_good_pair_by_layer[1] <<  " has_good_pair_by_layer[2]: " << has_good_pair_by_layer[2] <<  " has_good_pair_by_layer[3]: " << has_good_pair_by_layer[3] <<  " has_good_pair_by_layer[4]: " << has_good_pair_by_layer[4] <<  " has_good_pair_by_layer[5]: " << has_good_pair_by_layer[5] <<  " pt: " << pt <<  " eta: " << eta <<  std::endl;
    // }

    return has_good_pair_all_layer;

}


bool checkModuleConnectionsAreGood(std::array<std::vector<unsigned int>, 6>& layers_good_paired_modules)
{
    // Dumbest possible solution
    for (auto& module0 : layers_good_paired_modules[0])
    {
        const std::vector<unsigned int>& connectedModule1s = ana.moduleConnectiongMapLoose.getConnectedModuleDetIds(module0);
        for (auto& module1 : layers_good_paired_modules[1])
        {
            if (std::find(connectedModule1s.begin(), connectedModule1s.end(), module1) == connectedModule1s.end())
                break;
            const std::vector<unsigned int>& connectedModule2s = ana.moduleConnectiongMapLoose.getConnectedModuleDetIds(module1);
            for (auto& module2 : layers_good_paired_modules[2])
            {
                if (std::find(connectedModule2s.begin(), connectedModule2s.end(), module2) == connectedModule2s.end())
                    break;
                const std::vector<unsigned int>& connectedModule3s = ana.moduleConnectiongMapLoose.getConnectedModuleDetIds(module2);
                for (auto& module3 : layers_good_paired_modules[3])
                {
                    if (std::find(connectedModule3s.begin(), connectedModule3s.end(), module3) == connectedModule3s.end())
                        break;
                    const std::vector<unsigned int>& connectedModule4s = ana.moduleConnectiongMapLoose.getConnectedModuleDetIds(module3);
                    for (auto& module4 : layers_good_paired_modules[4])
                    {
                        if (std::find(connectedModule4s.begin(), connectedModule4s.end(), module4) == connectedModule4s.end())
                            break;
                        const std::vector<unsigned int>& connectedModule5s = ana.moduleConnectiongMapLoose.getConnectedModuleDetIds(module4);
                        for (auto& module5 : layers_good_paired_modules[5])
                        {
                            if (std::find(connectedModule5s.begin(), connectedModule5s.end(), module5) == connectedModule5s.end())
                                break;
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

void addOuterTrackerHits(SDL::Event& event)
{
    for (auto&& [ihit, data] : iter::enumerate(iter::zip(trk.ph2_x(), trk.ph2_y(), trk.ph2_z(), trk.ph2_subdet(), trk.ph2_detId())))
    {

        auto&& [x, y, z, subdet, detId] = data;

        if (not (subdet == 5 or subdet == 4))
            continue;

        // Takes two arguments, SDL::Hit, and detId
        // SDL::Event internally will structure whether we already have the module instance or we need to create a new one.
        //
        event.addHitToEvent(x,y,z,detId);

    }
}

void runMiniDoublet(SDL::Event& event)
{
    TStopwatch my_timer;
    if (ana.verbose != 0) std::cout << "Reco Mini-Doublet start" << std::endl;
    my_timer.Start();
    event.createMiniDoublets();
    float md_elapsed = my_timer.RealTime();
            
    if (ana.verbose != 0) std::cout << "Reco Mini-doublet processing time: " << md_elapsed << " secs" << std::endl;

    if (ana.verbose != 0) std::cout << "# of Mini-doublets produced: " << event.getNumberOfMiniDoublets() << std::endl;
    if (ana.verbose != 0) std::cout << "# of Mini-doublets produced barrel layer 1: " << event.getNumberOfMiniDoubletsByLayerBarrel(0) << std::endl;
    if (ana.verbose != 0) std::cout << "# of Mini-doublets produced barrel layer 2: " << event.getNumberOfMiniDoubletsByLayerBarrel(1) << std::endl;
    if (ana.verbose != 0) std::cout << "# of Mini-doublets produced barrel layer 3: " << event.getNumberOfMiniDoubletsByLayerBarrel(2) << std::endl;
    if (ana.verbose != 0) std::cout << "# of Mini-doublets produced barrel layer 4: " << event.getNumberOfMiniDoubletsByLayerBarrel(3) << std::endl;
    if (ana.verbose != 0) std::cout << "# of Mini-doublets produced barrel layer 5: " << event.getNumberOfMiniDoubletsByLayerBarrel(4) << std::endl;
    if (ana.verbose != 0) std::cout << "# of Mini-doublets produced barrel layer 6: " << event.getNumberOfMiniDoubletsByLayerBarrel(5) << std::endl;

}

void runSegment(SDL::Event& event)
{
    TStopwatch my_timer;
    if (ana.verbose != 0) std::cout << "Reco Segment start" << std::endl;
    my_timer.Start(kFALSE);
    event.createSegmentsWithModuleMap();
    float sg_elapsed = my_timer.RealTime();
    if (ana.verbose != 0) std::cout << "Reco Segment processing time: " << sg_elapsed << " secs" << std::endl;

    if (ana.verbose != 0) std::cout << "# of Segments produced: " << event.getNumberOfSegments() << std::endl;
    if (ana.verbose != 0) std::cout << "# of Segments produced layer 1-2: " << event.getNumberOfSegmentsByLayerBarrel(0) << std::endl;
    if (ana.verbose != 0) std::cout << "# of Segments produced layer 2-3: " << event.getNumberOfSegmentsByLayerBarrel(1) << std::endl;
    if (ana.verbose != 0) std::cout << "# of Segments produced layer 3-4: " << event.getNumberOfSegmentsByLayerBarrel(2) << std::endl;
    if (ana.verbose != 0) std::cout << "# of Segments produced layer 4-5: " << event.getNumberOfSegmentsByLayerBarrel(3) << std::endl;
    if (ana.verbose != 0) std::cout << "# of Segments produced layer 5-6: " << event.getNumberOfSegmentsByLayerBarrel(4) << std::endl;

}

void runTracklet(SDL::Event& event)
{
    TStopwatch my_timer;
    if (ana.verbose != 0) std::cout << "Reco Tracklet start" << std::endl;
    my_timer.Start(kFALSE);
    event.createTrackletsWithModuleMap();
//    event.createTrackletsWithAGapWithModuleMap();
    event.createPixelTracklets();
    float tl_elapsed = my_timer.RealTime();
    if (ana.verbose != 0) std::cout << "Reco Tracklet processing time: " << tl_elapsed << " secs" << std::endl;
    if (ana.verbose != 0) std::cout << "# of Tracklets produced: " << event.getNumberOfTracklets() << std::endl;
    if (ana.verbose != 0) std::cout << "# of Tracklets produced layer 1-2-3-4: " << event.getNumberOfTrackletsByLayerBarrel(0) << std::endl;
    if (ana.verbose != 0) std::cout << "# of Tracklets produced layer 2-3-4-5: " << event.getNumberOfTrackletsByLayerBarrel(1) << std::endl;
    if (ana.verbose != 0) std::cout << "# of Tracklets produced layer 3-4-5-6: " << event.getNumberOfTrackletsByLayerBarrel(2) << std::endl;
    if (ana.verbose != 0) std::cout << "# of Tracklets produced layer 4: " << event.getNumberOfTrackletsByLayerBarrel(3) << std::endl;
    if (ana.verbose != 0) std::cout << "# of Tracklets produced layer 5: " << event.getNumberOfTrackletsByLayerBarrel(4) << std::endl;
    if (ana.verbose != 0) std::cout << "# of Tracklets produced layer 6: " << event.getNumberOfTrackletsByLayerBarrel(5) << std::endl;

}

void runTriplet(SDL::Event& event)
{
    TStopwatch my_timer;
    if (ana.verbose != 0) std::cout << "Reco Triplet start" << std::endl;
    my_timer.Start(kFALSE);
    event.createTriplets();
    float tp_elapsed = my_timer.RealTime();
    if (ana.verbose != 0) std::cout << "Reco Triplet processing time: " << tp_elapsed<< " secs" << std::endl;
    if (ana.verbose != 0) std::cout << "# of Triplets produced: " << event.getNumberOfTriplets() << std::endl;
    if (ana.verbose != 0) std::cout << "# of Triplets produced layer 1-2-3: " << event.getNumberOfTripletsByLayerBarrel(0) << std::endl;
    if (ana.verbose != 0) std::cout << "# of Triplets produced layer 2-3-4: " << event.getNumberOfTripletsByLayerBarrel(1) << std::endl;
    if (ana.verbose != 0) std::cout << "# of Triplets produced layer 3-4-5: " << event.getNumberOfTripletsByLayerBarrel(2) << std::endl;
    if (ana.verbose != 0) std::cout << "# of Triplets produced layer 4-5-6: " << event.getNumberOfTripletsByLayerBarrel(3) << std::endl;
    if (ana.verbose != 0) std::cout << "# of Triplets produced endcap layer 1-2-3: " << event.getNumberOfTripletsByLayerEndcap(0) << std::endl;
    if (ana.verbose != 0) std::cout << "# of Triplets produced endcap layer 2-3-4: " << event.getNumberOfTripletsByLayerEndcap(1) << std::endl;
    if (ana.verbose != 0) std::cout << "# of Triplets produced endcap layer 3-4-5: " << event.getNumberOfTripletsByLayerEndcap(2) << std::endl;

}

void runTrackCandidateTest_v2(SDL::Event& event)
{
    TStopwatch my_timer;
    if (ana.verbose != 0) std::cout << "Reco TrackCandidate start" << std::endl;
    my_timer.Start(kFALSE);
    // event.createTrackCandidatesFromTriplets();
    // event.createTrackCandidates();

    event.createTrackCandidates();
    float tc_elapsed = my_timer.RealTime();
    if (ana.verbose != 0) std::cout << "Reco TrackCandidate processing time: " << tc_elapsed << " secs" << std::endl;
    if (ana.verbose != 0) std::cout << "# of TrackCandidates produced: " << event.getNumberOfTrackCandidates() << std::endl;
    if (ana.verbose != 0) std::cout << "# of TrackCandidates produced layer 1-2-3-4-5-6: " << event.getNumberOfTrackCandidatesByLayerBarrel(0) << std::endl;
    if (ana.verbose != 0) std::cout << "# of TrackCandidates produced layer 2: " << event.getNumberOfTrackCandidatesByLayerBarrel(1) << std::endl;
    if (ana.verbose != 0) std::cout << "# of TrackCandidates produced layer 3: " << event.getNumberOfTrackCandidatesByLayerBarrel(2) << std::endl;
    if (ana.verbose != 0) std::cout << "# of TrackCandidates produced layer 4: " << event.getNumberOfTrackCandidatesByLayerBarrel(3) << std::endl;
    if (ana.verbose != 0) std::cout << "# of TrackCandidates produced layer 5: " << event.getNumberOfTrackCandidatesByLayerBarrel(4) << std::endl;
    if (ana.verbose != 0) std::cout << "# of TrackCandidates produced layer 6: " << event.getNumberOfTrackCandidatesByLayerBarrel(5) << std::endl;

}


bool goodEvent()
{
    if (ana.specific_event_index >= 0)
    {
        if ((int)ana.looper.getCurrentEventIndex() != ana.specific_event_index)
            return false;
    }

    // If splitting jobs are requested then determine whether to process the event or not based on remainder
    if (ana.nsplit_jobs >= 0 and ana.job_index >= 0)
    {
        if (ana.looper.getNEventsProcessed() % ana.nsplit_jobs != (unsigned int) ana.job_index)
            return false;
    }

    if (ana.verbose) std::cout <<  " ana.looper.getCurrentEventIndex(): " << ana.looper.getCurrentEventIndex() <<  std::endl;

    return true;
}

std::vector<float> getPtBounds()
{
    std::vector<float> pt_boundaries;
    if (ana.ptbound_mode == 0)
        pt_boundaries = {0, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.5, 2.0, 3.0, 5.0, 10, 15., 25, 50};
    else if (ana.ptbound_mode == 1)
        pt_boundaries = {0.988, 0.99, 0.992, 0.994, 0.996, 0.998, 1.0, 1.002, 1.004, 1.006, 1.008, 1.01, 1.012}; // lowpt
    else if (ana.ptbound_mode == 2)
        pt_boundaries = {0.955, 0.96, 0.965, 0.97, 0.975, 0.98, 0.985, 0.99, 0.995, 1.00, 1.005, 1.01, 1.015, 1.02, 1.025, 1.03, 1.035, 1.04, 1.045, 1.05}; // pt 0p95 1p05
    else if (ana.ptbound_mode == 3)
        pt_boundaries = {0.5, 0.6, 0.7, 0.8, 0.9, 0.92, 0.94, 0.96, 0.98, 1.0, 1.02, 1.04, 1.06, 1.08, 1.1, 1.2, 1.5}; // lowpt
    else if (ana.ptbound_mode == 4)
        pt_boundaries = {0.5, 0.52, 0.54, 0.56, 0.58, 0.6, 0.62, 0.64, 0.66, 0.68, 0.7, 0.72, 0.74, 0.76, 0.78, 0.8, 0.82, 0.84, 0.86, 0.88, 0.9, 0.92, 0.94, 0.96, 0.98, 1.0, 1.02, 1.04, 1.06, 1.08, 1.1, 1.12, 1.14, 1.16, 1.18, 1.2, 1.22, 1.24, 1.26, 1.28, 1.3, 1.32, 1.34, 1.36, 1.38, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0}; // lowpt
    else if (ana.ptbound_mode == 5)
        pt_boundaries = {0.5, 0.52, 0.54, 0.56, 0.58, 0.6, 0.62, 0.64, 0.66, 0.68, 0.7, 0.72, 0.74, 0.76, 0.78, 0.8, 0.82, 0.84, 0.86, 0.88, 0.9, 0.92, 0.94, 0.96, 0.98, 1.0, 1.02, 1.04, 1.06, 1.08, 1.1, 1.12, 1.14, 1.16, 1.18, 1.2, 1.24, 1.28, 1.32, 1.36, 1.4, 1.6, 1.8, 2.0, 2.2, 2.4, 2.6, 2.8, 3.0}; // lowpt
    else if (ana.ptbound_mode == 6)
        pt_boundaries = {0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 2.0, 3.0, 4.0, 5.0}; // lowpt
    else if (ana.ptbound_mode == 7)
        pt_boundaries = {0.5, 0.52, 0.54, 0.56, 0.58, 0.6, 0.62, 0.64, 0.66, 0.68, 0.7, 0.72, 0.74, 0.76, 0.78, 0.8, 0.82, 0.84, 0.86, 0.88, 0.9, 0.92, 0.94, 0.96, 0.98, 1.0, 1.02, 1.04, 1.06, 1.08, 1.1, 1.12, 1.14, 1.16, 1.18, 1.2, 1.22, 1.24, 1.26, 1.28, 1.3, 1.32, 1.34, 1.36, 1.38, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0, 2.2, 2.4, 2.6, 2.8, 3, 4, 5, 6, 7, 8, 9, 10, 15, 20, 25, 30, 35, 40, 45, 50}; // lowpt
    else if (ana.ptbound_mode == 8)
        pt_boundaries = {0, 0.5, 1.0, 3.0, 5.0, 10, 15., 25, 50};
    return pt_boundaries;
}

bool inTimeTrackWithPdgId(int isimtrk, int pdgid)
{
    // Then select all charged particle
    if (pdgid == 0)
    {
        // Select all charged particle tracks
        if (abs(trk.sim_q()[isimtrk]) == 0)
            return false;
    }
    else
    {
        // Select tracks with given pdgid
        if (abs(trk.sim_pdgId()[isimtrk]) != pdgid)
            return false;
    }

    // Select in time only
    if (abs(trk.sim_bunchCrossing()[isimtrk]) != 0)
        return false;

    return true;
}

std::vector<int> matchedSimTrkIdxs(std::vector<int> hitidxs, std::vector<int> hittypes)
{
    if (hitidxs.size() != hittypes.size())
    {
        std::cout << "Error: matched_sim_trk_idxs()   hitidxs and hittypes have different lengths" << std::endl;
        std::cout << "hitidxs.size(): " << hitidxs.size() << std::endl;
        std::cout << "hittypes.size(): " << hittypes.size() << std::endl;
    }

    std::vector<std::pair<int, int>> to_check_duplicate;
    for (auto&& [ihit, ihitdata] : iter::enumerate(iter::zip(hitidxs, hittypes)))
    {
        auto&& [hitidx, hittype] = ihitdata;
        auto item = std::make_pair(hitidx, hittype);
        if (std::find(to_check_duplicate.begin(), to_check_duplicate.end(), item) == to_check_duplicate.end())
        {
            to_check_duplicate.push_back(item);
        }
    }

    int nhits_input = to_check_duplicate.size();

    std::vector<vector<int>> simtrk_idxs;
    std::vector<int> unique_idxs; // to aggregate which ones to count and test

    for (auto&& [ihit, ihitdata] : iter::enumerate(to_check_duplicate))
    {
        auto&& [hitidx, hittype] = ihitdata;

        std::vector<int> simtrk_idxs_per_hit;

        const std::vector<vector<int>>* simHitIdxs;

        if (hittype == 4)
            simHitIdxs = &trk.ph2_simHitIdx();
        else
            simHitIdxs = &trk.pix_simHitIdx();

        if ( (*simHitIdxs).size() <= hitidx)
        {
                std::cout << (*simHitIdxs).size() << " " << hittype << std::endl;
                std::cout << hitidx << " " << hittype << std::endl;
        }

        for (auto& simhit_idx : (*simHitIdxs).at(hitidx))
        {
            // std::cout << "  " << trk.simhit_simTrkIdx().size() << std::endl;
            // std::cout << " " << simhit_idx << std::endl;
            if (trk.simhit_simTrkIdx().size() <= simhit_idx)
            {
                std::cout << (*simHitIdxs).size() << " " << hittype << std::endl;
                std::cout << hitidx << " " << hittype << std::endl;
                std::cout << trk.simhit_simTrkIdx().size() << " " << simhit_idx << std::endl;
            }
            int simtrk_idx = trk.simhit_simTrkIdx().at(simhit_idx);
            simtrk_idxs_per_hit.push_back(simtrk_idx);
            if (std::find(unique_idxs.begin(), unique_idxs.end(), simtrk_idx) == unique_idxs.end())
                unique_idxs.push_back(simtrk_idx);
        }

        if (simtrk_idxs_per_hit.size() == 0)
        {
            simtrk_idxs_per_hit.push_back(-1);
            if (std::find(unique_idxs.begin(), unique_idxs.end(), -1) == unique_idxs.end())
                unique_idxs.push_back(-1);
        }

        simtrk_idxs.push_back(simtrk_idxs_per_hit);
    }

    // // print
    // if (ana.verbose != 0)
    // {
    //     std::cout << "va print" << std::endl;
    //     for (auto& vec : simtrk_idxs)
    //     {
    //         for (auto& idx : vec)
    //         {
    //             std::cout << idx << " ";
    //         }
    //         std::cout << std::endl;
    //     }
    //     std::cout << "va print end" << std::endl;
    // }

    // Compute all permutations
    std::function<void(vector<vector<int>>&, vector<int>, size_t, vector<vector<int>>&)> perm =
        [&](vector<vector<int>>& result, vector<int> intermediate, size_t n, vector<vector<int>>& va)
    {
        if (va.size() > n)
        {
            for (auto x : va[n])
            {
                intermediate.push_back(x);
                perm(result, intermediate, n+1, va);
            }
        }
        else
        {
            result.push_back(intermediate);
        }
    };

    vector<vector<int>> allperms;
    perm(allperms, vector<int>(), 0, simtrk_idxs);

    std::vector<int> matched_sim_trk_idxs;
    for (auto& trkidx_perm : allperms)
    {
        std::vector<int> counts;
        for (auto& unique_idx : unique_idxs)
        {
            int cnt = std::count(trkidx_perm.begin(), trkidx_perm.end(), unique_idx);
            counts.push_back(cnt);
        }
        auto result = std::max_element(counts.begin(), counts.end());
        int rawidx = std::distance(counts.begin(), result);
        int trkidx = unique_idxs[rawidx];
        if (trkidx < 0)
            continue;
        if (counts[rawidx] > (((float)nhits_input) * 0.75))
            matched_sim_trk_idxs.push_back(trkidx);
    }

    return matched_sim_trk_idxs;
}

bool isMTVMatch(unsigned int isimtrk, std::vector<unsigned int> hit_idxs, bool verbose)
{
    std::vector<unsigned int> sim_trk_ihits;
    for (auto& i_simhit_idx : trk.sim_simHitIdx()[isimtrk])
    {
        for (auto& ihit : trk.simhit_hitIdx()[i_simhit_idx])
        {
            sim_trk_ihits.push_back(ihit);
        }
    }

    std::sort(sim_trk_ihits.begin(), sim_trk_ihits.end());
    std::sort(hit_idxs.begin(), hit_idxs.end());

    std::vector<unsigned int> v_intersection;

    std::set_intersection(sim_trk_ihits.begin(), sim_trk_ihits.end(),
                          hit_idxs.begin(), hit_idxs.end(),
                          std::back_inserter(v_intersection));

    if (verbose)
    {
        if (v_intersection.size() > ana.nmatch_threshold)
        {
            std::cout << "Matched" << std::endl;
        }
        else
        {
            std::cout << "Not matched" << std::endl;
        }
        std::cout << "sim_trk_ihits: ";
        for (auto& i_simhit_idx : sim_trk_ihits)
            std::cout << i_simhit_idx << " ";
        std::cout << std::endl;

        std::cout << "     hit_idxs: ";
        for (auto& i_hit_idx : hit_idxs)
            std::cout << i_hit_idx << " ";
        std::cout << std::endl;
    }

    int nhits = hit_idxs.size();

    float factor = nhits / 12.;

    // If 75% of 12 hits have been found than it is matched
    return (v_intersection.size() > ana.nmatch_threshold * factor);
}

void loadMaps()
{
    SDL::endcapGeometry.load("/home/users/phchang/public_html/analysis/sdl/TrackLooper_/scripts/endcap_orientation_data_v2.txt"); // centroid values added to the map
    SDL::tiltedGeometry.load("/home/users/phchang/public_html/analysis/sdl/TrackLooper_/scripts/tilted_orientation_data.txt");

    SDL::moduleConnectionMap.load("data/module_connection_combined_2020_0520_helixray.txt");
    ana.moduleConnectiongMapLoose.load("data/module_connection_combined_2020_0520_helixray.txt");

    // SDL::moduleConnectionMap.load("/home/users/phchang/public_html/analysis/sdl/TrackLooper_/scripts/module_connection_map_data_10_e0_200_100_pt0p8_2p0_400_pt0p8_2p0_nolossers_dxy35cm_endcaplayer2.txt");
    // ana.moduleConnectiongMapLoose.load("/home/users/phchang/public_html/analysis/sdl/TrackLooper_/scripts/module_connection_map_data_10_e0_200_100_pt0p8_2p0_400_pt0p8_2p0_nolossers_dxy35cm_endcaplayer2.txt");

    // SDL::moduleConnectionMap.load("data/module_connection_2020_0429.txt");
    // ana.moduleConnectiongMapLoose.load("data/module_connection_2020_0429.txt");

    // SDL::moduleConnectionMap.load("data/module_connection_tracing_2020_0514.txt");
    // SDL::moduleConnectionMap.load("data/module_connection_combined_2020_0518_helixray.txt");

}

std::vector<int> matchedSimTrkIdxs(SDL::Tracklet& tl)
{
    std::vector<int> hitidxs = {
        tl.innerSegmentPtr()->innerMiniDoubletPtr()->lowerHitPtr()->idx(),
        tl.innerSegmentPtr()->innerMiniDoubletPtr()->upperHitPtr()->idx(),
        tl.innerSegmentPtr()->outerMiniDoubletPtr()->lowerHitPtr()->idx(),
        tl.innerSegmentPtr()->outerMiniDoubletPtr()->upperHitPtr()->idx(),
        tl.outerSegmentPtr()->innerMiniDoubletPtr()->lowerHitPtr()->idx(),
        tl.outerSegmentPtr()->innerMiniDoubletPtr()->upperHitPtr()->idx(),
        tl.outerSegmentPtr()->outerMiniDoubletPtr()->lowerHitPtr()->idx(),
        tl.outerSegmentPtr()->outerMiniDoubletPtr()->upperHitPtr()->idx()
        };

    std::vector<vector<int>> simtrk_idxs;
    std::vector<int> unique_idxs; // to aggregate which ones to count and test

    for (auto& hitidx : hitidxs)
    {
        std::vector<int> simtrk_idxs_per_hit;
        for (auto& simhit_idx : trk.ph2_simHitIdx()[hitidx])
        {
            int simtrk_idx = trk.simhit_simTrkIdx()[simhit_idx];
            simtrk_idxs_per_hit.push_back(simtrk_idx);
            if (std::find(unique_idxs.begin(), unique_idxs.end(), simtrk_idx) == unique_idxs.end())
                unique_idxs.push_back(simtrk_idx);
        }
        if (simtrk_idxs_per_hit.size() == 0)
        {
            simtrk_idxs_per_hit.push_back(-1);
            if (std::find(unique_idxs.begin(), unique_idxs.end(), -1) == unique_idxs.end())
                unique_idxs.push_back(-1);
        }
        simtrk_idxs.push_back(simtrk_idxs_per_hit);
    }

    // // print
    // std::cout << "va print" << std::endl;
    // for (auto& vec : simtrk_idxs)
    // {
    //     for (auto& idx : vec)
    //     {
    //         std::cout << idx << " ";
    //     }
    //     std::cout << std::endl;
    // }
    // std::cout << "va print end" << std::endl;

    // Compute all permutations
    std::function<void(vector<vector<int>>&, vector<int>, size_t, vector<vector<int>>&)> perm = [&](vector<vector<int>>& result, vector<int> intermediate, size_t n, vector<vector<int>>& va)
    {
        if (va.size() > n)
        {
            for (auto x : va[n])
            {
                intermediate.push_back(x);
                perm(result, intermediate, n+1, va);
            }
        }
        else
        {
            result.push_back(intermediate);
        }
    };

    vector<vector<int>> allperms;
    perm(allperms, vector<int>(), 0, simtrk_idxs);

    // for (auto& iperm : allperms)
    // {
    //     for (auto& idx : iperm)
    //         std::cout << idx << " ";
    //     std::cout << std::endl;
    // }

    std::vector<int> matched_sim_trk_idxs;
    for (auto& trkidx_perm : allperms)
    {
        std::vector<int> counts;
        for (auto& unique_idx : unique_idxs)
        {
            int cnt = std::count(trkidx_perm.begin(), trkidx_perm.end(), unique_idx);
            counts.push_back(cnt);
        }
        auto result = std::max_element(counts.begin(), counts.end());
        int rawidx = std::distance(counts.begin(), result);
        int trkidx = unique_idxs[rawidx];
        if (trkidx < 0)
            continue;
        if (counts[rawidx] > 6)
            matched_sim_trk_idxs.push_back(trkidx);
    }

    return matched_sim_trk_idxs;

}

vector<int> matchedSimTrkIdxs(SDL::Segment* sg, bool matchOnlyAnchor)
{
    std::vector<int> hitidxs_all = {
        sg->innerMiniDoubletPtr()->lowerHitPtr()->idx(),
        sg->innerMiniDoubletPtr()->upperHitPtr()->idx(),
        sg->outerMiniDoubletPtr()->lowerHitPtr()->idx(),
        sg->outerMiniDoubletPtr()->upperHitPtr()->idx()
    };

    std::vector<int> hitidxs_onlyAnchor = {
        sg->innerMiniDoubletPtr()->anchorHitPtr()->idx(),
        sg->outerMiniDoubletPtr()->anchorHitPtr()->idx()
    };

    std::vector<int> hitidxs;
    if (matchOnlyAnchor)
    {
        hitidxs = hitidxs_onlyAnchor;
    }
    else
    {
        hitidxs = hitidxs_all;
    }

    int threshold = matchOnlyAnchor ? 1 : 3;

    std::vector<vector<int>> simtrk_idxs;
    std::vector<int> unique_idxs; // to aggregate which ones to count and test

    for (auto& hitidx : hitidxs)
    {
        std::vector<int> simtrk_idxs_per_hit;
        for (auto& simhit_idx : trk.ph2_simHitIdx()[hitidx])
        {
            int simtrk_idx = trk.simhit_simTrkIdx()[simhit_idx];
            simtrk_idxs_per_hit.push_back(simtrk_idx);
            if (std::find(unique_idxs.begin(), unique_idxs.end(), simtrk_idx) == unique_idxs.end())
                unique_idxs.push_back(simtrk_idx);
        }
        if (simtrk_idxs_per_hit.size() == 0)
        {
            simtrk_idxs_per_hit.push_back(-1);
            if (std::find(unique_idxs.begin(), unique_idxs.end(), -1) == unique_idxs.end())
                unique_idxs.push_back(-1);
        }
        simtrk_idxs.push_back(simtrk_idxs_per_hit);
    }

    // Compute all permutations
    std::function<void(vector<vector<int>>&, vector<int>, size_t, vector<vector<int>>&)> perm = [&](vector<vector<int>>& result, vector<int> intermediate, size_t n, vector<vector<int>>& va)
    {
        if (va.size() > n)
        {
            for (auto x : va[n])
            {
                intermediate.push_back(x);
                perm(result, intermediate, n+1, va);
            }
        }
        else
        {
            result.push_back(intermediate);
        }
    };

    vector<vector<int>> allperms;
    perm(allperms, vector<int>(), 0, simtrk_idxs);

    // for (auto& iperm : allperms)
    // {
    //     for (auto& idx : iperm)
    //         std::cout << idx << " ";
    //     std::cout << std::endl;
    // }

    std::vector<int> matched_sim_trk_idxs;
    for (auto& trkidx_perm : allperms)
    {
        std::vector<int> counts;
        for (auto& unique_idx : unique_idxs)
        {
            int cnt = std::count(trkidx_perm.begin(), trkidx_perm.end(), unique_idx);
            counts.push_back(cnt);
        }
        auto result = std::max_element(counts.begin(), counts.end());
        int rawidx = std::distance(counts.begin(), result);
        int trkidx = unique_idxs[rawidx];
        if (trkidx < 0)
            continue;
        if (counts[rawidx] > threshold)
            matched_sim_trk_idxs.push_back(trkidx);
    }

    return matched_sim_trk_idxs;

}

float drfracSimHitConsistentWithHelix(int isimtrk, int isimhitidx)
{
    // Read track parameters
    float vx = trk.simvtx_x()[0];
    float vy = trk.simvtx_y()[0];
    float vz = trk.simvtx_z()[0];
    float pt = trk.sim_pt()[isimtrk];
    float eta = trk.sim_eta()[isimtrk];
    float phi = trk.sim_phi()[isimtrk];
    float charge = trk.sim_q()[isimtrk];

    // Construct helix object
    SDLMath::Helix helix(pt, eta, phi, vx, vy, vz, charge);

    return drfracSimHitConsistentWithHelix(helix, isimhitidx);

}

//__________________________________________________________________________________________
float drfracSimHitConsistentWithHelix(SDLMath::Helix& helix, int isimhitidx)
{

    // Sim hit vector
    std::vector<float> point = {trk.simhit_x()[isimhitidx], trk.simhit_y()[isimhitidx], trk.simhit_z()[isimhitidx]};

    // Inferring parameter t of helix parametric function via z position
    float t = helix.infer_t(point);

    // If the best fit is more than pi parameter away then it's a returning hit and should be ignored
    if (not (t <= M_PI))
        return 999;

    // Expected hit position with given z
    auto [x, y, z, r] = helix.get_helix_point(t);

    // ( expected_r - simhit_r ) / expected_r
    float drfrac = abs(helix.compare_radius(point)) / r;

    return drfrac;

}

//__________________________________________________________________________________________
float distxySimHitConsistentWithHelix(int isimtrk, int isimhitidx)
{
    // Read track parameters
    float vx = trk.simvtx_x()[0];
    float vy = trk.simvtx_y()[0];
    float vz = trk.simvtx_z()[0];
    float pt = trk.sim_pt()[isimtrk];
    float eta = trk.sim_eta()[isimtrk];
    float phi = trk.sim_phi()[isimtrk];
    float charge = trk.sim_q()[isimtrk];

    // Construct helix object
    SDLMath::Helix helix(pt, eta, phi, vx, vy, vz, charge);

    return distxySimHitConsistentWithHelix(helix, isimhitidx);

}

//__________________________________________________________________________________________
float distxySimHitConsistentWithHelix(SDLMath::Helix& helix, int isimhitidx)
{

    // Sim hit vector
    std::vector<float> point = {trk.simhit_x()[isimhitidx], trk.simhit_y()[isimhitidx], trk.simhit_z()[isimhitidx]};

    // Inferring parameter t of helix parametric function via z position
    float t = helix.infer_t(point);

    // If the best fit is more than pi parameter away then it's a returning hit and should be ignored
    if (not (t <= M_PI))
        return 999;

    // Expected hit position with given z
    auto [x, y, z, r] = helix.get_helix_point(t);

    // ( expected_r - simhit_r ) / expected_r
    float distxy = helix.compare_xy(point);

    return distxy;

}

void addPixelSegments(SDL::Event& event, int isimtrk)
{
    for (auto&& [iSeed, _] : iter::enumerate(trk.see_stateTrajGlbPx()))
    {

        if (isimtrk >= 0)
        {
            bool match = false;
            for (auto& seed_simtrkidx : trk.see_simTrkIdx()[iSeed])
            {
                if (seed_simtrkidx == isimtrk)
                {
                    match = true;
                }
            }
            if (not match)
                continue;
        }

        TVector3 p3LH(trk.see_stateTrajGlbPx()[iSeed], trk.see_stateTrajGlbPy()[iSeed], trk.see_stateTrajGlbPz()[iSeed]);
        TVector3 r3LH(trk.see_stateTrajGlbX()[iSeed], trk.see_stateTrajGlbY()[iSeed], trk.see_stateTrajGlbZ()[iSeed]);
        TVector3 p3PCA(trk.see_px()[iSeed], trk.see_py()[iSeed], trk.see_pz()[iSeed]);
        TVector3 r3PCA(r3FromPCA(p3PCA, trk.see_dxy()[iSeed], trk.see_dz()[iSeed]));
        auto const& seedHitsV = trk.see_hitIdx()[iSeed];
        auto const& seedHitTypesV = trk.see_hitType()[iSeed];

        bool good_seed_type = false;
        if (trk.see_algo()[iSeed] == 4) good_seed_type = true;
        if (trk.see_algo()[iSeed] == 5) good_seed_type = true;
        if (trk.see_algo()[iSeed] == 7) good_seed_type = true;
        if (trk.see_algo()[iSeed] == 22) good_seed_type = true;
        if (trk.see_algo()[iSeed] == 23) good_seed_type = true;
        if (trk.see_algo()[iSeed] == 24) good_seed_type = true;
        if (not good_seed_type)
            continue;

        int nHits = seedHitsV.size();

        int seedSD_mdRef_pixL = trk.see_hitIdx()[iSeed][0];
        int seedSD_mdRef_pixU = trk.see_hitIdx()[iSeed][1];
        TVector3 seedSD_mdRef_r3 = r3PCA;
        float seedSD_mdRef_rt = r3PCA.Pt();
        float seedSD_mdRef_z = r3PCA.Z();
        float seedSD_mdRef_r = r3PCA.Mag();
        float seedSD_mdRef_phi = r3PCA.Phi();
        float seedSD_mdRef_alpha = r3PCA.DeltaPhi(p3PCA);

        int seedSD_mdOut_pixL = trk.see_hitIdx()[iSeed][2];
        int seedSD_mdOut_pixU = trk.see_hitIdx()[iSeed][3];
        TVector3 seedSD_mdOut_r3 = r3LH;
        float seedSD_mdOut_rt = r3LH.Pt();
        float seedSD_mdOut_z = r3LH.Z();
        float seedSD_mdOut_r = r3LH.Mag();
        float seedSD_mdOut_phi = r3LH.Phi();
        float seedSD_mdOut_alpha = r3LH.DeltaPhi(p3LH);

        float seedSD_iRef = iSeed;
        float seedSD_iOut = iSeed;
        TVector3 seedSD_r3 = r3LH;
        float seedSD_rt = r3LH.Pt();
        float seedSD_rtInv = 1.f / seedSD_rt;
        float seedSD_z = seedSD_r3.Z();
        TVector3 seedSD_p3 = p3LH;
        float seedSD_alpha = r3LH.DeltaPhi(p3LH);
        float seedSD_dr = (r3LH - r3PCA).Pt();
        float seedSD_d = seedSD_rt - r3PCA.Pt();
        float seedSD_zeta = seedSD_p3.Pt() / seedSD_p3.Z();
        struct SDL::hits* hitsInGPU = event.getHits();
        // Inner most hit
        //FIXME:There is no SDL::Hit now!
        int hitIdx0InNtuple = trk.see_hitIdx()[iSeed][0];
        int hittype0 = trk.see_hitType()[iSeed][0];
        event.addHitToEvent(r3PCA.X(),r3PCA.Y(), r3PCA.Z(),1, hitIdx0InNtuple);
        unsigned int hitIdx0 = *(hitsInGPU->nHits) - 1; //last hit index

//        hits.push_back(SDL::Hit(r3PCA.X(), r3PCA.Y(), r3PCA.Z(), hitidx0));
//
        int hitIdx1InNtuple = trk.see_hitIdx()[iSeed][1];
        int hittype1 = trk.see_hitType()[iSeed][1];
//        hits.push_back(SDL::Hit(r3PCA.X(), r3PCA.Y(), r3PCA.Z(), hitidx1));
        event.addHitToEvent(r3PCA.X(), r3PCA.Y(), r3PCA.Z(), 1, hitIdx1InNtuple);
        unsigned int hitIdx1 = *(hitsInGPU->nHits) - 1;

        int hitIdx2InNtuple = trk.see_hitIdx()[iSeed][2];
//        int hittype2 = trk.see_hitType()[iSeed][2];

//        hits.push_back(SDL::Hit(r3LH.X(), r3LH.Y(), r3LH.Z(), hitidx2));

        event.addHitToEvent(r3LH.X(), r3LH.Y(), r3LH.Z(),1,hitIdx2InNtuple);
        unsigned int hitIdx2 = *(hitsInGPU->nHits) - 1;

        int hitIdx3InNtuple = trk.see_hitIdx()[iSeed].size() > 3 ? trk.see_hitIdx()[iSeed][3] : trk.see_hitIdx()[iSeed][2]; // repeat last one if triplet
        //int hittype3 = trk.see_hitIdx()[iSeed].size() > 3 ? trk.see_hitType()[iSeed][3] : trk.see_hitIdx()[iSeed][2]; // repeat last one if triplet
        // hits.push_back(SDL::Hit(trk.pix_x()[hitidx3], trk.pix_y()[hitidx3], trk.pix_z()[hitidx3], hitidx3));
        unsigned int hitIdx3;
        if(trk.see_hitIdx[iSeed].size() <= 3)
        {   
            hitIdx3 = hitIdx2;
        }
        else
        {
            event.addHitToEvent(r3LH.X(), r3LH.Y(), r3LH.Z(),1,hitIdx3InNtuple);
            hitIdx3 = *(hitsInGPU->nHits) - 1;
        }

>>>>>>> c6d3d5074216a50087071959ebf4ea2e55150a6b
        float pixelSegmentDeltaPhiChange = r3LH.DeltaPhi(p3LH);
        float ptIn = p3LH.Pt();
        float ptErr = trk.see_ptErr()[iSeed];
        float etaErr = trk.see_etaErr()[iSeed];
        float px = p3LH.X();
        float py = p3LH.Y();
        float pz = p3LH.Z();

        if ((ptIn > 0.7) and (fabs(p3LH.Eta()) < 3))
        {
            int hittype0 = trk.see_hitType()[iSeed][0];
            event.addHitToEvent(r3PCA.X(),r3PCA.Y(), r3PCA.Z(),1);
            unsigned int hitIdx0 = *(hitsInGPU->nHits) - 1; //last hit index

            int hittype1 = trk.see_hitType()[iSeed][1];
            event.addHitToEvent(r3PCA.X(), r3PCA.Y(), r3PCA.Z(), 1);
            unsigned int hitIdx1 = *(hitsInGPU->nHits) - 1;

            event.addHitToEvent(r3LH.X(), r3LH.Y(), r3LH.Z(),1);
            unsigned int hitIdx2 = *(hitsInGPU->nHits) - 1;

            unsigned int hitIdx3;
            if(trk.see_hitIdx()[iSeed].size() <= 3)
            {   
                hitIdx3 = hitIdx2;
            }
            else
            {
                event.addHitToEvent(r3LH.X(), r3LH.Y(), r3LH.Z(),1);
                hitIdx3 = *(hitsInGPU->nHits) - 1;
            }


            std::vector<unsigned int> hitIndices = {hitIdx0, hitIdx1, hitIdx2, hitIdx3}; 

            event.addPixelSegmentToEvent(hitIndices, pixelSegmentDeltaPhiChange, ptIn, ptErr, px, py, pz, etaErr);


       } 
    }
}

