#ifndef IGNITIONPATH_H
#define IGNITIONPATH_H

#include "seg.h"
#include "stratum.h"
#include "flame_series.h"

class Species;
class Flame;

/*!\brief Represents the path of ignition through a Species crown or a Stratum.

  An ignition path through the crown of a species or through a 
  stratum is a timeseries of
  ignited segments. An IgnitionPath object contains and 
  manipulates data of this type.
*/
class IgnitionPath{

public:

  /*!\brief Indicates whether the path is through a Species crown or a stratum type crown.

    Note that according to the model if the path is through a stratum type crown 
    it is still considered to be burning in one Species.
  */ 
  enum PathType{UNKNOWN_PATH, PLANT_PATH, STRATUM_PATH};

  //constructors

  IgnitionPath();
  IgnitionPath(const PathType& pathType, const Stratum::LevelType& level, const Species& species, 
	       const int& startTimeStep);

  //accessors 

  PathType type() const;
  Stratum::LevelType level() const;
  Species species() const;
  int startTimeStep() const;
  std::vector<Seg> ignitedSegments() const;
  Seg ignitedSegment(const int& i) const;
  
  //mutators
  void startTimeStep(const int& startTimeStep);
  void addSegment(const Seg& seg);

  //other methods

  int size() const;
  bool empty() const;
  bool fullSize() const;
  bool spreads() const;
  void sortSegments();
  double maxSegmentLength() const;
  int indexOfMaxSegment() const;
  Pt originOfMaxSegment() const;
  double maxHeightBurnt(const double& slope) const; 
  double maxFlameLength() const;
  double maxX() const;
  double maxHorizontalRun() const;
  double timeToIgnition() const;  //seconds
  int timeStepsIgnitionToMaxFlame() const;  
  double timeIgnitionToMaxFlame() const;  //seconds
  double meanFlameLength(const bool& ignoreZeros = true) const;
  double stdDevFlameLength(const bool& ignoreZeros = true) const;
  double flameLength() const;
  double flameLength(const int& i) const;
  double ignitedLength() const;
  double ignitedLength(const int& i) const;
  Pt origin() const;
  Pt origin(const int& i) const;
  Flame flame(const double& windSpeed, const double& slope) const;
  Flame flame(const int& i, const double& windSpeed, const double& slope) const;
  FlameSeries flameSeries(const double& windSpeed, const double& slope) const;
  double ros(const int& i) const;
  double basicROS() const;
  double nonIndependentROS() const;
  double timeOfSpread() const;
  std::string printToString() const;

private:

  PathType type_;
  Stratum::LevelType level_;
  Species species_;
  int startTimeStep_;
  std::vector<Seg> ignitedSegments_;

};

//map for names of path types (used for printing)
const std::map<IgnitionPath::PathType, std::string> ignitionPathTypeStringMap = 
  {{IgnitionPath::UNKNOWN_PATH, "Unknown"},
   {IgnitionPath::PLANT_PATH, "Plant path"},
   {IgnitionPath::STRATUM_PATH, "Stratum path"}};

#include "ignition_path_inline.h"

#endif //IGNITIONPATH_H
