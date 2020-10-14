#ifndef trkCore_h
#define trkCore_h

#include "trktree.h"
#include "TCanvas.h"
#include "AnalysisInterface/EventForAnalysisInterface.h"
#include "AnalysisInterface/Module.h"
#include "AnalysisConfig.h"
#include "SDL/ModuleConnectionMap.h"

enum TrackletType {
    BB1BB3 = 0,
    BB2BB4,
    BB3BB5,
    BB1BE3,
    BB2BE4,
    BB3BE5,
    BB1EE3,
    BB2EE4,
    BB3EE5,
    BE1EE3,
    BE2EE4,
    BE3EE5,
    EE1EE3,
    EE2EE4,
};

float simhit_p(unsigned int simhitidx);
float hitAngle(unsigned int simhitidx);
bool isMuonCurlingHit(unsigned int isimtrk, unsigned int ith_hit);
bool hasAll12HitsWithNBarrel(unsigned int isimtrk, int nbarrel);
bool hasAll12HitsWithNBarrelUsingModuleMap(unsigned int isimtrk, int nbarrel, bool usesimhits=false);
bool checkModuleConnectionsAreGood(std::array<std::vector<unsigned int>, 6>& layers_good_paired_modules);
bool goodEvent();
void addOUterTrackerHits(SDL::Event& event);
void runMinidoublet(SDL::Event& event);
void runSegment(SDL::Event& event);
void runTracklet(SDL::Event& event);
std::vector<float> getPtBounds();
bool inTimeTrackWithPdgId(int isimtrk, int pdgid);
std::vector<int> matchedSimTrkIdxs(std::vector<int> hitidxs, std::vector<int> hittypes);
bool isMTVMatch(unsigned int isimtrk, std::vector<unsigned int> hit_idxs, bool verbose);
#endif
