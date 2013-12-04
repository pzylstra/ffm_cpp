#ifndef WEATHER_INLINE_H
#define WEATHER_INLINE_H

//constructors

/*!\brief Default constructor*/
inline Weather::Weather() : airTempC_(-99) {} 

/*!\brief Standard constructor*/
inline Weather::Weather(const double& temperature) : airTempC_(temperature) {}


//accessors

/*!\brief Air temperature
\return Air temperature (C)
*/
inline double Weather::airTempC() const {return airTempC_;}

//printing

/*!\brief Printing
  \return A formatted string describing the Weather object
*/
inline std::string Weather::printToString() const {
  std::string str;
  str = "Air temperature (C): ";
  str += printAirTempC() + "\n";
  return str;
}

/*!\brief Printing
  \return A formatted string describing the air temperature (C)
*/
inline std::string Weather::printAirTempC() const {
  char s[10];
  sprintf(s,"%.1f",airTempC_);
  return std::string(s);
}

#endif
