#include "doAnalysis.h"

// ./process INPUTFILEPATH OUTPUTFILE [NEVENTS]
//

/*void addPixelSegments(SDL::Event& event, int isimtrk)
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
//        int hitidx0 = trk.see_hitIdx()[iSeed][0];
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
}*/
int main(int argc, char** argv)
{

//********************************************************************************
//
// 1. Parsing options
//
//********************************************************************************

    // cxxopts is just a tool to parse argc, and argv easily

    // Grand option setting
    cxxopts::Options options("\n  $ doAnalysis",  "\n         **********************\n         *                    *\n         *       Looper       *\n         *                    *\n         **********************\n");

    // Read the options
    options.add_options()
        ("i,input"          , "Comma separated input file list OR if just a directory is provided it will glob all in the directory BUT must end with '/' for the path", cxxopts::value<std::string>())
        ("t,tree"           , "Name of the tree in the root file to open and loop over"                                             , cxxopts::value<std::string>())
        ("o,output"         , "Output file name"                                                                                    , cxxopts::value<std::string>())
        ("n,nevents"        , "N events to loop over"                                                                               , cxxopts::value<int>()->default_value("-1"))
        ("x,event_index"    , "specific event index to process"                                                                     , cxxopts::value<int>()->default_value("-1"))
        ("p,ptbound_mode"   , "Pt bound mode (i.e. 0 = default, 1 = pt~1, 2 = pt~0.95-1.5, 3 = pt~0.5-1.5, 4 = pt~0.5-2.0"          , cxxopts::value<int>()->default_value("0"))
        ("g,pdg_id"         , "The simhit pdgId match option (default = 13)"                                                        , cxxopts::value<int>()->default_value("13"))
        ("v,verbose"        , "Verbose mode"                                                                                        , cxxopts::value<int>()->default_value("0"))
        ("l,run_ineff_study", "Write debug ntuples 0 = MDs, 1 = SGs, 2 = TLs, 3 = TCs"                                              , cxxopts::value<int>()->default_value("-1"))
        ("d,debug"          , "Run debug job. i.e. overrides output option to 'debug.root' and 'recreate's the file.")
        ("c,print_conn"     , "Print module connections")
        ("b,print_boundary" , "Print module boundaries")
        ("r,centroid"       , "Print centroid information")
        ("e,run_eff_study"  , "Run efficiency study")
        ("m,run_mtv_study"  , "Run MTV study")
        ("j,nsplit_jobs"    , "Enable splitting jobs by N blocks (--job_index must be set)", cxxopts::value<int>())
        ("I,job_index"      , "job_index of split jobs (--nsplit_jobs must be set. index starts from 0. i.e. 0, 1, 2, 3, etc...)", cxxopts::value<int>())
        ("h,help"           , "Print help")
        ;

    auto result = options.parse(argc, argv);

    // NOTE: When an option was provided (e.g. -i or --input), then the result.count("<option name>") is more than 0
    // Therefore, the option can be parsed easily by asking the condition if (result.count("<option name>");
    // That's how the several options are parsed below

    //_______________________________________________________________________________
    // --help
    if (result.count("help"))
    {
        std::cout << options.help() << std::endl;
        exit(1);
    }

    //_______________________________________________________________________________
    // --input
    if (result.count("input"))
    {
        ana.input_file_list_tstring = result["input"].as<std::string>();
    }
    else
    {
        std::cout << options.help() << std::endl;
        std::cout << "ERROR: Input list is not provided! Check your arguments" << std::endl;
        exit(1);
    }

    //_______________________________________________________________________________
    // --tree
    if (result.count("tree"))
    {
        ana.input_tree_name = result["tree"].as<std::string>();
    }
    else
    {
        std::cout << options.help() << std::endl;
        std::cout << "ERROR: Input tree name is not provided! Check your arguments" << std::endl;
        exit(1);
    }

    //_______________________________________________________________________________
    // --debug
    if (result.count("debug"))
    {
        ana.output_tfile = new TFile("debug.root", "recreate");
    }
    else
    {
        //_______________________________________________________________________________
        // --output
        if (result.count("output"))
        {
            ana.output_tfile = new TFile(result["output"].as<std::string>().c_str(), "create");
            if (not ana.output_tfile->IsOpen())
            {
                std::cout << options.help() << std::endl;
                std::cout << "ERROR: output already exists! provide new output name or delete old file. OUTPUTFILE=" << result["output"].as<std::string>() << std::endl;
                exit(1);
            }
        }
        else
        {
            std::cout << options.help() << std::endl;
            std::cout << "ERROR: Output file name is not provided! Check your arguments" << std::endl;
            exit(1);
        }
    }

    //_______________________________________________________________________________
    // --print_conn
    if (result.count("print_conn"))
    {
        ana.print_conn = true;
    }
    else
    {
        ana.print_conn = false;
    }

    //_______________________________________________________________________________
    // --print_boundary
    if (result.count("print_boundary"))
    {
        ana.print_boundary = true;
    }
    else
    {
        ana.print_boundary = false;
    }

    //_______________________________________________________________________________
    // --run_eff_study
    if (result.count("run_eff_study"))
    {
        ana.run_eff_study = true;
    }
    else
    {
        ana.run_eff_study = false;
    }

    //_______________________________________________________________________________
    // --run_ineff_study
    if (result.count("run_ineff_study"))
    {
        ana.mode_write_ineff_study_debug_ntuple = result["run_ineff_study"].as<int>();
        if (result["run_ineff_study"].as<int>() >= 0)
        {
            ana.run_ineff_study = true;
        }
        else
        {
            ana.run_ineff_study = false;
        }
    }

    //_______________________________________________________________________________
    // --run_mtv_study
    if (result.count("run_mtv_study"))
    {
        ana.run_mtv_study = true;
    }
    else
    {
        ana.run_mtv_study = false;
    }

    //_______________________________________________________________________________
    // --centroid
    if (result.count("centroid"))
    {
        ana.print_centroid = true;
    }
    else
    {
        ana.print_centroid = false;
    }

    //_______________________________________________________________________________
    // --nevents
    ana.n_events = result["nevents"].as<int>();
    ana.specific_event_index = result["event_index"].as<int>();

    // -1 upto mini-doublet is all-comb
    // -2 upto segment is all-comb
    // -3 upto tracklet is all-comb NOTE: MEMORY WILL BLOW UP FOR HIGH PU
    // -4 upto trackcandidate is all-comb NOTE: MEMORY WILL BLOW UP FOR HIGH PU
    //  0 nothing
    //  1 upto mini-doublet is all-comb
    //  2 upto mini-doublet is default segment is all-comb
    //  3 upto segment is default tracklet is all-comb
    //  4 upto tracklet is default trackcandidate is all-comb
    ana.ptbound_mode = result["ptbound_mode"].as<int>();

    //_______________________________________________________________________________
    // --pdg_id
    ana.pdg_id = result["pdg_id"].as<int>();

    //_______________________________________________________________________________
    // --nsplit_jobs
    if (result.count("nsplit_jobs"))
    {
        ana.nsplit_jobs = result["nsplit_jobs"].as<int>();
        if (ana.nsplit_jobs <= 0)
        {
            std::cout << options.help() << std::endl;
            std::cout << "ERROR: option string --nsplit_jobs" << ana.nsplit_jobs << " has zero or negative value!" << std::endl;
            std::cout << "I am not sure what this means..." << std::endl;
            exit(1);
        }
    }
    else
    {
        ana.nsplit_jobs = -1;
    }

    //_______________________________________________________________________________
    // --job_index
    if (result.count("job_index"))
    {
        ana.job_index = result["job_index"].as<int>();
        if (ana.job_index < 0)
        {
            std::cout << options.help() << std::endl;
            std::cout << "ERROR: option string --job_index" << ana.job_index << " has negative value!" << std::endl;
            std::cout << "I am not sure what this means..." << std::endl;
            exit(1);
        }
    }
    else
    {
        ana.job_index = -1;
    }

    // Sanity check for split jobs (if one is set the other must be set too)
    if (result.count("job_index") or result.count("nsplit_jobs"))
    {
        // If one is not provided then throw error
        if ( not (result.count("job_index") and result.count("nsplit_jobs")))
        {
            std::cout << options.help() << std::endl;
            std::cout << "ERROR: option string --job_index and --nsplit_jobs must be set at the same time!" << std::endl;
            exit(1);
        }
        // If it is set then check for sanity
        else
        {
            if (ana.job_index >= ana.nsplit_jobs)
            {
                std::cout << options.help() << std::endl;
                std::cout << "ERROR: --job_index >= --nsplit_jobs ! This does not make sense..." << std::endl;
                exit(1);
            }
        }
    }

    //_______________________________________________________________________________
    // --verbose
    ana.verbose = result["verbose"].as<int>();

    //
    // Printing out the option settings overview
    //
    std::cout <<  "=========================================================" << std::endl;
    std::cout <<  " Setting of the analysis job based on provided arguments " << std::endl;
    std::cout <<  "---------------------------------------------------------" << std::endl;
    std::cout <<  " ana.input_file_list_tstring: " << ana.input_file_list_tstring <<  std::endl;
    std::cout <<  " ana.output_tfile: " << ana.output_tfile->GetName() <<  std::endl;
    std::cout <<  " ana.n_events: " << ana.n_events <<  std::endl;
    std::cout <<  " ana.run_eff_study: " << ana.run_eff_study <<  std::endl;
    std::cout <<  " ana.run_ineff_study: " << ana.run_ineff_study <<  std::endl;
    std::cout <<  " ana.run_mtv_study: " << ana.run_mtv_study <<  std::endl;
    std::cout <<  " ana.print_centroid: " << ana.print_centroid <<  std::endl;
    std::cout <<  " ana.print_conn: " << ana.print_conn <<  std::endl;
    std::cout <<  " ana.print_boundary: " << ana.print_boundary <<  std::endl;
    std::cout <<  " ana.ptbound_mode: " << ana.ptbound_mode <<  std::endl;
    std::cout <<  " ana.nsplit_jobs: " << ana.nsplit_jobs <<  std::endl;
    std::cout <<  " ana.job_index: " << ana.job_index <<  std::endl;
    std::cout <<  "=========================================================" << std::endl;

    // Consistency check
    if ((ana.run_ineff_study and ana.run_eff_study) or
        (ana.run_ineff_study and ana.run_mtv_study) or
        (ana.run_eff_study and ana.run_mtv_study)
       )
    {
        RooUtil::error("More than one of -e, -l, or -m option is set! Please only set one of them");
    }


//********************************************************************************
//
// 2. Opening input baby files
//
//********************************************************************************

    // Create the TChain that holds the TTree's of the baby ntuples
    ana.events_tchain = RooUtil::FileUtil::createTChain(ana.input_tree_name, ana.input_file_list_tstring);

    // Create a Looper object to loop over input files
    // the "www" object is defined in the wwwtree.h/cc
    // This is an instance which helps read variables in the WWW baby TTree
    // It is a giant wrapper that facilitates reading TBranch values.
    // e.g. if there is a TBranch named "lep_pt" which is a std::vector<float> then, one can access the branch via
    //
    //    std::vector<float> lep_pt = www.lep_pt();
    //
    // and no need for "SetBranchAddress" and declaring variable shenanigans necessary
    // This is a standard thing SNT does pretty much every looper we use
    ana.looper.init(ana.events_tchain, &trk, ana.n_events);

//********************************************************************************
//
// Interlude... notes on RooUtil framework
//
//********************************************************************************

    // Set the cutflow object output file
    ana.cutflow.setTFile(ana.output_tfile);
    // ana.cutflow.addCut("CutWeight", UNITY, UNITY);

    // Create ttree to output to the ana.output_tfile
    ana.output_ttree = new TTree("tree", "tree");

    // Create TTreeX instance that will take care of the interface part of TTree
    ana.tx = new RooUtil::TTreeX(ana.output_ttree);

    // Print cut structure
    ana.cutflow.printCuts();

    // pt_boundaries
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
        pt_boundaries = {0.5, 0.52, 0.54, 0.56, 0.58, 0.6, 0.62, 0.64, 0.66, 0.68, 0.7, 0.72, 0.74, 0.76, 0.78, 0.8, 0.82, 0.84, 0.86, 0.88, 0.9, 0.92, 0.94, 0.96, 0.98, 1.0, 1.02, 1.04, 1.06, 1.08, 1.1, 1.12, 1.14, 1.16, 1.18, 1.2, 1.22, 1.24, 1.26, 1.28, 1.3, 1.32, 1.34, 1.36, 1.38, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8, 2.9, 3.0}; // lowpt
    else if (ana.ptbound_mode == 6)
        pt_boundaries = {0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 2.0, 3.0, 4.0, 5.0}; // lowpt
        // pt_boundaries = {0.5, 0.52, 0.54, 0.56, 0.58, 0.6, 0.62, 0.64, 0.66, 0.68, 0.7, 0.72, 0.74, 0.76, 0.78, 0.8, 0.82, 0.84, 0.86, 0.88, 0.9, 0.92, 0.94, 0.96, 0.98, 1.0, 1.02, 1.04, 1.06, 1.08, 1.1, 1.12, 1.14, 1.16, 1.18, 1.2, 1.22, 1.24, 1.26, 1.28, 1.3, 1.32, 1.34, 1.36, 1.38, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8, 2.9, 3.0, 3.2, 3.4, 3.6, 3.8, 4.0, 4.5, 5.0}; // lowpt

    // List of studies to perform
   
    std::vector<Study*> studies;
    studies.push_back(new StudySegmentOccupancy("studySegmentOccupancy"));
    studies.push_back(new StudyTrackletOccupancy("studyTrackletOccupancy"));
    studies.push_back(new StudyTripletOccupancy("studyTripletOccupancy"));
    studies.push_back(new StudyTrackCandidateOccupancy("studyTrackCandidateOccupancy"));
    studies.push_back(new StudyTrackCandidateOccupancyT4T4("studyTrackCandidateOccupancyT4T4"));
    studies.push_back(new StudyTrackCandidateOccupancyT4T3("studyTrackCandidateOccupancyT4T3"));
    studies.push_back(new StudyTrackCandidateOccupancyT3T4("studyTrackCandidateOccupancyT3T4"));
    studies.push_back(new StudyPixelTrackletOccupancy("studyPixelTrackletOuterSegmentOccupancy"));

    // book the studies
    for (auto& study : studies)
    {
        study->bookStudy();
    }

    // Book Histograms
    ana.cutflow.bookHistograms(ana.histograms); // if just want to book everywhere

    // SDL::endcapGeometry.load("scripts/endcap_orientation_data.txt");
    SDL::endcapGeometry.load("data/endcap_orientation_data_v2.txt"); // centroid values added to the map
    SDL::tiltedGeometry.load("data/tilted_orientation_data.txt");
    SDL::moduleConnectionMap.load("data/module_connection_combined_2020_0520_helixray.txt");
//    SDL::moduleConnectionMap.load("/home/users/phchang/public_html/analysis/sdl/TrackLooper_/scripts/module_connection_map_data_10_e0_200_100_pt0p8_2p0_400_pt0p8_2p0_nolossers_dxy35cm_endcaplayer2.txt");


//    SDL::moduleConnectionMap.load("data/module_connection_2020_0429.txt");

    // // Following maps to compute centroid of each modules
    std::map<unsigned int, std::vector<float>> module_xs;
    std::map<unsigned int, std::vector<float>> module_ys;
    std::map<unsigned int, std::vector<float>> module_zs;

    // connection information
    std::ofstream module_connection_log_output;
    if (ana.print_conn)
        module_connection_log_output.open("conn.txt");

    // module boundary information to be written out in case module boundary info is asked to be printed
    std::ofstream module_boundary_output_info;
    if (ana.print_boundary)
        module_boundary_output_info.open("module_boundary.txt");

    // Write the simhits in a given module to the output TTree
    if (ana.print_boundary)
    {
        ana.tx->createBranch<int>("detId");
        ana.tx->createBranch<int>("subdet");
        ana.tx->createBranch<int>("side");
        ana.tx->createBranch<int>("layer");
        ana.tx->createBranch<int>("rod");
        ana.tx->createBranch<int>("module");
        ana.tx->createBranch<int>("ring");
        ana.tx->createBranch<int>("isPS");
        ana.tx->createBranch<int>("isStrip");
        ana.tx->createBranch<vector<float>>("x");
        ana.tx->createBranch<vector<float>>("y");
        ana.tx->createBranch<vector<float>>("z");
    }

    //create them modules
    SDL::initModules();

    // Looping input file
    while (ana.looper.nextEvent())
    {

        if (ana.specific_event_index >= 0)
        {
            if ((int)ana.looper.getCurrentEventIndex() != ana.specific_event_index)
                continue;
        }

        // If splitting jobs are requested then determine whether to process the event or not based on remainder
        if (result.count("job_index") and result.count("nsplit_jobs"))
        {
            if (ana.looper.getNEventsProcessed() % ana.nsplit_jobs != (unsigned int) ana.job_index)
                continue;
        }

        if (ana.verbose) std::cout <<  " ana.looper.getCurrentEventIndex(): " << ana.looper.getCurrentEventIndex() <<  std::endl;

        // *****************************************************
        // Print module -> module connection info from sim track
        // *****************************************************
/*        if (ana.print_conn)
        {
            // Print the module connection info and do nothing else on the event
            printModuleConnectionInfo(module_connection_log_output);
            continue;
        }*/

        // *****************************************************
        // Print module boundaries
        // *****************************************************
/*        if (ana.print_boundary)
        {
            // Print the module connection info and do nothing else on the event
            processModuleBoundaryInfo();
            continue;
        }*/

/*        if (ana.print_centroid)
        {
            // Adding hits to modules
            for (unsigned int ihit = 0; ihit < trk.ph2_x().size(); ++ihit)
            {

                // To print the reco hits per module to create a table of centroids of each module
                SDL::Module module = SDL::Module(trk.ph2_detId()[ihit]);
                if (module.isLower())
                {
                    module_xs[trk.ph2_detId()[ihit]].push_back(trk.ph2_x()[ihit]);
                    module_ys[trk.ph2_detId()[ihit]].push_back(trk.ph2_y()[ihit]);
                    module_zs[trk.ph2_detId()[ihit]].push_back(trk.ph2_z()[ihit]);
                }
                else
                {
                    module_xs[module.partnerDetId()].push_back(trk.ph2_x()[ihit]);
                    module_ys[module.partnerDetId()].push_back(trk.ph2_y()[ihit]);
                    module_zs[module.partnerDetId()].push_back(trk.ph2_z()[ihit]);
                }

            }

            continue;

        }*/

        // <--------------------------
        // <--------------------------
        // <--------------------------
        //
        // ***************************
        // Testing SDL Implementations
        // ***************************
        //

        // *************************************************
        // Reconstructed hits and formation of mini-doublets
        // *************************************************

        // Main instance that will hold modules, hits, minidoublets, etc. (i.e. main data structure)
        SDL::Event event;

        // Each SDL::Event object in simtrkevents will hold single sim-track related hits
        // It will be a vector of tuple of <sim_track_index, SDL::Event*>.
        std::vector<std::tuple<unsigned int, SDL::Event*>> simtrkevents;
        std::vector<std::tuple<unsigned int, SDL::EventForAnalysisInterface*>> simtrkeventsForAnalysisInterface;

        TStopwatch my_timer;

        // run_eff_study == 0 then run all the reconstruction
      // 
        //if ((ana.run_eff_study == 0 and ana.run_ineff_study == 0) or ana.run_mtv_study)
        

            // Adding hits to modules
            for (unsigned int ihit = 0; ihit < trk.ph2_x().size(); ++ihit)
            {
                event.addHitToEvent(trk.ph2_x()[ihit], trk.ph2_y()[ihit], trk.ph2_z()[ihit],trk.ph2_detId()[ihit], ihit);
            }

            float elapsed = 0;

            // ----------------
            if (ana.verbose != 0) std::cout << "Summary of hits" << std::endl;
            if (ana.verbose != 0) std::cout << "# of Hits: " << event.getNumberOfHits() << std::endl;
            if (ana.verbose != 0) std::cout << "# of Hits in layer 1: " << event.getNumberOfHitsByLayerBarrel(0) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Hits in layer 2: " << event.getNumberOfHitsByLayerBarrel(1) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Hits in layer 3: " << event.getNumberOfHitsByLayerBarrel(2) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Hits in layer 4: " << event.getNumberOfHitsByLayerBarrel(3) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Hits in layer 5: " << event.getNumberOfHitsByLayerBarrel(4) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Hits in layer 6: " << event.getNumberOfHitsByLayerBarrel(5) << std::endl;
/*            if (ana.verbose != 0) std::cout << "# of Hits Upper Module in layer 1: " << event.getNumberOfHitsByLayerBarrelUpperModule(0) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Hits Upper Module in layer 2: " << event.getNumberOfHitsByLayerBarrelUpperModule(1) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Hits Upper Module in layer 3: " << event.getNumberOfHitsByLayerBarrelUpperModule(2) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Hits Upper Module in layer 4: " << event.getNumberOfHitsByLayerBarrelUpperModule(3) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Hits Upper Module in layer 5: " << event.getNumberOfHitsByLayerBarrelUpperModule(4) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Hits Upper Module in layer 6: " << event.getNumberOfHitsByLayerBarrelUpperModule(5) << std::endl;*/
            // ----------------


            // ----------------
            if (ana.verbose != 0) std::cout << "Reco Mini-Doublet start" << std::endl;
            my_timer.Start();
            event.createMiniDoublets();
            // event.createPseudoMiniDoubletsFromAnchorModule(); // Useless.....
            float md_elapsed = my_timer.RealTime();
            
            if (ana.verbose != 0) std::cout << "Reco Mini-doublet processing time: " << md_elapsed << " secs" << std::endl;

            if (ana.verbose != 0) std::cout << "# of Mini-doublets produced: " << event.getNumberOfMiniDoublets() << std::endl;
            if (ana.verbose != 0) std::cout << "# of Mini-doublets produced barrel layer 1: " << event.getNumberOfMiniDoubletsByLayerBarrel(0) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Mini-doublets produced barrel layer 2: " << event.getNumberOfMiniDoubletsByLayerBarrel(1) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Mini-doublets produced barrel layer 3: " << event.getNumberOfMiniDoubletsByLayerBarrel(2) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Mini-doublets produced barrel layer 4: " << event.getNumberOfMiniDoubletsByLayerBarrel(3) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Mini-doublets produced barrel layer 5: " << event.getNumberOfMiniDoubletsByLayerBarrel(4) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Mini-doublets produced barrel layer 6: " << event.getNumberOfMiniDoubletsByLayerBarrel(5) << std::endl;
/*
            if (ana.verbose != 0) std::cout << "# of Mini-doublets considered: " << event.getNumberOfMiniDoubletCandidates() << std::endl;
            if (ana.verbose != 0) std::cout << "# of Mini-doublets considered barrel layer 1: " << event.getNumberOfMiniDoubletCandidatesByLayerBarrel(0) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Mini-doublets considered barrel layer 2: " << event.getNumberOfMiniDoubletCandidatesByLayerBarrel(1) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Mini-doublets considered barrel layer 3: " << event.getNumberOfMiniDoubletCandidatesByLayerBarrel(2) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Mini-doublets considered barrel layer 4: " << event.getNumberOfMiniDoubletCandidatesByLayerBarrel(3) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Mini-doublets considered barrel layer 5: " << event.getNumberOfMiniDoubletCandidatesByLayerBarrel(4) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Mini-doublets considered barrel layer 6: " << event.getNumberOfMiniDoubletCandidatesByLayerBarrel(5) << std::endl;*/

            if (ana.verbose != 0) std::cout << "# of Mini-doublets produced endcap layer 1: " << event.getNumberOfMiniDoubletsByLayerEndcap(0) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Mini-doublets produced endcap layer 2: " << event.getNumberOfMiniDoubletsByLayerEndcap(1) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Mini-doublets produced endcap layer 3: " << event.getNumberOfMiniDoubletsByLayerEndcap(2) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Mini-doublets produced endcap layer 4: " << event.getNumberOfMiniDoubletsByLayerEndcap(3) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Mini-doublets produced endcap layer 5: " << event.getNumberOfMiniDoubletsByLayerEndcap(4) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Mini-doublets produced endcap layer 6: " << event.getNumberOfMiniDoubletsByLayerEndcap(5) << std::endl;
/*            if (ana.verbose != 0) std::cout << "# of Mini-doublets considered endcap layer 1: " << event.getNumberOfMiniDoubletCandidatesByLayerEndcap(0) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Mini-doublets considered endcap layer 2: " << event.getNumberOfMiniDoubletCandidatesByLayerEndcap(1) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Mini-doublets considered endcap layer 3: " << event.getNumberOfMiniDoubletCandidatesByLayerEndcap(2) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Mini-doublets considered endcap layer 4: " << event.getNumberOfMiniDoubletCandidatesByLayerEndcap(3) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Mini-doublets considered endcap layer 5: " << event.getNumberOfMiniDoubletCandidatesByLayerEndcap(4) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Mini-doublets considered endcap layer 6: " << event.getNumberOfMiniDoubletCandidatesByLayerEndcap(5) << std::endl;*/


            // ----------------

            // ----------------
            // 

            if (ana.verbose != 0) std::cout << "Reco Segment start" << std::endl;
            my_timer.Start(kFALSE);
            event.createSegmentsWithModuleMap();
            float sg_elapsed = my_timer.RealTime();
            if (ana.verbose != 0) std::cout << "Reco Segment processing time: " << sg_elapsed - md_elapsed << " secs" << std::endl;
            if (ana.verbose != 0) std::cout << "# of Segments produced: " << event.getNumberOfSegments() << std::endl;
            if (ana.verbose != 0) std::cout << "# of Segments produced layer 1-2: " << event.getNumberOfSegmentsByLayerBarrel(0) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Segments produced layer 2-3: " << event.getNumberOfSegmentsByLayerBarrel(1) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Segments produced layer 3-4: " << event.getNumberOfSegmentsByLayerBarrel(2) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Segments produced layer 4-5: " << event.getNumberOfSegmentsByLayerBarrel(3) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Segments produced layer 5-6: " << event.getNumberOfSegmentsByLayerBarrel(4) << std::endl;


            // ----------------
            if (ana.verbose != 0) std::cout << "Reco Triplet start" << std::endl;
            my_timer.Start(kFALSE);
            event.createTriplets();
            float tp_elapsed = my_timer.RealTime();
            if (ana.verbose != 0) std::cout << "Reco Triplet processing time: " << tp_elapsed - sg_elapsed << " secs" << std::endl;
            if (ana.verbose != 0) std::cout << "# of Triplets produced: " << event.getNumberOfTriplets() << std::endl;
            if (ana.verbose != 0) std::cout << "# of Triplets produced layer 1-2-3: " << event.getNumberOfTripletsByLayerBarrel(0) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Triplets produced layer 2-3-4: " << event.getNumberOfTripletsByLayerBarrel(1) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Triplets produced layer 3-4-5: " << event.getNumberOfTripletsByLayerBarrel(2) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Triplets produced layer 4-5-6: " << event.getNumberOfTripletsByLayerBarrel(3) << std::endl;
	    if (ana.verbose != 0) std::cout << "# of Triplets produced endcap layer 1-2-3: " << event.getNumberOfTripletsByLayerEndcap(0) << std::endl;
	    if (ana.verbose != 0) std::cout << "# of Triplets produced endcap layer 2-3-4: " << event.getNumberOfTripletsByLayerEndcap(1) << std::endl;
	    if (ana.verbose != 0) std::cout << "# of Triplets produced endcap layer 3-4-5: " << event.getNumberOfTripletsByLayerEndcap(2) << std::endl;
            // ----------------
        
            //pixel tracklet addition
            if(ana.verbose != 0) std::cout<<"Adding Pixel segments!"<<std::endl;
            addPixelSegments(event,-1);
            if(ana.verbose != 0) std::cout<<"# of Pixel Segments: "<<event.getNumberOfPixelSegments()<<std::endl;
            if(ana.verbose != 0) std::cout<<" Reco Pixel tracklet start"<<std::endl;
            my_timer.Start(kFALSE);
            event.createPixelTracklets();
            float ptl_elapsed = my_timer.RealTime();
            if (ana.verbose != 0) std::cout << "# of Pixel Tracklets: "<<event.getNumberOfPixelTracklets()<<std::endl;
            if (ana.verbose != 0) std::cout << "Reco Pixel Tracklet processing time: " << ptl_elapsed - tp_elapsed << " secs" << std::endl;

            // ----------------
            if (ana.verbose != 0) std::cout << "Reco Tracklet start" << std::endl;
            my_timer.Start(kFALSE);
            // event.createTracklets();
             event.createTrackletsWithModuleMap();
//             event.createTrackletsWithAGapWithModuleMap();
            //event.createTrackletsViaNavigation();
            float tl_elapsed = my_timer.RealTime();
            if (ana.verbose != 0) std::cout << "Reco Tracklet processing time: " << tl_elapsed - ptl_elapsed << " secs" << std::endl;
            if (ana.verbose != 0) std::cout << "# of Tracklets produced: " << event.getNumberOfTracklets() << std::endl;
            if (ana.verbose != 0) std::cout << "# of Tracklets produced layer 1-2-3-4: " << event.getNumberOfTrackletsByLayerBarrel(0) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Tracklets produced layer 2-3-4-5: " << event.getNumberOfTrackletsByLayerBarrel(1) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Tracklets produced layer 3-4-5-6: " << event.getNumberOfTrackletsByLayerBarrel(2) << std::endl;
             if (ana.verbose != 0) std::cout << "# of Tracklets produced layer 4: " << event.getNumberOfTrackletsByLayerBarrel(3) << std::endl;
             if (ana.verbose != 0) std::cout << "# of Tracklets produced layer 5: " << event.getNumberOfTrackletsByLayerBarrel(4) << std::endl;
             if (ana.verbose != 0) std::cout << "# of Tracklets produced layer 6: " << event.getNumberOfTrackletsByLayerBarrel(5) << std::endl;
            /*if (ana.verbose != 0) std::cout << "# of Tracklets considered: " << event.getNumberOfTrackletCandidates() << std::endl;
            if (ana.verbose != 0) std::cout << "# of Tracklets considered layer 1-2-3-4: " << event.getNumberOfTrackletCandidatesByLayerBarrel(0) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Tracklets considered layer 2-3-4-5: " << event.getNumberOfTrackletCandidatesByLayerBarrel(1) << std::endl;
            if (ana.verbose != 0) std::cout << "# of Tracklets considered layer 3-4-5-6: " << event.getNumberOfTrackletCandidatesByLayerBarrel(2) << std::endl;
            // if (ana.verbose != 0) std::cout << "# of Tracklets considered layer 4: " << event.getNumberOfTrackletCandidatesByLayerBarrel(3) << std::endl;
            // if (ana.verbose != 0) std::cout << "# of Tracklets considered layer 5: " << event.getNumberOfTrackletCandidatesByLayerBarrel(4) << std::endl;
            // if (ana.verbose != 0) std::cout << "# of Tracklets considered layer 6: " << event.getNumberOfTrackletCandidatesByLayerBarrel(5) << std::endl;*/
            // ----------------


            // ----------------
            if (ana.verbose != 0) std::cout << "Reco TrackCandidate start" << std::endl;
            my_timer.Start(kFALSE);
            // event.createTrackCandidatesFromTriplets();
            // event.createTrackCandidates();
            event.createTrackCandidates();
            float tc_elapsed = my_timer.RealTime();
            if (ana.verbose != 0) std::cout << "Reco TrackCandidate processing time: " << tc_elapsed - tl_elapsed << " secs" << std::endl;
            if (ana.verbose != 0) std::cout << "# of TrackCandidates produced: " << event.getNumberOfTrackCandidates() << std::endl;
            if (ana.verbose != 0) std::cout << "# of TrackCandidates produced layer 1-2-3-4-5-6: " << event.getNumberOfTrackCandidatesByLayerBarrel(0) << std::endl;
             if (ana.verbose != 0) std::cout << "# of TrackCandidates produced layer 2: " << event.getNumberOfTrackCandidatesByLayerBarrel(1) << std::endl;
             if (ana.verbose != 0) std::cout << "# of TrackCandidates produced layer 3: " << event.getNumberOfTrackCandidatesByLayerBarrel(2) << std::endl;
             if (ana.verbose != 0) std::cout << "# of TrackCandidates produced layer 4: " << event.getNumberOfTrackCandidatesByLayerBarrel(3) << std::endl;
             if (ana.verbose != 0) std::cout << "# of TrackCandidates produced layer 5: " << event.getNumberOfTrackCandidatesByLayerBarrel(4) << std::endl;
             if (ana.verbose != 0) std::cout << "# of TrackCandidates produced layer 6: " << event.getNumberOfTrackCandidatesByLayerBarrel(5) << std::endl;
//            if (ana.verbose != 0) std::cout << "# of TrackCandidates considered: " << event.getNumberOfTrackCandidateCandidates() << std::endl;
            //if (ana.verbose != 0) std::cout << "# of TrackCandidates considered layer 1-2-3-4-5-6: " << event.getNumberOfTrackCandidateCandidatesByLayerBarrel(0) << std::endl;
            // if (ana.verbose != 0) std::cout << "# of TrackCandidates considered layer 2: " << event.getNumberOfTrackCandidateCandidatesByLayerBarrel(1) << std::endl;
            // if (ana.verbose != 0) std::cout << "# of TrackCandidates considered layer 3: " << event.getNumberOfTrackCandidateCandidatesByLayerBarrel(2) << std::endl;
            // if (ana.verbose != 0) std::cout << "# of TrackCandidates considered layer 4: " << event.getNumberOfTrackCandidateCandidatesByLayerBarrel(3) << std::endl;
            // if (ana.verbose != 0) std::cout << "# of TrackCandidates considered layer 5: " << event.getNumberOfTrackCandidateCandidatesByLayerBarrel(4) << std::endl;
            // if (ana.verbose != 0) std::cout << "# of TrackCandidates considered layer 6: " << event.getNumberOfTrackCandidateCandidatesByLayerBarrel(5) << std::endl;
            // ----------------

            if (ana.verbose != 0) std::cout << "Reco SDL end" << std::endl;

        // If efficiency is to be calculated






        // ********************************************************************************************
        // Perform various studies with reco events and sim-track-matched-reco-hits-based mini-doublets
        // ********************************************************************************************
    //create the reco study analysis object
   
       SDL::EventForAnalysisInterface* eventForAnalysisInterface = new SDL::EventForAnalysisInterface(SDL::modulesInGPU, event.getHits(), event.getMiniDoublets(), event.getSegments(), event.getTracklets(),event.getTriplets(),event.getTrackCandidates());



        // ************************************************
        // Now fill all the histograms booked by each study
        // ************************************************
        for(auto& study : studies)
        {
            study->doStudy(*eventForAnalysisInterface,simtrkeventsForAnalysisInterface);
        }
        // Fill all the histograms
        ana.cutflow.fill();

        delete eventForAnalysisInterface;
        // <--------------------------
        // <--------------------------
        // <--------------------------
    }

    // Writing output file
    ana.cutflow.saveOutput();

    // Writing ttree output to file
    ana.output_ttree->Write();

    // The below can be sometimes crucial
    delete ana.output_tfile;
}


