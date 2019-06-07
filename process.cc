#include "process.h"

AnalysisConfig ana;

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

    // List of studies to perform
    std::vector<Study*> studies;
    studies.push_back(new StudyBarreldPhiChangeCutThresholdValidity());
    studies.push_back(new StudyEfficiency("studyEff", StudyEfficiency::kStudyEffAll, /*pt_boundaries=*/{0, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.5, 2.0, 3.0, 5.0, 10, 15., 25, 50}));
    studies.push_back(new StudyEfficiency("studyEff", StudyEfficiency::kStudyEffBarrel, /*pt_boundaries=*/{0, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.5, 2.0, 3.0, 5.0, 10, 15., 25, 50}));
    studies.push_back(new StudyEfficiency("studyEff", StudyEfficiency::kStudyEffBarrelFlat, /*pt_boundaries=*/{0, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.5, 2.0, 3.0, 5.0, 10, 15., 25, 50}));
    studies.push_back(new StudyEfficiency("studyEff", StudyEfficiency::kStudyEffBarrelTilt, /*pt_boundaries=*/{0, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.5, 2.0, 3.0, 5.0, 10, 15., 25, 50}));
    studies.push_back(new StudyEfficiency("studyEff", StudyEfficiency::kStudyEffEndcap, /*pt_boundaries=*/{0, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.5, 2.0, 3.0, 5.0, 10, 15., 25, 50}));
    studies.push_back(new StudyEfficiency("studyEff", StudyEfficiency::kStudyEffEndcapPS, /*pt_boundaries=*/{0, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.5, 2.0, 3.0, 5.0, 10, 15., 25, 50}));
    studies.push_back(new StudyEfficiency("studyEff", StudyEfficiency::kStudyEffEndcap2S, /*pt_boundaries=*/{0, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.5, 2.0, 3.0, 5.0, 10, 15., 25, 50}));
    studies.push_back(new StudyEfficiency("studyEff", StudyEfficiency::kStudyEffEndcapPSCloseRing, /*pt_boundaries=*/{0, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.5, 2.0, 3.0, 5.0, 10, 15., 25, 50}));
    studies.push_back(new StudyEfficiency("studyEff", StudyEfficiency::kStudyEffEndcapPSLowPt, /*pt_boundaries=*/{0, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.5, 2.0, 3.0, 5.0, 10, 15., 25, 50}));
    studies.push_back(new StudyEndcapInefficiency("studyEndcapInEff", StudyEndcapInefficiency::kStudyEndcapIneffAll));
    studies.push_back(new StudyEndcapInefficiency("studyEndcapInEff", StudyEndcapInefficiency::kStudyEndcapIneffPS));
    studies.push_back(new StudyEndcapInefficiency("studyEndcapInEff", StudyEndcapInefficiency::kStudyEndcapIneff2S));
    studies.push_back(new StudyEndcapInefficiency("studyEndcapInEff", StudyEndcapInefficiency::kStudyEndcapIneffPSLowP));
    studies.push_back(new StudyEndcapInefficiency("studyEndcapInEff", StudyEndcapInefficiency::kStudyEndcapIneffPSLowS));

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

            // Access hits on the S side of the PS modules in the endcaps and get three numbers, (detId, x, y)
            SDL::Module module = SDL::Module(trk.ph2_detId()[ihit]);
            // if (module.subdet() == SDL::Module::Barrel and module.side() != SDL::Module::Center and module.moduleType() == SDL::Module::PS and module.moduleLayerType() == SDL::Module::Strip)
            // {
            //     std::cout <<  " 'tilted': " << "tilted" <<  " trk.ph2_detId()[ihit]: " << trk.ph2_detId()[ihit] <<  " trk.ph2_x()[ihit]: " << trk.ph2_x()[ihit] <<  " trk.ph2_y()[ihit]: " << trk.ph2_y()[ihit] <<  " trk.ph2_z()[ihit]: " << trk.ph2_z()[ihit] <<  std::endl;
            // }

        }

        // Create mini doublets
        // event.setLogLevel(SDL::Log_Debug2); // Set log level
        event.createMiniDoublets();

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

            // Push to the vector so we have a data-base of per hit, mini-doublets
            simtrkevents.push_back(std::make_tuple(isimtrk, trackevent));

        }



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



    // Writing output file
    ana.cutflow.saveOutput();

    // The below can be sometimes crucial
    delete ana.output_tfile;
}


