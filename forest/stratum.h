#ifndef STRATUM_H
#define STRATUM_H

#include <map>

#include "ffm_settings.h"
#include "species.h"

/*!\brief A Stratum is a layer of the forest, and contains one or more species.

  The model currently allows for four strata, near surface, elevated, mid-storey
  and canopy. The surface layer is handled separately by the Surface class.
*/

class Stratum{

public:

  /*!\brief Specifies the level of the Stratum object.

    The SURFACE value is included only for convenience. The level() attribute of a 
    Stratum object can only take values SURFACE, NEAR_SURFACE, ELEVATED, MID_STOREY and CANOPY
  */
  enum LevelType{UNKNOWN_LEVEL = -1, SURFACE, NEAR_SURFACE, ELEVATED, MID_STOREY, CANOPY};
  
  //constructors

  Stratum();
  Stratum(const LevelType& level,
	  const std::vector<Species>& allSpecies, 
	  const double& plantSeparation,
	  bool includeForIgnition);

  //accessors

  LevelType level() const;
  std::vector<Species> allSpecies() const;
  double plantSep() const;
  bool includeForIgnition() const;

  //other methods

  double avWidth() const;
  double avTop() const;
  double avBottom() const;
  double avMidHt() const;
  double avFlameDuration() const;
  double modelPlantSep() const;

  ///Proportion of surface area covered by Stratum.
  double cover() const;

  ///Leaf area index of Stratum.
  double leafAreaIndex() const;

  //operators

  //these are comparison operators based on natural ordering of LevelType
  //ie ordering increasing from lower stratum up
  bool operator<(const Stratum&) const;
  bool operator>=(const Stratum&) const;
  bool operator>(const Stratum&) const;
  bool operator<=(const Stratum&) const;

  //other methods

  std::string printToString() const;
  std::string printPlantSep() const;
  std::string name() const;
  //returns the name of the stratum eg "Near surface" etc

private:

  LevelType level_;
  std::vector<Species> allSpecies_;
  double plantSep_;
  bool includeForIgnition_;
};


//map for names of stratum levels (used for printing)
const std::map<Stratum::LevelType, std::string> levelStringMap = 
  {{Stratum::UNKNOWN_LEVEL, "Unknown"},
   {Stratum::SURFACE, "Surface"},
   {Stratum::NEAR_SURFACE, "Near-surface"},
   {Stratum::ELEVATED, "Elevated"},
   {Stratum::MID_STOREY, "Mid-storey"},
   {Stratum::CANOPY, "Canopy"}};


#include "stratum_inline.h"

#endif //STRATUM_H
