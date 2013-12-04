#ifndef LOCATION_H
#define LOCATION_H

#include <string>

#include "ffm_numerics.h"
#include "forest.h"
#include "weather.h"
#include "ignition_path.h"
#include "forest_ignition_run.h"
#include "results.h"

class Flame;
class FlameSeries;
class PreHeatingFlame;

/*!\brief Objects of the Location class hold all the data necessary to run the model 
  at a single spatial location.

  A Location object contains a Forest object, a 
  Weather object and other necessary input data which at the moment are incident 
  wind speed and fireline length. The class also has methods which perform the 
  main fire computation.
*/
class Location{
public:

  //constructors

  Location();
  Location(const Forest& forest, const Weather& weather, 
	   const double& incidentWindSpeed, const double& firelineLength);

  //accessors for members

  Forest forest() const;
  Weather weather() const;
  double incidentWindSpeed() const;
  double firelineLength() const; 

  //accessors from data members

  double slope() const;
  std::vector<Stratum> strata() const;

  //printing

  std::string printToString() const;
  std::string printWindSpeed() const;
  std::string printFirelineLength() const;

  //other methods
  bool empty() const;

  //main fire computations
  Results results() const;

private:

  Forest forest_ ;
  Weather weather_ ;
  double incidentWindSpeed_;
  double firelineLength_;

  ForestIgnitionRun forestIgnitionRun(const bool& withCanopy = true) const;
  //computes all ignition paths in a forest

  IgnitionPath computeIgnitionPath(const std::vector<Flame>& incidentFlames,
				   const bool& plantFlameRun,
				   std::vector<PreHeatingFlame> preHeatingFlames,
				   const double& preHeatingEndTime,
				   const Stratum::LevelType& level,
				   const Species& spec, 
				   const double& canopyHeatingDistance,
				   const double& windSpeed,
				   const Pt& initialPt) const;
//computes ignition path in a species or stratum fire
};

#include "location_inline.h"

#endif //LOCATION_H