void Study::bookStudy() {} // To be overloaded
void Study::doStudy(SDL::Event& recoevent, std::vector<std::tuple<unsigned int, SDL::Event*>> simtrkevents) {} // To be overloaded

void StudyBarreldPhiChangeCutThresholdValidity::bookStudy()
{
    // Book Histograms
    ana.histograms.add2DVecHistogram("rt", 50, 20, 120, "phim", 50, 0., 1., [&]() { return rt_v_phim__rt; }, [&]() { return rt_v_phim__phim; } );
    for (int ii = 0; ii < NLAYERS; ++ii)
    {
        ana.histograms.add2DVecHistogram("rt", 50, 20, 120, TString::Format("phim_by_layer%d", ii), 50, 0., 1., [&, ii]() { return rt_v_phim__rt_by_layer[ii]; }, [&, ii]() { return rt_v_phim__phim_by_layer[ii]; } );
    }

    ana.histograms.add2DVecHistogram("rt", 50, 20, 120, "dphi", 50, 0., 1., [&]() { return rt_v_dphi__rt; }, [&]() { return rt_v_dphi__dphi; } );
    for (int ii = 0; ii < NLAYERS; ++ii)
    {
        ana.histograms.add2DVecHistogram("rt", 50, 20, 120, TString::Format("dphi_by_layer%d", ii), 50, 0., 1., [&, ii]() { return rt_v_dphi__rt_by_layer[ii]; }, [&, ii]() { return rt_v_dphi__dphi_by_layer[ii]; } );
    }

}

void StudyBarreldPhiChangeCutThresholdValidity::doStudy(SDL::Event& recoevent, std::vector<std::tuple<unsigned int, SDL::Event*>> simtrkevents)
{

    // Each do study is performed per event

    // First clear all the output variables that will be used to fill the histograms for this event
    rt_v_phim__rt.clear();
    rt_v_phim__phim.clear();
    for (int ii = 0; ii < NLAYERS; ++ii)
    {
        rt_v_phim__rt_by_layer[ii].clear();
        rt_v_phim__phim_by_layer[ii].clear();
    }

    rt_v_dphi__rt.clear();
    rt_v_dphi__dphi.clear();
    for (int ii = 0; ii < NLAYERS; ++ii)
    {
        rt_v_dphi__rt_by_layer[ii].clear();
        rt_v_dphi__dphi_by_layer[ii].clear();
    }

    for (auto& simtrkevent : simtrkevents)
    {

        unsigned int& isimtrk = std::get<0>(simtrkevent);
        SDL::Event& trackevent = *(std::get<1>(simtrkevent));

        // Plotting dPhiChange's and the thresholds etc. (study done for 04/29/2019 http://uaf-10.t2.ucsd.edu/~phchang/talks/PhilipChang20190429_SDL_Update.pdf)
        for (auto& lowerModulePtr_Track : trackevent.getLowerModulePtrs())
        {

            float pt = trk.sim_pt()[isimtrk];

            // Commented in or out for the plots
            // if (pt > 1.)
            //     continue;

            if (pt < 1.)
                continue;

            // Fou
            if (lowerModulePtr_Track->subdet() == SDL::Module::Barrel and lowerModulePtr_Track->side() == SDL::Module::Center)
            {

                for (auto& lowerHitPtr : lowerModulePtr_Track->getHitPtrs())
                {
                    rt_v_phim__rt.push_back(lowerHitPtr->rt());
                    rt_v_phim__phim.push_back(SDL::MiniDoublet::dPhiThreshold(*lowerHitPtr, *lowerModulePtr_Track));
                    int layer_idx = lowerModulePtr_Track->layer() - 1;
                    rt_v_phim__rt_by_layer[layer_idx].push_back(lowerHitPtr->rt());
                    rt_v_phim__phim_by_layer[layer_idx].push_back(SDL::MiniDoublet::dPhiThreshold(*lowerHitPtr, *lowerModulePtr_Track));
                }

                for (auto& md_Track : lowerModulePtr_Track->getMiniDoubletPtrs())
                {
                    SDL::Hit* lowerHitPtr = md_Track->lowerHitPtr();
                    float rt = lowerHitPtr->rt();
                    float dphi = lowerHitPtr->deltaPhiChange(*(md_Track->upperHitPtr()));
                    rt_v_dphi__rt.push_back(rt);
                    rt_v_dphi__dphi.push_back(dphi);
                    int layer_idx = lowerModulePtr_Track->layer() - 1;
                    rt_v_dphi__rt_by_layer[layer_idx].push_back(rt);
                    rt_v_dphi__dphi_by_layer[layer_idx].push_back(dphi);
                }

            }

        }

    }

}

