#ifndef PRE_HEATING_FLAME_H
#define PRE_HEATING_FLAME_H

/*!\brief Stores and manipulates data relating to pre-heating.

  A PreHeatingFlame object is a Flame object with a start time, an end time and a
  level.
*/
class PreHeatingFlame{
public:
  //constructors

  PreHeatingFlame();
  PreHeatingFlame(const Stratum::LevelType& level,const Flame& flame, 
		  const double& startTime, const double& endTime);

  //accessors

  Stratum::LevelType level() const;
  Flame flame() const;
  double startTime() const;
  double endTime() const;

  //other methods

  double duration() const;
  double duration(const double& end) const;

private:
  Stratum::LevelType level_;
  Flame flame_;
  double startTime_;
  double endTime_;
};

#include "pre_heating_flame_inline.h"

#endif //PRE_HEATING_FLAME_H
