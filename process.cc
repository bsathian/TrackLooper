#include "process.h"

// ./process INPUTFILEPATH OUTPUTFILE [NEVENTS]
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
        ("i,input"       , "Comma separated input file list OR if just a directory is provided it will glob all in the directory BUT must end with '/' for the path", cxxopts::value<std::string>())
        ("t,tree"        , "Name of the tree in the root file to open and loop over"                                             , cxxopts::value<std::string>())
        ("o,output"      , "Output file name"                                                                                    , cxxopts::value<std::string>())
        ("n,nevents"     , "N events to loop over"                                                                               , cxxopts::value<int>()->default_value("-1"))
        ("d,debug"       , "Run debug job. i.e. overrides output option to 'debug.root' and 'recreate's the file.")
        ("h,help"        , "Print help")
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
    // --nevents
    ana.n_events = result["nevents"].as<int>();

    //
    // Printing out the option settings overview
    //
    std::cout <<  "=========================================================" << std::endl;
    std::cout <<  " Setting of the analysis job based on provided arguments " << std::endl;
    std::cout <<  "---------------------------------------------------------" << std::endl;
    std::cout <<  " ana.input_file_list_tstring: " << ana.input_file_list_tstring <<  std::endl;
    std::cout <<  " ana.output_tfile: " << ana.output_tfile->GetName() <<  std::endl;
    std::cout <<  " ana.n_events: " << ana.n_events <<  std::endl;
    std::cout <<  "=========================================================" << std::endl;

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

    // ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
    // Quick tutorial on RooUtil::Cutflow object cut tree formation
    // ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
    //
    // The RooUtil::Cutflow object facilitates creating a tree structure of cuts
    //
    // To add cuts to each node of the tree with cuts defined, use "addCut" or "addCutToLastActiveCut"
    // The "addCut" or "addCutToLastActiveCut" accepts three argument, <name>, and two lambda's that define the cut selection, and the weight to apply to that cut stage
    //
    // e.g. To create following cut-tree structure one does
    //
    //             (Root) <--- Always exists as soon as RooUtil::Cutflow object is created. But this is basically hidden underneath and users do not have to care
    //                |
    //            CutWeight
    //            |       |
    //     CutPreSel1    CutPreSel2
    //       |                  |
    //     CutSel1           CutSel2
    //
    //
    //   code:
    //
    //      // Create the object (Root node is created as soon as the instance is created)
    //      RooUtil::Cutflow cutflow;
    //
    //      cutflow.addCut("CutWeight"                 , <lambda> , <lambda>); // CutWeight is added below "Root"-node
    //      cutflow.addCutToLastActiveCut("CutPresel1" , <lambda> , <lambda>); // The last "active" cut is "CutWeight" since I just added that. So "CutPresel1" is added below "CutWeight"
    //      cutflow.addCutToLastActiveCut("CutSel1"    , <lambda> , <lambda>); // The last "active" cut is "CutPresel1" since I just added that. So "CutSel1" is added below "CutPresel1"
    //
    //      cutflow.getCut("CutWeight"); // By "getting" the cut object, this makes the "CutWeight" the last "active" cut.
    //      cutflow.addCutToLastActiveCut("CutPresel2" , <lambda> , <lambda>); // The last "active" cut is "CutWeight" since I "getCut" on it. So "CutPresel2" is added below "CutWeight"
    //      cutflow.addCutToLastActiveCut("CutSel2"    , <lambda> , <lambda>); // The last "active" cut is "CutPresel2" since I just added that. So "CutSel2" is added below "CutPresel1"
    //
    // (Side note: "UNITY" lambda is defined in the framework to just return 1. This so that use don't have to type [&]() {return 1;} so many times.)
    //
    // Once the cutflow is formed, create cutflow histograms can be created by calling RooUtil::Cutflow::bookCutflows())
    // This function looks through the terminating nodes of the tree structured cut tree. and creates a histogram that will fill the yields
    // For the example above, there are two terminationg nodes, "CutSel1", and "CutSel2"
    // So in this case Root::Cutflow::bookCutflows() will create two histograms. (Actually four histograms.)
    //
    //  - TH1F* type object :  CutSel1_cutflow (4 bins, with first bin labeled "Root", second bin labeled "CutWeight", third bin labeled "CutPreSel1", fourth bin labeled "CutSel1")
    //  - TH1F* type object :  CutSel2_cutflow (...)
    //  - TH1F* type object :  CutSel1_rawcutflow (...)
    //  - TH1F* type object :  CutSel2_rawcutflow (...)
    //                                ^
    //                                |
    // NOTE: There is only one underscore "_" between <CutName>_cutflow or <CutName>_rawcutflow
    //
    // And later in the loop when RooUtil::Cutflow::fill() function is called, the tree structure will be traversed through and the appropriate yields will be filled into the histograms
    //
    // After running the loop check for the histograms in the output root file

    // ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
    // Quick tutorial on RooUtil::Histograms object
    // ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
    //
    // The RooUtil::Histograms object facilitates book keeping histogram definitions
    // And in conjunction with RooUtil::Cutflow object, one can book same histograms across different cut stages easily without copy pasting codes many times by hand.
    //
    // The histogram addition happens in two steps.
    // 1. Defining histograms
    // 2. Booking histograms to cuts
    //
    // Histograms are defined via following functions
    //
    //      RooUtil::Histograms::addHistogram       : Typical 1D histogram (TH1F*) "Fill()" called once per event
    //      RooUtil::Histograms::addVecHistogram    : Typical 1D histogram (TH1F*) "Fill()" called multiple times per event
    //      RooUtil::Histograms::add2DHistogram     : Typical 2D histogram (TH2F*) "Fill()" called once per event
    //      RooUtil::Histograms::add2DVecHistogram  : Typical 2D histogram (TH2F*) "Fill()" called multiple times per event
    // e.g.
    //
    //    RooUtil::Histograms histograms;
    //    histograms.addHistogram   ("MllSS"    , 180 , 0. , 300. , [&]() { return www.MllSS()  ; }); // The lambda returns float
    //    histograms.addVecHistogram("AllLepPt" , 180 , 0. , 300. , [&]() { return www.lep_pt() ; }); // The lambda returns vector<float>
    //
    // The addVecHistogram will have lambda to return vector<float> and it will loop over the values and call TH1F::Fill() for each item
    //
    // To book histograms to cuts one uses
    //
    //      RooUtil::Cutflow::bookHistogramsForCut()
    //      RooUtil::Cutflow::bookHistogramsForCutAndBelow()
    //      RooUtil::Cutflow::bookHistogramsForCutAndAbove()
    //      RooUtil::Cutflow::bookHistogramsForEndCuts()
    //
    // e.g. Given a tree like the following, we can book histograms to various cuts as we want
    //
    //              Root
    //                |
    //            CutWeight
    //            |       |
    //     CutPreSel1    CutPreSel2
    //       |                  |
    //     CutSel1           CutSel2
    //
    // For example,
    //
    //    1. book a set of histograms to one cut:
    //
    //       cutflow.bookHistogramsForCut(histograms, "CutPreSel2")
    //
    //    2. book a set of histograms to a cut and below
    //
    //       cutflow.bookHistogramsForCutAndBelow(histograms, "CutWeight") // will book a set of histograms to CutWeight, CutPreSel1, CutPreSel2, CutSel1, and CutSel2
    //
    //    3. book a set of histograms to a cut and above (... useless...?)
    //
    //       cutflow.bookHistogramsForCutAndAbove(histograms, "CutPreSel2") // will book a set of histograms to CutPreSel2, CutWeight (nothing happens to Root node)
    //
    //    4. book a set of histograms to a terminating nodes
    //
    //       cutflow.bookHistogramsForEndCuts(histograms) // will book a set of histograms to CutSel1 and CutSel2
    //
    // The naming convention of the booked histograms are as follows
    //
    //   cutflow.bookHistogramsForCut(histograms, "CutSel1");
    //
    //  - TH1F* type object : CutSel1__MllSS;
    //  - TH1F* type object : CutSel1__AllLepPt;
    //                               ^^
    //                               ||
    // NOTE: There are two underscores "__" between <CutName>__<HistogramName>
    //
    // And later in the loop when RooUtil::CutName::fill() function is called, the tree structure will be traversed through and the appropriate histograms will be filled with appropriate variables
    // After running the loop check for the histograms in the output root file

    // Set the cutflow object output file
    ana.cutflow.setTFile(ana.output_tfile);
    // ana.cutflow.addCut("CutWeight", UNITY, UNITY);

    // Print cut structure
    ana.cutflow.printCuts();

    std::vector<float> dzs;
    std::vector<float> dzFracs;
    std::vector<float> drts;
    std::vector<float> fabsdPhiDiffs;
    std::vector<float> fabsdPhiModDiffs;
    ana.histograms.addVecHistogram("md_failed_dzs", 50, 0, 2, [&]() { return dzs; } );
    ana.histograms.addVecHistogram("md_failed_dzFracs", 50, 0, 0.05, [&]() { return dzFracs; } );
    ana.histograms.addVecHistogram("md_failed_drts", 50, 0, 20, [&]() { return drts; } );
    ana.histograms.addVecHistogram("md_failed_fabsdPhiDiffs", 50, -1, 1, [&]() { return fabsdPhiDiffs; } );
    ana.histograms.addVecHistogram("md_failed_fabsdPhiModDiffs", 50, -1, 5, [&]() { return fabsdPhiModDiffs; } );

    std::vector<float> dzs_matched;
    std::vector<float> dzFracs_matched;
    std::vector<float> drts_matched;
    std::vector<float> fabsdPhiDiffs_matched;
    std::vector<float> fabsdPhiModDiffs_matched;
    ana.histograms.addVecHistogram("md_matched_dzs", 50, 0, 2, [&]() { return dzs_matched; } );
    ana.histograms.addVecHistogram("md_matched_dzFracs", 50, 0, 0.05, [&]() { return dzFracs_matched; } );
    ana.histograms.addVecHistogram("md_matched_drts", 50, 0, 20, [&]() { return drts_matched; } );
    ana.histograms.addVecHistogram("md_matched_fabsdPhiDiffs", 50, -1, 1, [&]() { return fabsdPhiDiffs_matched; } );
    ana.histograms.addVecHistogram("md_matched_fabsdPhiModDiffs", 50, -1, 5, [&]() { return fabsdPhiModDiffs_matched; } );

    // pt_boundaries
    std::vector<float> pt_boundaries = {0, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.5, 2.0, 3.0, 5.0, 10, 15., 25, 50};
    // std::vector<float> pt_boundaries = {0, 0.5, 0.6, 0.7, 0.8, 0.9, 0.92, 0.94, 0.96, 0.98, 1.0, 1.02, 1.04, 1.06, 1.08, 1.1, 1.2, 1.5}; // lowpt

    // List of studies to perform
    std::vector<Study*> studies;
    studies.push_back(new StudyBarreldPhiChangeCutThresholdValidity());
    studies.push_back(new StudyEfficiency("studyEff", StudyEfficiency::kStudyEffAll, pt_boundaries));
    studies.push_back(new StudyEfficiency("studyEff", StudyEfficiency::kStudyEffBarrel, pt_boundaries));
    studies.push_back(new StudyEfficiency("studyEff", StudyEfficiency::kStudyEffBarrelFlat, pt_boundaries));
    studies.push_back(new StudyEfficiency("studyEff", StudyEfficiency::kStudyEffBarrelTilt, pt_boundaries));
    studies.push_back(new StudyEfficiency("studyEff", StudyEfficiency::kStudyEffBarrelTiltHighZ, pt_boundaries));
    studies.push_back(new StudyEfficiency("studyEff", StudyEfficiency::kStudyEffBarrelTiltLowZ, pt_boundaries));
    studies.push_back(new StudyEfficiency("studyEff", StudyEfficiency::kStudyEffEndcap, pt_boundaries));
    studies.push_back(new StudyEfficiency("studyEff", StudyEfficiency::kStudyEffEndcapPS, pt_boundaries));
    studies.push_back(new StudyEfficiency("studyEff", StudyEfficiency::kStudyEffEndcap2S, pt_boundaries));
    studies.push_back(new StudyEfficiency("studyEff", StudyEfficiency::kStudyEffEndcapPSCloseRing, pt_boundaries));
    studies.push_back(new StudyEfficiency("studyEff", StudyEfficiency::kStudyEffEndcapPSLowPt, pt_boundaries));
    studies.push_back(new StudySegmentEfficiency("studyEffSgAll", StudySegmentEfficiency::kStudyEffAll, pt_boundaries));
    studies.push_back(new StudySegmentEfficiency("studyEffSgBarrelBarrel", StudySegmentEfficiency::kStudyEffBarrelBarrel, pt_boundaries));
    studies.push_back(new StudySegmentEfficiency("studyEffSgBarrelFlatBarrel", StudySegmentEfficiency::kStudyEffBarrelFlatBarrel, pt_boundaries));
    studies.push_back(new StudySegmentEfficiency("studyEffSgBarrelTiltBarrel", StudySegmentEfficiency::kStudyEffBarrelTiltBarrel, pt_boundaries));
    studies.push_back(new StudySegmentEfficiency("studyEffSgBarrelFlatBarrelFlat", StudySegmentEfficiency::kStudyEffBarrelFlatBarrelFlat, pt_boundaries));
    studies.push_back(new StudySegmentEfficiency("studyEffSgBarrelFlatBarrelTilt", StudySegmentEfficiency::kStudyEffBarrelFlatBarrelTilt, pt_boundaries));
    studies.push_back(new StudySegmentEfficiency("studyEffSgBarrelTiltBarrelFlat", StudySegmentEfficiency::kStudyEffBarrelTiltBarrelFlat, pt_boundaries));
    studies.push_back(new StudySegmentEfficiency("studyEffSgBarrelTiltBarrelTilt", StudySegmentEfficiency::kStudyEffBarrelTiltBarrelTilt, pt_boundaries));
    studies.push_back(new StudySegmentEfficiency("studyEffSgBarrelEndcap", StudySegmentEfficiency::kStudyEffBarrelEndcap, pt_boundaries));
    studies.push_back(new StudySegmentEfficiency("studyEffSgBarrel", StudySegmentEfficiency::kStudyEffBarrel, pt_boundaries));
    studies.push_back(new StudySegmentEfficiency("studyEffSgEndcap", StudySegmentEfficiency::kStudyEffEndcap, pt_boundaries));
    studies.push_back(new StudySegmentEfficiency("studyEffSgEndcapPS", StudySegmentEfficiency::kStudyEffEndcapPS, pt_boundaries));
    studies.push_back(new StudySegmentEfficiency("studyEffSgEndcapPSPS", StudySegmentEfficiency::kStudyEffEndcapPSPS, pt_boundaries));
    studies.push_back(new StudySegmentEfficiency("studyEffSgEndcapPS2S", StudySegmentEfficiency::kStudyEffEndcapPS2S, pt_boundaries));
    studies.push_back(new StudySegmentEfficiency("studyEffSgEndcap2S", StudySegmentEfficiency::kStudyEffEndcap2S, pt_boundaries));

    // book the studies
    for (auto& study : studies)
    {
        study->bookStudy();
    }

    // Book Histograms
    ana.cutflow.bookHistograms(ana.histograms); // if just want to book everywhere

    // Set extendable axes
    // ana.cutflow.setHistsAxesExtendable(); // Make histogram axes extendable

    SDL::endcapGeometry.load("scripts/endcap_orientation_data.txt");
    SDL::tiltedGeometry.load("scripts/tilted_orientation_data.txt");
    SDL::moduleConnectionMap.load("scripts/module_connection_map_data.txt");

    // // Following maps to compute centroid of each modules
    // std::map<unsigned int, std::vector<float>> module_xs;
    // std::map<unsigned int, std::vector<float>> module_ys;
    // std::map<unsigned int, std::vector<float>> module_zs;

    // Looping input file
    while (ana.looper.nextEvent())
    {
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

        // Adding hits to modules
        for (unsigned int ihit = 0; ihit < trk.ph2_x().size(); ++ihit)
        {
            // Takes two arguments, SDL::Hit, and detId
            // SDL::Event internally will structure whether we already have the module instance or we need to create a new one.
            event.addHitToModule(
                    // a hit
                    SDL::Hit(trk.ph2_x()[ihit], trk.ph2_y()[ihit], trk.ph2_z()[ihit], ihit),
                    // add to module with "detId"
                    trk.ph2_detId()[ihit]
                    );

            // // Access hits on the S side of the PS modules in the endcaps and get three numbers, (detId, x, y)
            // SDL::Module module = SDL::Module(trk.ph2_detId()[ihit]);
            // if (module.subdet() == SDL::Module::Endcap and module.moduleType() == SDL::Module::TwoS and module.isLower())
            // {
            //     std::cout <<  " 'endcap2S': " << "endcap2S" <<  " trk.ph2_detId()[ihit]: " << trk.ph2_detId()[ihit] <<  " trk.ph2_x()[ihit]: " << trk.ph2_x()[ihit] <<  " trk.ph2_y()[ihit]: " << trk.ph2_y()[ihit] <<  " trk.ph2_z()[ihit]: " << trk.ph2_z()[ihit] <<  std::endl;
            // }

            // // To print the reco hits per module to create a table of centroids of each module
            // SDL::Module module = SDL::Module(trk.ph2_detId()[ihit]);
            // if (module.isLower())
            // {
            //     module_xs[trk.ph2_detId()[ihit]].push_back(trk.ph2_x()[ihit]);
            //     module_ys[trk.ph2_detId()[ihit]].push_back(trk.ph2_y()[ihit]);
            //     module_zs[trk.ph2_detId()[ihit]].push_back(trk.ph2_z()[ihit]);
            // }
            // else
            // {
            //     module_xs[module.partnerDetId()].push_back(trk.ph2_x()[ihit]);
            //     module_ys[module.partnerDetId()].push_back(trk.ph2_y()[ihit]);
            //     module_zs[module.partnerDetId()].push_back(trk.ph2_z()[ihit]);
            // }

        }

        // Create mini doublets
        // event.setLogLevel(SDL::Log_Debug2); // Set log level
        event.createMiniDoublets();

        // Create segments
        event.createSegments();

        // // Create segments
        // event.createTracklets(SDL::AllComb_TLAlgo);

        // Print content in the event
        // (SDL::cout is a modified version of std::cout where each line is prefixed by SDL::)
        if (ana.looper.getCurrentEventIndex() < 3) // Print for the first 10 events only
        {
            SDL::cout << event;
        }




        // *******************************************************
        // Formation of mini-doublets "candidates" from sim-tracks
        // *******************************************************

        // "Candidate" here means that it is just a pair of two hits without any selection applied.
        // i.e. no cuts are applied.
        // See http://uaf-10.t2.ucsd.edu/~phchang/talks/PhilipChang20190426_SDL_Update.pdf#page=7

        // Each SDL::Event object in simtrkevents will hold single sim-track related hits
        // It will be a vector of tuple of <sim_track_index, SDL::Event*>.
        std::vector<std::tuple<unsigned int, SDL::Event*>> simtrkevents;

        // Loop over sim-tracks and per sim-track aggregate good hits (i.e. matched with particle ID)
        // and only use those hits, and run mini-doublet reco algorithm on the sim-track-matched-reco-hits
        for (unsigned int isimtrk = 0; isimtrk < trk.sim_q().size(); ++isimtrk)
        {

            // Select only muon tracks
            if (abs(trk.sim_pdgId()[isimtrk]) != 13)
                continue;

            // // Select only 1 cm from center tracks
            // if (fabs(trk.sim_pca_dz()[isimtrk]) > 0.1)
            //     continue;

            // event just for this track
            SDL::Event* trackevent = new SDL::Event();

            // loop over the simulated hits
            for (auto& simhitidx : trk.sim_simHitIdx()[isimtrk])
            {

                int simhit_particle = trk.simhit_particle()[simhitidx];

                // Select only the sim hits that is matched to the muon
                if (abs(simhit_particle) != 13)
                    continue;

                // list of reco hit matched to this sim hit
                for (unsigned int irecohit = 0; irecohit < trk.simhit_hitIdx()[simhitidx].size(); ++irecohit)
                {

                    // Get the recohit type
                    int recohittype = trk.simhit_hitType()[simhitidx][irecohit];

                    // Consider only ph2 hits (i.e. outer tracker hits)
                    if (recohittype == 4)
                    {

                        int ihit = trk.simhit_hitIdx()[simhitidx][irecohit];

                        trackevent->addHitToModule(
                                // a hit
                                SDL::Hit(trk.ph2_x()[ihit], trk.ph2_y()[ihit], trk.ph2_z()[ihit], ihit),
                                // add to module with "detId"
                                trk.ph2_detId()[ihit]
                                );

                    }
                }

            }

            // Create mini-doublet CANDIDATES. i.e. create mini-doublet via ALL COMBINATION of hits
            trackevent->createMiniDoublets(SDL::AllComb_MDAlgo);
            // trackevent->createMiniDoublets(SDL::Default_MDAlgo);

            // Create mini-doublet CANDIDATES. i.e. create mini-doublet via ALL COMBINATION of hits
            trackevent->createSegments(SDL::AllComb_SGAlgo);

            // Push to the vector so we have a data-base of per hit, mini-doublets
            simtrkevents.push_back(std::make_tuple(isimtrk, trackevent));

        }


        // *****************************************************
        // Print module -> module connection info from sim track
        // *****************************************************
        // printModuleConnectionInfo();

        // ********************************************************************************************
        // Perform various studies with reco events and sim-track-matched-reco-hits-based mini-doublets
        // ********************************************************************************************

        for (auto& study : studies)
        {
            study->doStudy(event, simtrkevents);
        }


        // ************************************************
        // Now fill all the histograms booked by each study
        // ************************************************

        // Fill all the histograms
        ana.cutflow.fill();

        // <--------------------------
        // <--------------------------
        // <--------------------------
    }

    // std::map<unsigned int, float> module_center_x;
    // std::map<unsigned int, float> module_center_y;
    // std::map<unsigned int, float> module_center_z;

    // for (auto& key : module_xs)
    // {
    //     unsigned int detid = key.first;
    //     std::vector<float> fs = key.second;
    //     float sumf = 0;
    //     for (auto& f : fs)
    //         sumf += f;
    //     if (fs.size() > 0)
    //         module_center_x[detid] = sumf / fs.size();
    // }

    // for (auto& key : module_ys)
    // {
    //     unsigned int detid = key.first;
    //     std::vector<float> fs = key.second;
    //     float sumf = 0;
    //     for (auto& f : fs)
    //         sumf += f;
    //     if (fs.size() > 0)
    //         module_center_y[detid] = sumf / fs.size();
    // }

    // for (auto& key : module_zs)
    // {
    //     unsigned int detid = key.first;
    //     std::vector<float> fs = key.second;
    //     float sumf = 0;
    //     for (auto& f : fs)
    //         sumf += f;
    //     if (fs.size() > 0)
    //         module_center_z[detid] = sumf / fs.size();
    // }

    // for (auto& key : module_center_x)
    // {
    //     unsigned int detid = key.first;
    //     std::cout <<  " detid: " << detid <<  " module_center_x[detid]: " << module_center_x[detid] <<  " module_center_y[detid]: " << module_center_y[detid] <<  " module_center_z[detid]: " << module_center_z[detid] <<  std::endl;
    // }

    // Writing output file
    ana.cutflow.saveOutput();

    // The below can be sometimes crucial
    delete ana.output_tfile;
}