StudyEfficiency::StudyEfficiency(const char* studyName, StudyEfficiency::StudyEfficiencyMode mode_, std::vector<float> ptbounds)
{

    studyname = studyName;
    mode = mode_;
    switch (mode)
    {
        case kStudyEffAll: modename = "all"; break;
        case kStudyEffBarrel: modename = "barrel"; break;
        case kStudyEffBarrelFlat: modename = "barrelflat"; break;
        case kStudyEffBarrelTilt: modename = "barreltilt"; break;
        case kStudyEffEndcap: modename = "endcap"; break;
        case kStudyEffEndcapPS: modename = "endcapPS"; break;
        case kStudyEffEndcap2S: modename = "endcap2S"; break;
        case kStudyEffEndcapPSCloseRing: modename = "endcapPSCloseRing"; break;
        case kStudyEffEndcapPSLowPt: modename = "endcapPSLowPt"; break;
        default: modename = "UNDEFINED"; break;
    }
    pt_boundaries = ptbounds;

}

void StudyEfficiency::bookStudy()
{
    // Book Histograms
    ana.histograms.addVecHistogram(TString::Format("md_%s_matched_track_pt", modename), pt_boundaries, [&]() { return md_matched_track_pt; } );
    ana.histograms.addVecHistogram(TString::Format("md_%s_all_track_pt", modename), pt_boundaries, [&]() { return md_all_track_pt; } );
    ana.histograms.addVecHistogram(TString::Format("md_%s_matched_track_eta", modename), 50, -4, 4, [&]() { return md_matched_track_eta; } );
    ana.histograms.addVecHistogram(TString::Format("md_%s_all_track_eta", modename), 50, -4, 4, [&]() { return md_all_track_eta; } );
    ana.histograms.addVecHistogram(TString::Format("md_%s_matched_track_phi", modename), 200, -3.1416, 3.1416, [&]() { return md_matched_track_phi; } );
    ana.histograms.addVecHistogram(TString::Format("md_%s_all_track_phi", modename), 200, -3.1416, 3.1416, [&]() { return md_all_track_phi; } );
    ana.histograms.addVecHistogram(TString::Format("md_%s_matched_track_z", modename), 200, -300, 300, [&]() { return md_matched_track_z; } );
    ana.histograms.addVecHistogram(TString::Format("md_%s_all_track_z", modename), 200, -300, 300, [&]() { return md_all_track_z; } );

    const int nlayers = NLAYERS;
    for (int ii = 0; ii < nlayers; ++ii)
    {
        ana.histograms.addVecHistogram(TString::Format("md_%s_matched_track_pt_by_layer%d", modename, ii), pt_boundaries, [&, ii]() { return md_matched_track_pt_by_layer[ii]; } );
        ana.histograms.addVecHistogram(TString::Format("md_%s_all_track_pt_by_layer%d", modename, ii), pt_boundaries, [&, ii]() { return md_all_track_pt_by_layer[ii]; } );
    }
    for (int ii = 0; ii < nlayers; ++ii)
    {
        ana.histograms.addVecHistogram(TString::Format("md_%s_matched_track_eta_by_layer%d", modename, ii), 50, -4, 4, [&, ii]() { return md_matched_track_eta_by_layer[ii]; } );
        ana.histograms.addVecHistogram(TString::Format("md_%s_all_track_eta_by_layer%d", modename, ii), 50, -4, 4, [&, ii]() { return md_all_track_eta_by_layer[ii]; } );
    }
    for (int ii = 0; ii < nlayers; ++ii)
    {
        ana.histograms.addVecHistogram(TString::Format("md_%s_matched_track_phi_by_layer%d", modename, ii), 200, -3.1416, 3.1416, [&, ii]() { return md_matched_track_phi_by_layer[ii]; } );
        ana.histograms.addVecHistogram(TString::Format("md_%s_all_track_phi_by_layer%d", modename, ii), 200, -3.1416, 3.1416, [&, ii]() { return md_all_track_phi_by_layer[ii]; } );
    }
    for (int ii = 0; ii < nlayers; ++ii)
    {
        ana.histograms.addVecHistogram(TString::Format("md_%s_matched_track_wrapphi_by_layer%d", modename, ii), 200, -3.1416, 3.1416, [&, ii]() { return md_matched_track_wrapphi_by_layer[ii]; } );
        ana.histograms.addVecHistogram(TString::Format("md_%s_all_track_wrapphi_by_layer%d", modename, ii), 200, -3.1416, 3.1416, [&, ii]() { return md_all_track_wrapphi_by_layer[ii]; } );
    }
    for (int ii = 0; ii < nlayers; ++ii)
    {
        ana.histograms.addVecHistogram(TString::Format("md_%s_matched_track_z_by_layer%d", modename, ii), 200, -300, 300, [&, ii]() { return md_matched_track_z_by_layer[ii]; } );
        ana.histograms.addVecHistogram(TString::Format("md_%s_all_track_z_by_layer%d", modename, ii), 200, -300, 300, [&, ii]() { return md_all_track_z_by_layer[ii]; } );
    }
    ana.histograms.addVecHistogram(TString::Format("md_%s_lower_hit_only_track_pt", modename), pt_boundaries, [&]() { return md_lower_hit_only_track_pt; } );
    ana.histograms.addVecHistogram(TString::Format("md_%s_lower_hit_only_track_eta", modename), 50, -4, 4, [&]() { return md_lower_hit_only_track_eta; } );
}

