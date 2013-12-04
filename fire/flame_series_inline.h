#ifndef FLAME_SERIES_INLINE_H
#define FLAME_SERIES_INLINE_H

#include <algorithm>

//constructors

/*!\brief Default constructor 

  Produces an empty series of flames within a Stratum::UNKNOWN_LEVEL stratum.
*/
inline FlameSeries::FlameSeries() : level_(Stratum::UNKNOWN_LEVEL), flames_() {
  flames_.reserve(static_cast<int>(ceil(ffm_settings::maxTime/ffm_settings::computationTimeInterval + 0.5)));
}

/*!\brief Constructor
  \param lev

    
  Produces an empty series of flames within the lev Stratum.
*/
inline FlameSeries::FlameSeries(const Stratum::LevelType& lev) :  level_(lev),flames_() {
  flames_.reserve(static_cast<int>(ceil(ffm_settings::maxTime/ffm_settings::computationTimeInterval + 0.5)));
}

/*!\brief Standard constructor
  \param lev
  \param flames


  Produces a FlameSeries in the Stratum at  Stratum::LevelType& lev, containing
  the vector flames of Flame objects.
*/
inline  FlameSeries::FlameSeries(const Stratum::LevelType& lev, const std::vector<Flame>& flames) 
  : level_(lev), flames_(flames) {
  flames_.reserve(static_cast<int>(ceil(ffm_settings::maxTime/ffm_settings::computationTimeInterval + 0.5)));
}

//accessors

/*!\brief The stratum in which the flame series exists
  \return  Stratum::LevelType of the stratum in which the flame series exists.
*/
inline Stratum::LevelType  FlameSeries::level() const {return level_;}

/*!\brief Vector of flames
  \return The vector of Flame objects comprising the FlameSeries.
*/
inline std::vector<Flame> FlameSeries::flames() const {return flames_;}

/*!\brief Extract constituent flames
  \param i
  \return The i'th Flame of the FlameSeries (counting from i = 0).
*/
inline Flame FlameSeries::flame(const unsigned& i) const {
  return flames_.at(i);
}

/*!\brief Extract flame length
  \param i
  \return The flame length (m) of the i'th flame, counting from i = 0.
*/
inline double FlameSeries::flameLength(const unsigned& i) const {
  return flames_.at(i).flameLength();
}

/*!\brief Extract depth of ignition
  \param i
  \return Depth of ignition (m) of the i'th flame (counting from i = 0).
*/
inline double FlameSeries::depthIgnited(const unsigned& i) const {
  return flames_.at(i).depthIgnited();
}

/*!\brief Extract temperature. 
  \param i
  \return Temperature (C) above ambient of the i'th flame (counting from i = 0).
*/
inline double FlameSeries::deltaTemperature(const unsigned& i) const {
  return flames_.at(i).deltaTemperature();
}

/*!\brief Extract flame origin
  \param i
  \return Origin (Pt) of the i'th flame (counting from i = 0).
*/
inline Pt FlameSeries::origin(const unsigned& i) const {
  return flames_.at(i).origin();
}

//mutators

/*!\brief Adds a Flame to the end of the FlameSeries
  \param newFlame
*/
inline void FlameSeries::addFlame(const Flame& newFlame) {flames_.push_back(newFlame);}

//other methods

/*!\brief Check for empty FlameSeries
  \return true if and only if the size of the FlameSeries is 0.
*/
inline bool FlameSeries::empty() const {
  return flames_.empty();
}

/*!\brief Check for null FlameSeries
  \return true if and only if no non-null flames are contained in the FlameSeries.
*/
inline bool FlameSeries::isNull() const {
  if (empty()) return true;
  auto iter = find_if_not(flames_.begin(), flames_.end(), [](Flame f){return f.isNull();});
  return iter == flames_.end();
}

/*!\brief Size of the FlameSeries
  \return The number of flames in the FlameSeries, including null flames.
*/
inline unsigned FlameSeries::size() const {
  return flames_.size();
}

