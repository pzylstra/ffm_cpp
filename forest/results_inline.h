#ifndef RESULTS_INLINE_H
#define RESULTS_INLINE_H

//constructors
  /*!\brief Default constructor 
    
    Constructs an empty Results object
  */
inline Results::Results () {}

//accessors

  /*!\brief Rate of spread 
    \return The overall rate of spread (m/s)
  */
inline double Results::ros() const {return ros_;}

  /*!\brief Overall flame tip height
    \return The overall flame tip height (m)
  */
inline double Results::flameTipHeight() const {return flameTipHeight_;}

  /*!\brief Overall flame origin height
    \return The overall flame origin height (m)
  */
inline double Results::flameOriginHeight() const {return flameOriginHeight_;}

  /*!\brief Overall flame length
    \return The overall flame length (m)
  */
inline double Results::flameLength() const {return flameLength_;}

  /*!\brief Overall flame angle
    \return The overall flame angle (rad)
  */
inline double Results::flameAngle() const {return flameAngle_;}

  /*!\brief Surface fire rate of spread
    \return The surface fire rate of spread (m/s)
  */
inline double Results::surfaceROS() const {return surfaceROS_;}

  /*!\brief Surface fire flame height
    \return The surface fire flame height (m)
  */
inline double Results::surfaceFlameHeight() const {return surfaceFlameHeight_;}

  /*!\brief Surface fire flame length
    \return The surface fire flame length (m)
  */
inline double Results::surfaceFlameLength() const {return surfaceFlameLength_;}

  /*!\brief Surface fire flame angle
    \return The surface fire flame angle (rad)
  */
inline double Results::surfaceFlameAngle() const {return surfaceFlameAngle_;}

  /*!\brief Overall flame depth
    \return The overall flame depth (m)
  */
inline double Results::flameDepth() const {return flameDepth_;}

  /*!\brief Type of crown fire
    \return Results::CrownFireType
  */
inline Results::CrownFireType Results::crownFireType() const {return crownFireType_;}

  /*!\brief Crown run length
    \return crown run length (m)
  */
inline double Results::crownRunLength() const {return crownRunLength_;}

  /*!\brief Crown run velocity
    \return The velocity of crown runs (m/s)
  */
inline double Results::crownRunVelocity() const {return crownRunVelocity_;}

  /*!\brief Wind reduction factor
    \return The wind reduction factor

    The wind reduction factor is the ratio of the incident wind speed to 
    the wind speed computed at 1.5 metres above the ground
  */
inline double Results::windReductionFactor() const {return windReductionFactor_;}

  /*!\brief McArthur scorch height model
    \return The value of the McArthur scorch height model (m)
  */
inline double Results::scorchHeightMcarthur() const {return scorchHeightMcarthur_;}

  /*!\brief Luke-McArthur scorch height model
    \return The value of the Luke McArthur scorch height model (m)
  */
inline double Results::scorchHeightLukeMcarthur() const {return scorchHeightLukeMcarthur_;}

  /*!\brief van Wagner scorch height model
    \return The value of the van Wagner scorch height model (m)
  */
inline double Results::scorchHeightVanWagner() const {return scorchHeightVanWagner_;}

  /*!\brief van Wagner scorch height model with wind
    \return The value of the McArthur scorch height model with wind (m)
  */
inline double Results::scorchHeightVanWagnerWithWind() const {return scorchHeightVanWagnerWithWind_;}

  /*!\brief Accesses the vector of stratum specific results
    \return The vector of StratumResults
  */
inline std::vector<StratumResults> Results::strataResults() const {return strataResults_;}

  /*!\brief The ForestIgnitionRun objects
    \return The vector of ForestIgnitionRun objects computed by the model

    This vector has at least one and at most two elements. The second element, if 
    it exists, is the ForestIgnitionRun object produced using the windfield computed
    as if the canopy Stratum did not exist
  */
inline std::vector<ForestIgnitionRun> Results::runs() const {return runs_;}

  /*!\brief Was a second run done?
    \return true if and only if a second run was done

    This refers to the second run done with the wind field 
    computed as if the canopy stratum was missing
  */
