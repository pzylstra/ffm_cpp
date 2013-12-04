#ifndef FLAME_SERIES_H
#define FLAME_SERIES_H

#include "flame.h"
#include "stratum.h"

///\brief A FlameSeries is a timeseries of Flame objects associated with a particular Stratum
class FlameSeries{
public:
  //constructors

  FlameSeries();
  FlameSeries(const Stratum::LevelType& level);
  FlameSeries(const Stratum::LevelType& level, const std::vector<Flame>& flames);

  //accessors

  Stratum::LevelType level() const;
  Flame flame(const unsigned& i) const;
  std::vector<Flame> flames() const;

  //the following four methods return characteristics of the 
  //i'th flame, counting from i = 0

  double flameLength(const unsigned& i) const;
  double depthIgnited(const unsigned& i) const;
  double deltaTemperature(const unsigned& i) const;
  Pt origin(const unsigned& i) const;

  //mutators

  void addFlame(const Flame& newFlame);

  //other methods
  bool empty() const;
  bool isNull() const;
  unsigned size() const;
  FlameSeries combine(const FlameSeries& otherSeries,
		      const double& windSpeed, 
		      const double& slope, 
		      const double& fireLineLength) const; 
  FlameSeries combine(const FlameSeries& otherSeries) const; 

  //stats

  int nonNullCount() const;
  double maxFlameLength() const;
  double cappedMaxFlameLength() const;
  double meanFlameLength() const;
  double stdDevFlameLength() const;
  double meanAngle() const;
  Pt meanOrigin() const;
  double meanDepthIgnited() const;
  double meanDeltaTemperature() const;

  //printing
  std::string printToString() const;

private:
  Stratum::LevelType level_;
  std::vector<Flame> flames_;

}; //FlameSeries

#include "flame_series_inline.h"

#endif //FLAME_SERIES_H
