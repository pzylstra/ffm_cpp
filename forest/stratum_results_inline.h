#ifndef STRATUM_RESULTS_INLINE_H
#define STRATUM_RESULTS_INLINE_H

//constructors

/*!\brief Default constructor
 */
inline StratumResults::StratumResults() {}

/*!\brief Constructor
  \param lev


  Constructs empty StratumResults object  with a given Stratum::LevelType
*/
inline StratumResults::StratumResults(const Stratum::LevelType& lev) : level_(lev) {}

//accessors

/*!\brief The level of the Stratum to which the results apply
  \return The Stratum::LevelType of the Stratum
*/
inline Stratum::LevelType StratumResults::level() const {return level_;}

/*!\brief Rate of spread
  \return Rate of spread of the Stratum fire (m/s)
*/
inline double StratumResults::ros() const {return ros_;}

/*!\brief Flame tip height
*/
inline double StratumResults::flameTipHeight() const {return flameTipHeight_;}

/*!\brief Flame origin height
*/
inline double StratumResults::flameOriginHeight() const {return flameOriginHeight_;}

/*!\brief Flame length
  \return Flame length of the Stratum fire (m)
*/
inline double StratumResults::flameLength() const {return flameLength_;}

/*!\brief Flame angle
  \return Flame angle of the Stratum fire (radians)
*/
inline double StratumResults::flameAngle() const {return flameAngle_;}

/*!\brief Proportion burnt
  \return The proportion of the Stratum burnt
*/
inline double StratumResults::proportionBurnt() const {return proportionBurnt_;}

inline std::map<std::string, double> StratumResults::speciesFlameTipHeights() const { return speciesFlameTipHeights_; }


//mutators

/*!\brief Set Stratum fire rate of spread
  \param ros Rate of spread (m/s)
*/
inline void StratumResults::ros(const double& ros) {ros_ = ros;}


inline void StratumResults::flameTipHeight(const double& flame_height) {flameTipHeight_ = flame_height;}

inline void StratumResults::flameOriginHeight(const double& flame_height) {flameOriginHeight_ = flame_height;}

/*!\brief Set Stratum fire flame length
  \param flame_length (m)
*/
inline void StratumResults::flameLength(const double& flame_length) {flameLength_ = flame_length;}

/*!\brief Set Stratum fire flame angle
  \param flame_angle (radians)
*/
inline void StratumResults::flameAngle(const double& flame_angle) {flameAngle_ = flame_angle;}

/*!\brief Set proportion of Stratum burnt
  \param proportion_burnt
*/
inline void StratumResults::proportionBurnt(const double& proportion_burnt) {proportionBurnt_ = proportion_burnt;}

inline void StratumResults::addSpeciesFlameTipHeight(const std::string& name, const double& ht) { speciesFlameTipHeights_[name] = ht; }

//printing and conversion

/*!\brief Printing
  \return Formatted string describing rate of spread in km/h
*/
inline std::string StratumResults::printROS() const {
  char s[30];
  sprintf(s, "%6.2f", ros_*3.6);
  return std::string(s);
}

/*!\brief Printing
  \return Formatted string describing flame tip height in metres
*/
inline std::string StratumResults::printFlameTipHeight() const {
  char s[30];
  sprintf(s, "%5.1f", flameTipHeight_);
  return std::string(s);
}

/*!\brief Printing
  \return Formatted string describing flame origin height in metres
*/
inline std::string StratumResults::printFlameOriginHeight() const {
  char s[30];
  sprintf(s, "%5.1f", flameOriginHeight_);
  return std::string(s);
}

/*!\brief Printing
  \return Formatted string describing flame length in metres
*/
inline std::string StratumResults::printFlameLength() const {
  char s[30];
  sprintf(s, "%5.1f", flameLength_);
  return std::string(s);
}

/*!\brief Printing
  \return Formatted string describing flame angle in degrees
*/
inline std::string StratumResults::printFlameAngle() const {
  char s[30];
  sprintf(s, "%5.1f", flameAngle_*180/PI);
  return std::string(s);
}

/*!\brief Printing
  \return Formatted string describing proportion of Stratum burnt
*/
inline std::string StratumResults::printProportionBurnt() const {
  char s[30];
  sprintf(s, "%4.0f", proportionBurnt_*100);
  return std::string(s);
}


#endif //STRATUM_RESULTS_INLINE_H