inline bool Results::runTwoExists() const {return runs_.size() == 2;}

//mutators

  /*!\brief Sets overall rate of spread
    \param r (double, m/s)
    
    Sets overall rate of spread to r
  */
inline void Results::ros(const double& r) {ros_ = r;}

  /*!\brief Sets overall flame tip height
    \param fh (double, m)

    Sets overall flame tip height to fh
  */
inline void Results::flameTipHeight(const double& fh) {flameTipHeight_ = fh;}

  /*!\brief Sets overall flame origin height
    \param fh (double, m)

    Sets overall flame origin height to fh
  */
inline void Results::flameOriginHeight(const double& fh) {flameOriginHeight_ = fh;}

  /*!\brief Sets overall flame length
    \param fl (double, m)

    Sets overall flame length to fl
  */
inline void Results::flameLength(const double& fl) {flameLength_ = fl;}

  /*!\brief Sets overall flame angle
    \param fa (double, rad)

    Sets overall flame angle to fa
  */
inline void Results::flameAngle(const double& fa) {flameAngle_ = fa;}


  /*!\brief Sets flame depth 
    \param fd (double, m)

    Sets flame depth to fd
  */
inline void Results::flameDepth(const double& fd) {flameDepth_ = fd;}

  /*!\brief Sets surface rate of spread
    \param r (double, m/s)

    Sets surface rate of spread to r
  */
inline void Results::surfaceROS(const double& r) {surfaceROS_ = r;}

  /*!\brief Sets surface flame height
    \param fh (double, m)

    Sets surface flame height to fh
  */
inline void Results::surfaceFlameHeight(const double& fh) {surfaceFlameHeight_ = fh;}

  /*!\brief Sets surface flame length
    \param fl (double, m)

    Sets surface flame length to fl
  */
inline void Results::surfaceFlameLength(const double& fl) {surfaceFlameLength_ = fl;}
 
  /*!\brief Sets surface flame angle
    \param fa (double, rad)

    Sets surface flame angle to fa
  */
inline void Results::surfaceFlameAngle(const double& fa) {surfaceFlameAngle_ = fa;}

  /*!\brief Sets crown fire type
    \param cft (Results::CrownFireType)

    Sets crown fire type to cft
  */
inline void Results::crownFireType(const CrownFireType& cft) {crownFireType_ = cft;}

  /*!\brief Sets crown run length
    \param crl (double, m)

    Sets crown run length to crl
  */
inline void Results::crownRunLength(const double& crl) {crownRunLength_ = crl;}

  /*!\brief Sets crown run velocity
    \param crv (double, m/s)

    Sets crown run velocity to crv
  */
inline void Results::crownRunVelocity(const double& crv) {crownRunVelocity_ = crv;}

  /*!\brief Sets wind reduction factor
    \param wrf (double)

    Sets wind reduction factor to wrf
  */
inline void Results::windReductionFactor(const double& wrf) {windReductionFactor_ = wrf;}

  /*!\brief Sets McArthur scorch height
    \param ht (double, m)

    Sets McArthur scorch height to ht
  */
inline void Results::scorchHeightMcarthur(const double& ht) {
  scorchHeightMcarthur_ = ht;}

  /*!\brief Sets Luke-McArthur scorch height
    \param ht (double, m)

    Sets Luke-McArthur scorch height to ht
  */
inline void Results::scorchHeightLukeMcarthur(const double& ht) {
  scorchHeightLukeMcarthur_ = ht;}

  /*!\brief Sets van Wagner scorch height
    \param ht (double, m)

    Sets van Wagner scorch height to ht
  */
inline void Results::scorchHeightVanWagner(const double& ht) {
  scorchHeightVanWagner_ = ht;}

  /*!\brief Sets van Wagner scorch height with wind
    \param ht (double, m)

    Sets van Wagner scorch height with wind to ht
  */
inline void Results::scorchHeightVanWagnerWithWind(const double& ht) {
  scorchHeightVanWagnerWithWind_ = ht;}

  /*!\brief Adds stratum specific results to the vector of all such results
    \param sr (StratumResults)

    Adds sr to the vector of StratumResults objects
  */