void printModuleConnectionInfo()
{
    // *****************************************************
    // Print module -> module connection info from sim track
    // *****************************************************

    // Loop over sim-tracks and per sim-track aggregate good hits (i.e. matched with particle ID)
    // and only use those hits, and run mini-doublet reco algorithm on the sim-track-matched-reco-hits
    for (unsigned int isimtrk = 0; isimtrk < trk.sim_q().size(); ++isimtrk)
    {

        // Select only muon tracks
        if (abs(trk.sim_pdgId()[isimtrk]) != 13)
            continue;

        if (trk.sim_pt()[isimtrk] < 1)
            continue;

        // // Select only 1 cm from center tracks
        // if (fabs(trk.sim_pca_dz()[isimtrk]) > 0.1)
        //     continue;

        std::vector<int> layers;
        std::vector<int> subdets;
        std::vector<int> detids;

        std::cout <<  " trk.event(): " << trk.event() <<  std::endl;
        std::cout <<  " isimtrk: " << isimtrk <<  std::endl;

        // loop over the simulated hits
        for (auto& simhitidx : trk.sim_simHitIdx()[isimtrk])
        {

            // Select only the hits in the outer tracker
            if (not (trk.simhit_subdet()[simhitidx] == 4 or trk.simhit_subdet()[simhitidx] == 5))
                continue;

            int simhit_particle = trk.simhit_particle()[simhitidx];
            int detid = trk.simhit_detId()[simhitidx];
            int layer = trk.simhit_layer()[simhitidx];
            float x = trk.simhit_x()[simhitidx];
            float y = trk.simhit_y()[simhitidx];
            float z = trk.simhit_z()[simhitidx];
            float r3 = sqrt(x*x + y*y + z*z);
            int subdet = trk.simhit_subdet()[simhitidx];
            int trkidx = trk.simhit_simTrkIdx()[simhitidx];
            SDL::Module module = SDL::Module(detid);

            std::cout <<  " layer: " << layer <<  " subdet: " << subdet <<  " simhit_particle: " << simhit_particle <<  " x: " << x <<  " y: " << y <<  " z: " << z <<  " r3: " << r3 <<  " trkidx: " << trkidx <<  " module.isLower(): " << module.isLower() <<  " detid: " << detid <<  std::endl;

            // Select only the sim hits that is matched to the muon
            if (abs(simhit_particle) != 13)
                continue;

            // Access hits on the S side of the PS modules in the endcaps and get three numbers, (detId, x, y)
            if (module.isLower())
            {
                layers.push_back(layer);
                subdets.push_back(subdet);
                detids.push_back(detid);
            }

        }

        if (layers.size() > 0)
        {
            std::cout <<  " trk.sim_pt()[isimtrk]: " << trk.sim_pt()[isimtrk] <<  " trk.sim_phi()[isimtrk]: " << trk.sim_phi()[isimtrk] <<  " trk.sim_eta()[isimtrk]: " << trk.sim_eta()[isimtrk] <<  " trk.sim_pca_dz()[isimtrk]: " << trk.sim_pca_dz()[isimtrk] <<  " trk.sim_pca_dxy()[isimtrk]: " << trk.sim_pca_dxy()[isimtrk] <<  " trk.sim_pca_lambda()[isimtrk]: " << trk.sim_pca_lambda()[isimtrk] <<  " trk.sim_pca_cotTheta()[isimtrk]: " << trk.sim_pca_cotTheta()[isimtrk] <<  " trk.sim_pca_phi()[isimtrk]: " << trk.sim_pca_phi()[isimtrk] <<  std::endl;
            std::cout << "moduleconnection: ";
            for (unsigned int i = 0; i < layers.size(); ++i)
            {
                std::cout << "(" << layers[i] << "," << subdets[i] << "," << detids[i] << "," << SDL::Module(detids[i]).partnerDetId() << ");";
            }
            std::cout << std::endl;
            std::cout << trk.event() << " " << isimtrk << " ";
            for (unsigned int i = 0; i < layers.size(); ++i)
            {
                std::cout << detids[i] << "," << SDL::Module(detids[i]).partnerDetId();
                if (i != layers.size() - 1)
                    std::cout << ",";
            }
            std::cout << std::endl;
        }

    }
}