void StudyEfficiency::doStudy(SDL::Event& event, std::vector<std::tuple<unsigned int, SDL::Event*>> simtrkevents)
{
    // Each do study is performed per event

    // First clear all the output variables that will be used to fill the histograms for this event
    md_matched_track_pt.clear();
    md_all_track_pt.clear();
    md_matched_track_eta.clear();
    md_all_track_eta.clear();
    md_matched_track_phi.clear();
    md_all_track_phi.clear();
    md_matched_track_z.clear();
    md_all_track_z.clear();
    for (int ii = 0; ii < NLAYERS; ++ii)
    {
        md_matched_track_pt_by_layer[ii].clear();
        md_all_track_pt_by_layer[ii].clear();
        md_matched_track_eta_by_layer[ii].clear();
        md_all_track_eta_by_layer[ii].clear();
        md_matched_track_phi_by_layer[ii].clear();
        md_all_track_phi_by_layer[ii].clear();
        md_matched_track_wrapphi_by_layer[ii].clear();
        md_all_track_wrapphi_by_layer[ii].clear();
        md_matched_track_z_by_layer[ii].clear();
        md_all_track_z_by_layer[ii].clear();
    }
    md_lower_hit_only_track_pt.clear();
    md_lower_hit_only_track_eta.clear();

    //***********************
    // Efficiency calculation
    //***********************

    // Loop over track events
    for (auto& simtrkevent : simtrkevents)
    {

        // Unpack the tuple (sim_track_index, SDL::Event containing reco hits only matched to the given sim track)
        unsigned int& isimtrk = std::get<0>(simtrkevent);
        SDL::Event& trackevent = *(std::get<1>(simtrkevent));

        // Loop over the lower modules that contains hits for this track
        for (auto& lowerModulePtr_Track : trackevent.getLowerModulePtrs())
        {

            // Depending on the mode, only run a subset of interested modules
            switch (mode)
            {
                case kStudyEffAll: /* do nothing */ break;
                case kStudyEffBarrel: if (not (lowerModulePtr_Track->subdet() == SDL::Module::Barrel)) { continue; } break;
                case kStudyEffBarrelFlat: if (not (lowerModulePtr_Track->subdet() == SDL::Module::Barrel and lowerModulePtr_Track->side() == SDL::Module::Center)) { continue; } break;
                case kStudyEffBarrelTilt: if (not (lowerModulePtr_Track->subdet() == SDL::Module::Barrel and lowerModulePtr_Track->side() != SDL::Module::Center)) { continue; } break;
                case kStudyEffEndcap: if (not (lowerModulePtr_Track->subdet() == SDL::Module::Endcap)) { continue; } break;
                case kStudyEffEndcapPS: if (not (lowerModulePtr_Track->subdet() == SDL::Module::Endcap and lowerModulePtr_Track->moduleType() == SDL::Module::PS)) { continue; } break;
                case kStudyEffEndcap2S: if (not (lowerModulePtr_Track->subdet() == SDL::Module::Endcap and lowerModulePtr_Track->moduleType() == SDL::Module::TwoS)) { continue; } break;
                case kStudyEffEndcapPSCloseRing: if (not (lowerModulePtr_Track->subdet() == SDL::Module::Endcap and lowerModulePtr_Track->moduleType() == SDL::Module::PS and lowerModulePtr_Track->ring() <= 1 and lowerModulePtr_Track->layer() < 3)) { continue; } break;
                case kStudyEffEndcapPSLowPt: if (not (lowerModulePtr_Track->subdet() == SDL::Module::Endcap and lowerModulePtr_Track->moduleType() == SDL::Module::PS and lowerModulePtr_Track->ring() <= 1 and lowerModulePtr_Track->layer() < 3 and trk.sim_pt()[isimtrk] > 1 and trk.sim_pt()[isimtrk] < 2)) { continue; } break;
                default: /* skip everything should not be here anyways...*/ continue; break;
            }

            // Parse the layer index later to be used for indexing
            int layer_idx = lowerModulePtr_Track->layer() - 1;

            // Parse pt and eta of this track
            float pt = std::min((double) trk.sim_pt()[isimtrk], 49.999);
            float eta = trk.sim_eta()[isimtrk];
            float phi = trk.sim_phi()[isimtrk];

            // For this module that a sim-track supposedly passed through if there are no more than 1 mini-doublets
            // it means that this track did not leave at least one hit each in each side of the module
            // The getMiniDoubletPtrs() will return ALL pairs of hits between the modules.
            // That's because the simtrkevent would have createMiniDoublets called with SDL::AllComb_MDAlgo option.
            // This option loops over all hits in lower module and upper module and forms every possible pair.
            // So if the following condition of size() == 0 is true, it means this sim-track didn't leave at least one hit in each side.
            if (lowerModulePtr_Track->getMiniDoubletPtrs().size() == 0)
            {
                md_lower_hit_only_track_pt.push_back(pt);
                md_lower_hit_only_track_pt.push_back(eta);
                continue;
            }

            // Boolean to test whether for this module that a track passed through, whether it found a matched mini-doublet
            bool match = false;

            // Loop over the md "candidate" from the module that a sim-track passed through and left at least one hit in each module
            float z = 0; // The z position of this "truth candidate" mini-doublet will be calculated by the average of all combos
            for (auto& md_Track : lowerModulePtr_Track->getMiniDoubletPtrs())
            {

                // To ge
                z += md_Track->lowerHitPtr()->z();

                // Loop over the md reconstructed from with proper SDL algorithm and if the index of the hits match
                // Then we have found at least one mini-doublet associated to this track's reco-hits in this module
                // Therefore flag the match boolean
                for (auto& md : event.getModule(lowerModulePtr_Track->detId()).getMiniDoubletPtrs())
                {
                    if (md_Track->isIdxMatched(*md))
                    {
                        match = true;
                    }
                }
            }

            if (lowerModulePtr_Track->getMiniDoubletPtrs().size() > 0)
                z /= lowerModulePtr_Track->getMiniDoubletPtrs().size();

            // At this stage, we have either found a mini-doublet in this module matched to the track or not.

            // Denominator for all layers pt efficiency
            md_all_track_pt.push_back(pt);

            // Denominator for all layers eta efficiency (notice the 1 GeV cut)
            if (trk.sim_pt()[isimtrk] > 1.)
                md_all_track_eta.push_back(eta);

            // Denominator for all layers phi efficiency (notice the 1 GeV cut)
            if (trk.sim_pt()[isimtrk] > 1.)
                md_all_track_phi.push_back(phi);

            // Denominator for all layers z efficiency (notice the 1 GeV cut)
            if (trk.sim_pt()[isimtrk] > 1.)
                md_all_track_z.push_back(z);

            // Numerators
            if (match)
            {

                // Numerators for matched all layers pt efficiency
                md_matched_track_pt.push_back(pt);

                // Numeratosr for matched all layers eta efficiency (notice the 1 GeV cut)
                if (trk.sim_pt()[isimtrk] > 1.)
                    md_matched_track_eta.push_back(eta);

                // Numeratosr for matched all layers phi efficiency (notice the 1 GeV cut)
                if (trk.sim_pt()[isimtrk] > 1.)
                    md_matched_track_phi.push_back(phi);

                // Numeratosr for matched all layers z efficiency (notice the 1 GeV cut)
                if (trk.sim_pt()[isimtrk] > 1.)
                    md_matched_track_z.push_back(z);

            }
            // Failed tracks for all layers
            else
            {
                // Doing nothing for now ...
            }

            // Denominator for specific layers pt efficiency
            md_all_track_pt_by_layer[layer_idx].push_back(pt);

            // Denominator for specific layers eta efficiency (notice the 1 GeV cut)
            if (trk.sim_pt()[isimtrk] > 1.)
                md_all_track_eta_by_layer[layer_idx].push_back(eta);

            // Denominator for specific layers phi efficiency (notice the 1 GeV cut)
            if (trk.sim_pt()[isimtrk] > 1.)
                md_all_track_phi_by_layer[layer_idx].push_back(phi);

            // Denominator for specific layers z efficiency (notice the 1 GeV cut)
            if (trk.sim_pt()[isimtrk] > 1.)
                md_all_track_z_by_layer[layer_idx].push_back(z);

            // Denominator for specific layers wrapphi efficiency (notice the 1 GeV cut)
            float wrapphi = 0;
            if (layer_idx == 0)
                wrapphi = fmod(fabs(phi), 2*TMath::Pi() / 18);
            else if (layer_idx == 1)
                wrapphi = fmod(fabs(phi), 2*TMath::Pi() / 26);
            else
                wrapphi = fmod(fabs(phi), 2*TMath::Pi() / 36);
            if (trk.sim_pt()[isimtrk] > 5.)
                md_all_track_wrapphi_by_layer[layer_idx].push_back(wrapphi);

            // Numerators
            if (match)
            {
                // Numerators for matched specific layer pt effciency
                md_matched_track_pt_by_layer[layer_idx].push_back(pt);

                // Numerators for matched specific layer eta effciency (notice the 1 GeV cut)
                if (trk.sim_pt()[isimtrk] > 1.)
                    md_matched_track_eta_by_layer[layer_idx].push_back(eta);

                // Numerators for matched specific layer phi effciency (notice the 1 GeV cut)
                if (trk.sim_pt()[isimtrk] > 1.)
                    md_matched_track_phi_by_layer[layer_idx].push_back(phi);

                // Numerators for matched specific layer wrapphi effciency (notice the 1 GeV cut)
                if (trk.sim_pt()[isimtrk] > 5.)
                    md_matched_track_wrapphi_by_layer[layer_idx].push_back(wrapphi);

                // Numerators for matched specific layer z effciency (notice the 1 GeV cut)
                if (trk.sim_pt()[isimtrk] > 1.)
                    md_matched_track_z_by_layer[layer_idx].push_back(z);

            }
            // Failed tracks for specific layers
            else
            {
            }

        }

    }

}