inline void Results::addStratumResults(const StratumResults& sr) {strataResults_.push_back(sr);}

/*!\brief Adds a ForestIgnitionRun object to the vector of all such runs
  \param fir (ForestIgnitionRun)

  Adds fir to the vector of all ForestIgnitionRun objects
*/
inline void Results::addRun(const ForestIgnitionRun& fir) {
  if (runs_.empty() && fir.type() == ForestIgnitionRun::WITH_CANOPY)
    runs_.push_back(fir);
  if (runs_.size() == 1 && fir.type() == ForestIgnitionRun::WITHOUT_CANOPY)
    runs_.push_back(fir);
}

//other methods

/*!\brief Test for existence of strataResults
  \return true if and only if there are no StratumResults objects present in the Results object
*/ 
inline bool Results::strataResultsEmpty() const {return strataResults_.empty();}

/*!\brief Iterator
  \return A constant iterator to the beginning of the vector of StratumResults
*/
inline std::vector<StratumResults>::const_iterator Results::strataResultsBegin() const {
  return strataResults_.cbegin();
}

/*!\brief Iterator
  \return A constant iterator to (one past) the end of the vector of StratumResults
*/
inline std::vector<StratumResults>::const_iterator Results::strataResultsEnd() const {
  return strataResults_.cend();
}

//printing functions - they do conversions as well


/*!\brief Printing
  \return A formatted string describing overall rate of spread in km/h
*/
inline std::string Results::printROS() const {
  char s[30];
  sprintf(s, "%6.2f", ros_*3.6);
  return std::string(s);
}

/*!\brief Printing
  \return A formatted string describing overall flame tip height (m)
*/
inline std::string Results::printFlameTipHeight() const {
  char s[30];
  sprintf(s, "%6.2f", flameTipHeight_);
  return std::string(s);
}

/*!\brief Printing
  \return A formatted string describing overall flame origin height (m)
*/
inline std::string Results::printFlameOriginHeight() const {
  char s[30];
  sprintf(s, "%6.2f", flameOriginHeight_);
  return std::string(s);
}

/*!\brief Printing
  \return A formatted string describing overall flame length (m)
*/
inline std::string Results::printFlameLength() const {
  char s[30];
  sprintf(s, "%6.2f", flameLength_);
  return std::string(s);
}

/*!\brief Printing
  \return A formatted string describing overall flame angle (deg)
*/
inline std::string Results::printFlameAngle() const {
  char s[30];
  sprintf(s, "%6.2f", flameAngle_*180/PI);
  return std::string(s);
}

/*!\brief Printing
  \return A formatted string describing flame depth (m)
*/
inline std::string Results::printFlameDepth() const {
  char s[30];
  sprintf(s, "%6.1f", flameDepth_);
  return std::string(s);
}

/*!\brief Printing
  \return A formatted string describing surface fire rate of spread (km/h)
*/
inline std::string Results::printSurfaceROS() const {
  char s[30];
  sprintf(s, "%6.2f", surfaceROS_*3.6);
  return std::string(s);
}

/*!\brief Printing
  \return A formatted string describing surface fire flame length (m)
*/
inline std::string Results::printSurfaceFlameLength() const {
  char s[30];
  sprintf(s, "%6.2f", surfaceFlameLength_);
  return std::string(s);
}

/*!\brief Printing
  \return A formatted string describing surface fire flame height (m)
*/
inline std::string Results::printSurfaceFlameHeight() const {
  char s[30];
  sprintf(s, "%6.2f", surfaceFlameHeight_);
  return std::string(s);
}

/*!\brief Printing
  \return A formatted string describing surface fire flame angle (deg)
*/
inline std::string Results::printSurfaceFlameAngle() const {
  char s[30];
  sprintf(s, "%6.2f", surfaceFlameAngle_*180/PI);
  return std::string(s);
}


/*!\brief Printing
  \return A formatted string describing crown fire type
*/
inline std::string Results::printCrownFireType() const {
  return crownFireTypeStringMap.at(crownFireType_);
}

/*!\brief Printing
  \return A formatted string describing crown fire run length (m)
*/
inline std::string Results::printCrownRunLength() const {
  char s[30];
  sprintf(s, "%6.1f", crownRunLength_);
  return std::string(s);
}

