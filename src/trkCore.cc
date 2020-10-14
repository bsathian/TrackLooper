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

        auto&& [x, y, z, subdet, detid] = data;

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
    TStopWatch my_timer;
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
