#ifndef FOREST_IGNITION_RUN_H
#define FOREST_IGNITION_RUN_H

#include "stratum.h"
#include "forest.h"
#include "flame.h"
#include "ignition_path.h"
//#include "results.h"

class Results;

/*!\brief Represents a complete ignition scenario at a forest location. 

  A ForestIgnitionRun contains the full set of IgnitionPath objects produced by the computation. These
  completely describe the dynamics of the progression of the fire through the species and the strata. In
  addition to the ignition paths, a ForestIgnitionRun object contains the combined flames produced by the
  ignition paths in the strata of the forest.
*/
class ForestIgnitionRun {

public:

  /*!\brief Indicates whether the Stratum::CANOPY level was included when determining
    the wind field used for the computation.

    The value WITHOUT_CANOPY indicates that the wind field was computed as if the canopy was not 
    there. In all other respects the computation is the same
  */
  enum RunType{UNKNOWN_RUN_TYPE, WITH_CANOPY, WITHOUT_CANOPY};

  //constructors

  ForestIgnitionRun();
  ForestIgnitionRun(const Forest& forest);
  ForestIgnitionRun(const RunType& runType, const Forest& forest);

  //accessors

  Forest forest() const;
  RunType type() const;
  std::vector<IgnitionPath> paths() const;
  std::vector<Flame> combinedFlames() const;

  //mutators
  void type(const RunType& runType);
  void addPath(const IgnitionPath& ignitionPath);
  void combinedFlames(const std::vector<Flame>& flames);

  //other methods
  bool spreadsInStratum(const Stratum::LevelType& level) const;
  std::vector<double> speciesWeightedFlameLengths(const Stratum::LevelType& level, 
						  const IgnitionPath::PathType& ptype) const;
  std::vector<double> laterallyMergedSpeciesWeightedPlantFlameLengths(const Stratum::LevelType& level, 
								 const double& firelineLength) const;
  std::vector<IgnitionPath>::const_iterator beginPaths() const;
  std::vector<IgnitionPath>::const_iterator endPaths() const;
  std::vector<Flame>::const_iterator beginCombinedFlames() const;
  std::vector<Flame>::const_iterator endCombinedFlames() const;
  double speciesWeightedBasicROS(const Stratum::LevelType& level) const;
  double speciesWeightedIgnitionTimeStep(const Stratum::LevelType& level,
					 const IgnitionPath::PathType pathType) const;
  double speciesWeightedTimeStepsIgnitionToMaxFlame(const Stratum::LevelType& level, 
						    const IgnitionPath::PathType pathType) const;
  double activeCrownFireROS(const Results& theResults) const;
  double speciesWeightedMaxHeightBurnt(const Stratum::LevelType& level) const;
  Pt speciesWeightedOriginOfMaxFlame(const Stratum::LevelType& level, 
				     const IgnitionPath::PathType& pathType) const;
  std::string printToString() const;

private:
  RunType type_ = UNKNOWN_RUN_TYPE;
  Forest forest_ = Forest();
  std::vector<IgnitionPath> paths_ = std::vector<IgnitionPath>();
  std::vector<Flame> combinedFlames_ = std::vector<Flame>();
};

//map for printing
const std::map<ForestIgnitionRun::RunType, std::string> runTypeStringMap = 
  {{ForestIgnitionRun::UNKNOWN_RUN_TYPE, "Unknown run type"},
   {ForestIgnitionRun::WITH_CANOPY, "With canopy"},
   {ForestIgnitionRun::WITHOUT_CANOPY, " Without canopy"}};


#include "forest_ignition_run_inline.h"

#endif //FOREST_IGNITION_RUN_H