/*!\brief Printing
  \return A formatted string describing crown run velocity (km/h)
*/
inline std::string Results::printCrownRunVelocity() const {
  char s[30];
  sprintf(s, "%6.2f", crownRunVelocity_*3.6);
  return std::string(s);
}

/*!\brief Printing
  \return A formatted string describing wind reduction factor
*/
inline std::string Results::printWindReductionFactor() const {
  char s[30];
  sprintf(s, "%4.2f", windReductionFactor_);
  return std::string(s);
}

/*!\brief Printing
  \return A formatted string describing scorch height according to McArthur model (m)
*/
inline std::string Results::printScorchHeightMcarthur() const {
  char s[30];
  sprintf(s, "%6.2f", scorchHeightMcarthur_);
  return std::string(s);
}

/*!\brief Printing
  \return A formatted string describing scorch height according to Luke-McArthur model (m)
*/
inline std::string Results::printScorchHeightLukeMcarthur() const {
  char s[30];
  sprintf(s, "%6.2f", scorchHeightLukeMcarthur_);
  return std::string(s);
}

/*!\brief Printing
  \return A formatted string describing scorch height according to van Wagner model (m)
*/
inline std::string Results::printScorchHeightVanWagner() const {
  char s[30];
  sprintf(s, "%6.2f", scorchHeightVanWagner_);
  return std::string(s);
}

/*!\brief Printing
  \return A formatted string describing scorch height according to van Wagner model with wind (m)
*/
inline std::string Results::printScorchHeightVanWagnerWithWind() const {
  char s[30];
  sprintf(s, "%6.2f", scorchHeightVanWagnerWithWind_);
  return std::string(s);
}

