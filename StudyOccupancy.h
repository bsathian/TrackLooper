#ifndef StudyOccupancy_h
#define StudyOccupancy_h

# include "SDL/Event.h"
# include "Study.h"
# include <vector>
# include <tuple>

# include "TString.h"
# include "trktree.h"
# include "constants.h"

# include "AnalysisConfig.h"

class StudyOccupancy : public Study
{
public:
  const char * studyname;

  float averageOccupancyInBarrel;
  float averageOccupancyInEndcap;
  float averageLayerOccupancy[6];
  float averageBarrelLayerOccupancy[6];
  float averageEndcapLayerOccupancy[6];

  StudyOccupancy(const char * studyName);
  virtual void bookStudy();
  virtual void doStudy(SDL::Event& event, std::vector<std::tuple<unsigned int, SDL::Event*>> simtrkevents);
};

#endif