StudyEndcapInefficiency::StudyEndcapInefficiency(const char* studyName, StudyEndcapInefficiency::StudyEndcapInefficiencyMode mode_)
{

    studyname = studyName;
    mode = mode_;
    switch (mode)
    {
        case kStudyEndcapIneffAll: modename = "all"; break;
        case kStudyEndcapIneffPS: modename = "PS"; break;
        case kStudyEndcapIneff2S: modename = "2S"; break;
        case kStudyEndcapIneffPSLowP: modename = "PSLowP"; break;
        case kStudyEndcapIneffPSLowS: modename = "PSLowS"; break;
        default: modename = "UNDEFINED"; break;
    }
}

void StudyEndcapInefficiency::bookStudy()
{

    // Book histograms to study the mini-doublet candidate either passing or failing
    ana.histograms.addVecHistogram(TString::Format("mdcand_%s_dzs_passed"             , modename) , 50 , 0  , 2    , [&]() { return dzs_passed              ; } );
    ana.histograms.addVecHistogram(TString::Format("mdcand_%s_drts_passed"            , modename) , 50 ,-2  , 2    , [&]() { return drts_passed             ; } );
    ana.histograms.addVecHistogram(TString::Format("mdcand_%s_fabsdPhis_passed"       , modename) , 50 , 0  , 0.1  , [&]() { return fabsdPhis_passed        ; } );
    ana.histograms.addVecHistogram(TString::Format("mdcand_%s_dzfracs_passed"         , modename) , 50 , 0  , 0.005 , [&]() { return dzfracs_passed          ; } );
    ana.histograms.addVecHistogram(TString::Format("mdcand_%s_zs_passed"              , modename) , 50 , 0  , 400  , [&]() { return zs_passed               ; } );
    ana.histograms.addVecHistogram(TString::Format("mdcand_%s_fabsdPhiMods_passed"    , modename) , 60 , -1 , 5    , [&]() { return fabsdPhiMods_passed     ; } );
    ana.histograms.addVecHistogram(TString::Format("mdcand_%s_fabsdPhiModDiffs_passed", modename) , 60 , -1 , 5    , [&]() { return fabsdPhiModDiffs_passed ; } );

    ana.histograms.addVecHistogram(TString::Format("mdcand_%s_dzs_failed"             , modename) , 50 , 0  , 2    , [&]() { return dzs_failed              ; } );
    ana.histograms.addVecHistogram(TString::Format("mdcand_%s_drts_failed"            , modename) , 50 ,-2  , 2    , [&]() { return drts_failed             ; } );
    ana.histograms.addVecHistogram(TString::Format("mdcand_%s_fabsdPhis_failed"       , modename) , 50 , 0  , 0.1  , [&]() { return fabsdPhis_failed        ; } );
    ana.histograms.addVecHistogram(TString::Format("mdcand_%s_dzfracs_failed"         , modename) , 50 , 0  , 0.005, [&]() { return dzfracs_failed          ; } );
    ana.histograms.addVecHistogram(TString::Format("mdcand_%s_zs_failed"              , modename) , 50 , 0  , 400  , [&]() { return zs_failed               ; } );
    ana.histograms.addVecHistogram(TString::Format("mdcand_%s_fabsdPhiMods_failed"    , modename) , 60 , -1 , 5    , [&]() { return fabsdPhiMods_failed     ; } );
    ana.histograms.addVecHistogram(TString::Format("mdcand_%s_fabsdPhiModDiffs_failed", modename) , 60 , -1 , 5    , [&]() { return fabsdPhiModDiffs_failed ; } );

}

