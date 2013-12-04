#ifndef WEATHER_H
#define WEATHER_H

#include <string>

/*!\brief Holds weather data. 

  At the moment the only such data held is air temperature
*/
class Weather {


 public:

  //constructors

  Weather();
  Weather(const double& airTempC);

  //accessors

  double airTempC() const;

  //other methods

  std::string printToString() const;
  std::string printAirTempC() const;

private:

  double airTempC_ ;
};

#include "weather_inline.h"

#endif
