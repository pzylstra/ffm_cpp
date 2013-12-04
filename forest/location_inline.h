#ifndef LOCATION_INLINE_H
#define LOCATION_INLINE_H

#include "ffm_settings.h"

//constructors

/*!\brief Default constructor.*/
inline Location::Location() : forest_(), weather_(), incidentWindSpeed_(-999), firelineLength_(-999){}

/*!\brief Standard constructor. 
  \param forest
  \param weather
  \param incidentWindSpeed (m/s)
  \param firelineLength (m)
*/
inline Location::Location(const Forest& forest, 
			  const Weather& weather, 
			  const double& incidentWindSpeed, 
			  const double& firelineLength) :
  forest_(forest), weather_(weather), incidentWindSpeed_(incidentWindSpeed), firelineLength_(firelineLength) {};

//accessors for member functions

/*!\brief The Forest object
  \return The Forest object associated with the Location.
*/
inline Forest Location::forest() const {return forest_;};

/*!\brief The Weather object
  \return The Weather object associated with the Location.
*/
inline Weather Location::weather() const {return weather_;}

/*!\brief Wind speed
  \return The incident wind speed (m/s).
*/
inline double Location::incidentWindSpeed() const {return incidentWindSpeed_;}

/*!\brief Length of the fireline
  \return Length of the fireline (m).
*/
inline double Location::firelineLength() const {return firelineLength_;}

//accessors from data members

/*!\brief Slope of the surface
  \return  forest().surface().slope() (Radians).
*/
inline double Location::slope() const {return forest_.surface().slope();}

/*!\brief The strata of the Forest
  \return forest().strata()
*/
inline std::vector<Stratum> Location::strata() const {return forest_.strata();}

//other methods

/*!\brief Detect empty Location
  \return true if and only if the Forest object associated with the Location has no Stratum objects.
*/
inline bool Location::empty() const {
  return forest_.empty();
}

/*!\brief Printing
  \return A formatted string describing the Location.
*/
inline std::string Location::printToString() const {
  std::string str;
  str = "Location input data\n\n";
  str += "Miscellaneous:\n\n";
  str += "Incident wind speed (km/h): " + printWindSpeed() + "\n";
  str += "Fireline length (m)): " + printFirelineLength() + "\n";
  str += "\nWeather:\n\n" + weather_.printToString();
  str += "\nForest:\n\n" + forest_.printToString();
  return str;
}

//conversion and printing of data members
/*!\brief Conversion and printing
  \return A formatted string describing wind speed in km/h.
*/
inline std::string Location::printWindSpeed() const {
  //converts to km/h
  char s[10];
  sprintf(s,"%.1f",incidentWindSpeed_*3.6);
  return std::string(s);
}
  
/*!\brief Printing
  \return A formatted string describing length of the fireline (m).
*/
inline std::string Location::printFirelineLength() const {
  char s[10];
  sprintf(s,"%.0f",firelineLength_);
  return std::string(s);
}



#endif //LOCATION_INLINE_H