void StudyEndcapInefficiency::doStudy(SDL::Event& event, std::vector<std::tuple<unsigned int, SDL::Event*>> simtrkevents)
{

    dzs_passed.clear();
    drts_passed.clear();
    fabsdPhis_passed.clear();
    zs_passed.clear();
    dzfracs_passed.clear();
    fabsdPhiMods_passed.clear();
    fabsdPhiModDiffs_passed.clear();

    dzs_failed.clear();
    drts_failed.clear();
    fabsdPhis_failed.clear();
    zs_failed.clear();
    dzfracs_failed.clear();
    fabsdPhiMods_failed.clear();
    fabsdPhiModDiffs_failed.clear();

    //*******************
    // Inefficiency study
    //*******************

    // Loop over track events
    for (auto& simtrkevent : simtrkevents)
    {

        // Unpack the tuple (sim_track_index, SDL::Event containing reco hits only matched to the given sim track)
        unsigned int& isimtrk = std::get<0>(simtrkevent);
        SDL::Event& trackevent = *(std::get<1>(simtrkevent));

        // Loop over the lower modules that contains hits for this track
        for (auto& lowerModulePtr_Track : trackevent.getLowerModulePtrs())
        {

            // Depending on the mode, only run a subset of interested modules
            switch (mode)
            {
                case kStudyEndcapIneffAll: if (not (lowerModulePtr_Track->subdet() == SDL::Module::Endcap)) { continue; } break;
                case kStudyEndcapIneffPS: if (not (lowerModulePtr_Track->subdet() == SDL::Module::Endcap and lowerModulePtr_Track->moduleType() == SDL::Module::PS)) { continue; } break;
                case kStudyEndcapIneff2S: if (not (lowerModulePtr_Track->subdet() == SDL::Module::Endcap and lowerModulePtr_Track->moduleType() == SDL::Module::TwoS)) { continue; } break;
                case kStudyEndcapIneffPSLowP: if (not (lowerModulePtr_Track->subdet() == SDL::Module::Endcap and lowerModulePtr_Track->moduleType() == SDL::Module::PS and lowerModulePtr_Track->moduleLayerType() == SDL::Module::Pixel)) { continue; } break;
                case kStudyEndcapIneffPSLowS: if (not (lowerModulePtr_Track->subdet() == SDL::Module::Endcap and lowerModulePtr_Track->moduleType() == SDL::Module::PS and lowerModulePtr_Track->moduleLayerType() == SDL::Module::Strip)) { continue; } break;
                default: /* skip everything should not be here anyways...*/ continue; break;
            }

            // Loop over the md "candidate" from the module that a sim-track passed through and left at least one hit in each module
            for (auto& md_Track : lowerModulePtr_Track->getMiniDoubletPtrs())
            {

                SDL::Module& lowerModule = *lowerModulePtr_Track;
                SDL::Hit& lowerHit = *(md_Track->lowerHitPtr());
                SDL::Hit& upperHit = *(md_Track->upperHitPtr());

                // These are the individual component for mini-doublet calculation
                // Copied from SDL::MiniDoublet code
                float z = fabs(lowerHit.z());
                float dz = std::abs(lowerHit.z() - upperHit.z());
                float drt = lowerHit.rt() - upperHit.rt();
                float fabsdPhi = (lowerModule.moduleType() == SDL::Module::PS) ?
                    SDL::MiniDoublet::fabsdPhiPixelShift(lowerHit, upperHit, lowerModule) : std::abs(lowerHit.deltaPhi(upperHit));
                float dzfrac = dz / fabs(lowerHit.z());
                float fabsdPhiMod = fabsdPhi / dzfrac * (1.f + dzfrac);
                float miniCut = SDL::MiniDoublet::dPhiThreshold(lowerHit, lowerModule);
                float fabsdPhiModDiff = fabsdPhiMod - miniCut;

                // Actually use the static function to perform the calculation
                if (SDL::MiniDoublet::isMiniDoubletPair(lowerHit, upperHit, lowerModule, SDL::Default_MDAlgo, SDL::Log_Nothing))
                {
                    // Passed
                    dzs_passed.push_back(dz);
                    drts_passed.push_back(drt);
                    fabsdPhis_passed.push_back(fabsdPhi);
                    zs_passed.push_back(z);
                    dzfracs_passed.push_back(dzfrac);
                    fabsdPhiMods_passed.push_back(fabsdPhiMod);
                    fabsdPhiModDiffs_passed.push_back(fabsdPhiModDiff);

                }
                else
                {
                    // Failed
                    dzs_failed.push_back(dz);
                    drts_failed.push_back(drt);
                    fabsdPhis_failed.push_back(fabsdPhi);
                    zs_failed.push_back(z);
                    dzfracs_failed.push_back(dzfrac);
                    fabsdPhiMods_failed.push_back(fabsdPhiMod);
                    fabsdPhiModDiffs_failed.push_back(fabsdPhiModDiff);

                }

            }

        }

    }

}