/*!\brief Combine two FlameSeries
  \param otherSeries
  \param windSpeed
  \param slope
  \param fireLineLength
  \return Result of combining *this flame by flame with otherSeries, using flame 
  angle models to compute the angle of the combined flames.
*/
inline FlameSeries FlameSeries::combine(const FlameSeries& otherSeries,
		    const double& windSpeed, 
		    const double& slope, 
		    const double& fireLineLength) const { 
  if (level_ != otherSeries.level() || otherSeries.empty()) return *this;
  return FlameSeries(level_, combineFlames(flames_,otherSeries.flames(),
					   windSpeed,slope,fireLineLength));
}


/*!\brief Combine two FlameSeries
  \param otherSeries
  \return Result of combining *this flame by flame with otherSeries. Flame angles of 
  the combined flames are flame length weighted averages of the constituent flames.
*/
inline FlameSeries FlameSeries::combine(const FlameSeries& otherSeries) const {
  if (level_ != otherSeries.level() || otherSeries.empty()) return *this;
  return FlameSeries(level_, combineFlames(flames_,otherSeries.flames()));
}

//stats

/*!\brief Count non-null flames
  \return Number of non-null flames
*/
inline int FlameSeries::nonNullCount() const {
  return ::nonNullCount(flames_);
}

/*!\brief Maximum flame length of the FlameSeries
  \return Maximum flame length (m)
*/
inline double FlameSeries::maxFlameLength() const {
  return ::maxFlameLength(flames_);
}

/*!\brief Capped maximum flame length of the FlameSeries 
  \return Maximum flame length (m) of the flame series capped by the mean + 1 standard deviation.
  Null flames are ignored in computing the mean and standard deviation.
*/
inline double FlameSeries::cappedMaxFlameLength() const {
  return ::cappedMaxFlameLength(flames_);
}

/*!\brief Mean flame length
  \return Mean flame length (m) of the FlameSeries. Null flames are ignored in computing the mean.
*/
inline double FlameSeries::meanFlameLength() const {
  return ::meanFlameLength(flames_);
}

/*!\brief Standard deviation of flame length
  \return Standard deviation of flame length (m) of the FlameSeries. 
  Null flames are ignored in computing the mean.
*/
inline double FlameSeries::stdDevFlameLength() const {
  return ::stdDevFlameLength(flames_);
}

/*!\brief Mean flame angle
  \return Mean flame angle (rad) of the FlameSeries. Null flames are ignored in computing the mean.
*/
inline double FlameSeries::meanAngle() const {
  return ::meanAngle(flames_);
}

/*!\brief Mean flame origin
  \return Mean flame origin (Pt) of the FlameSeries. Null flames are ignored in computing the mean.
*/
inline Pt FlameSeries::meanOrigin() const {
  return ::meanOrigin(flames_);
}

/*!\brief Mean depth of ignition
  \return Mean depth of ignition (m) (length of Zone I) of the FlameSeries. 
  Null flames are ignored in computing the mean.
*/
inline double FlameSeries::meanDepthIgnited() const {
  return ::meanDepthIgnited(flames_);
}

/*!\brief Mean temperature above ambient
  \return Mean temperature (C) above ambient of the FlameSeries. Null flames are ignored in computing the mean.
*/
inline double FlameSeries::meanDeltaTemperature() const {
  return ::meanDeltaTemperature(flames_);
}

/*!\brief Printing
  \return A formatted string describing the FlameSeries.
*/
inline std::string FlameSeries::printToString() const {
  std::string str;
  char s[40];
  str = "Level: " + levelStringMap.at(level_) + "\n";
  int cnt = 1;
  for (Flame fl : flames_){
    str += sprintf(s, "%d ", cnt);
    str += fl.printToString();
    str += "\n";
  }
  return str;
}



#endif //FLAME_SERIES_INLINE_H
