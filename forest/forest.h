#ifndef FOREST_H
#define FOREST_H

#include <tuple>
#include <string>

#include "surface.h" 
#include "stratum.h"

class Layer;
class Stratum;
/*!\brief A Forest object contains a complete description of the structure of a forest. 

  A Forest object contains a Surface object and up to four Stratum objects, one each of near-surface, 
  elevated, mid-storey and canopy. Any of these strata may be absent. A Forest object also 
  contains information about which strata will grow under other strata.
*/
class Forest{

public:

  /*!\brief Indicates the type of overlap between two strata

    A value of NOT_OVERLAPPED indicates that the lower Stratum does not grow under the higher one.
    A value of OVERLAPPED indicates that the lower Stratum does grow under the upper one. A value 
    of AUTO_CALC_OVERLAP indicates that a calculation based on the heights of the Strata will 
    be used to determine whether the lower Stratum will grow under the upper one. 
  */
  enum StrataOverlapType {AUTO_CALC_OVERLAP = -1, NOT_OVERLAPPED = 0, OVERLAPPED = 1};

  /*!\brief A type that specifies two strata and the value of the Forest::StrataOverlapType between them
   */
  typedef std::tuple<Stratum::LevelType, Stratum::LevelType, StrataOverlapType>  StrataOverlap;


  //constructors

  Forest();
  Forest(const Surface& surface, const std::vector<Stratum>& strata, 
	 const std::vector<StrataOverlap>& strataOverlaps);
  Forest(const Surface& surface, const std::vector<Stratum>& strata);

  //accessors

  Surface surface() const;
  std::vector<Stratum> strata() const;
  std::vector<StrataOverlap> strataOverlaps() const;

  //other methods

  Stratum::LevelType nextLevel(const Stratum::LevelType& thisLevel) const;
  std::vector<Layer> layers(const bool& includeCanopy = true) const;
  Stratum stratum(const Stratum::LevelType& level) const;
  bool empty() const;
  bool hasLevel(const Stratum::LevelType& level) const; 
  StrataOverlapType strataOverlap(const Stratum::LevelType& level1,
				  const Stratum::LevelType& level2) const;
  bool verticalAssociation(const Stratum::LevelType& level1, const Stratum::LevelType& level2) const;
  double windProfile(const double& windSpeedAboveCanopy, const double& height, 
		     const bool& includeCanopy = true) const;
  double heightForSurfaceWind() const;
  std::string printToString() const;



private:

  Surface surface_;
  std::vector<Stratum> strata_;
  std::vector<StrataOverlap> strataOverlaps_;

};


//map for names of strata overlaps (used for printing)
const std::map<Forest::StrataOverlapType, std::string> overlapStringMap =
  {{Forest::AUTO_CALC_OVERLAP, "Automatic"},
   {Forest::NOT_OVERLAPPED, "Not overlapped"},
   {Forest::OVERLAPPED, "Overlapped"}};

std::string overlapToString(const Forest::StrataOverlap&);



#include "forest_inline.h"

#endif //FOREST_H