/*!\brief Printing
  \param outputLevel
  \return A formatted string describing the Results object

  There are several levels of detail that will be printed out, depending on the 
  value supplied for the outputLevel. BASIC results in printing of overall 
  values for rate of spread, flame height, flame length and flame angle. 
  DETAILED output includes all the  results computed by the model. COMPREHENSIVE 
  output includes, in addition, the ForestIgnitionRuns that formed part 
  of the computation (there are at most two of these). In the current function a
  value of MONTE_CARLO is equivalent to COMPREHENSIVE.

*/
inline std::string Results::printToString(const OutputLevelType& outputLevel) const {
  std::string str;
  char s[100];
  const std::string surface      = "Surface:       ";
  const std::string overall      = "Overall:       ";
  const std::string near_surface = "Near surface:  ";
  const std::string elevated     = "Elevated:      ";
  const std::string mid_storey   = "Mid storey:    ";
  const std::string canopy       = "Canopy:        ";

  str = "****** Results ******\n\n";

  str += "Output Level: " + outputLevelTypeStringMap.at(outputLevel) + "\n\n";

  std::string name;
  std::string blanks;

  name = "Flame length (m)                  ";
  str += name;
  str += overall;
  str += printFlameLength() + "\n";
  if (outputLevel >= DETAILED) {
    blanks = std::string(name.size(), ' ');
    str += blanks + surface + printSurfaceFlameLength() + "\n";
    for (const StratumResults& sr : strataResults_) {
      str += blanks + levelStringMap.at(sr.level()) + ":";
      str += std::string(surface.size() - levelStringMap.at(sr.level()).size() - 1,' ');
      str += sr.printFlameLength() + "\n";
    }
  }
  str += "\n";

  name = "Flame angle (deg)                 ";
  str += name;
  str += overall;
  str += printFlameAngle() + "\n";
  if (outputLevel >= DETAILED) {
    blanks = std::string(name.size(), ' ');
    str += blanks + surface + printSurfaceFlameAngle() + "\n";
    for (const StratumResults& sr : strataResults_) {
      str += blanks + levelStringMap.at(sr.level()) + ":";
      str += std::string(surface.size() - levelStringMap.at(sr.level()).size() - 1,' ');
      str += sr.printFlameAngle() + "\n";
    }
  }
  str += "\n";


  name = "Flame tip height (m)              ";
  str += name;
  str += overall;
  str += printFlameTipHeight() + "\n";
  if (outputLevel >= DETAILED) {
    blanks = std::string(name.size(), ' ');
    str += blanks + surface + printSurfaceFlameHeight() + "\n";
    for (const StratumResults& sr : strataResults_) {
      str += blanks + levelStringMap.at(sr.level()) + ":";
      str += std::string(surface.size() - levelStringMap.at(sr.level()).size() - 1,' ');
      str += sr.printFlameTipHeight() + "\n";
    }
  }
  str += "\n";

  name = "Flame origin height (m)           ";
  str += name;
  str += overall;
  str += printFlameOriginHeight() + "\n";
  if (outputLevel >= DETAILED) {
    blanks = std::string(name.size(), ' ');
    str += blanks + surface + "  0.00" + "\n";
    for (const StratumResults& sr : strataResults_) {
      str += blanks + levelStringMap.at(sr.level()) + ":";
      str += std::string(surface.size() - levelStringMap.at(sr.level()).size() - 1,' ');
      str += sr.printFlameOriginHeight() + "\n";
    }
  }
  str += "\n";

  if (outputLevel >= DETAILED) {
    name = "Species flame tip heights (m)     ";
    str += name;
    blanks = std::string(name.size(), ' ');
    char buf[10];

    bool first = true;
    for (const StratumResults& sr : strataResults_) {
      const std::map<std::string, double>& spHts = sr.speciesFlameTipHeights();
      for (std::map<std::string, double>::const_iterator it = spHts.begin(); it != spHts.end(); ++it) {
        if (!first) str += blanks;
       
        str += levelStringMap.at(sr.level()) + ":" ;
        str += std::string(surface.size() - levelStringMap.at(sr.level()).size() - 1,' ');

        sprintf(buf, "%6.2f", it->second);
        str += std::string(buf) + " " + it->first + "\n";

        first = false;
      }
    }
    str += "\n";
  }

  name = "Rate of spread (km/h)             ";
  str += name;
  str += overall;
  str += printROS() + "\n";
  if (outputLevel >= DETAILED) {
    blanks = std::string(name.size(), ' ');
    str += blanks + surface + printSurfaceROS() + "\n";
    for (const StratumResults& sr : strataResults_) {
      str += blanks + levelStringMap.at(sr.level()) + ":";
      str += std::string(surface.size() - levelStringMap.at(sr.level()).size() - 1,' ');
      str += sr.printROS() + "\n";
    }
  }
  str += "\n";

  if (outputLevel >= DETAILED) {
    name = "Proportion of stratum burnt (%):  ";
    str += name;
    bool firstTime = true;
    blanks = std::string();
    for (const StratumResults& sr : strataResults_) {
      str += blanks + levelStringMap.at(sr.level()) + ":";
      str += std::string(15 - levelStringMap.at(sr.level()).size() - 1,' ');
      str += sr.printProportionBurnt() + "\n";
      if (firstTime){
  	blanks = std::string(name.size(), ' ');
  	firstTime = false;
      }
    }
    str += "\n";
    str += "Flame depth (m):                                " + printFlameDepth() + "\n";
    str += "Crown fire type:                                " + printCrownFireType() + "\n";
    str += "Crown run length (m):                           " + printCrownRunLength() + "\n";
    str += "Crown run velocity (km/h)                       " + printCrownRunVelocity() + "\n";
    str += "Wind reduction factor:                          " + printWindReductionFactor() + "\n";
    str += "Scorch height McArthur (m):                     " + printScorchHeightMcarthur() + "\n";
    str += "Scorch height Luke McArthur (m):                " + printScorchHeightLukeMcarthur() + "\n";
    str += "Scorch height Van Wagner (m):                   " + printScorchHeightVanWagner() + "\n";
    str += "Scorch height Van Wagner with wind (m):         " + printScorchHeightVanWagnerWithWind() + "\n";
    str += "\n";

  }

  if (outputLevel >= COMPREHENSIVE) {
    if (!runs_.empty()){
      str += "Ignition paths\n\n";
      for (const ForestIgnitionRun& fir : runs_)
	str += fir.printToString();
    }
  }
  return str;
}

#endif //RESULTS_INLINE_H
