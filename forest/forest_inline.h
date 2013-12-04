#ifndef FOREST_INLINE_H
#define FOREST_INLINE_H

#include "stratum.h"
#include "layer.h"

//constructors

/*!\brief Default constructor produces empty forest
 */
inline Forest::Forest() : surface_(), strata_() , strataOverlaps_() {}
//accessors

/*!\brief The Surface
  \return The Surface object associated with the Forest.
*/
inline Surface Forest::surface() const {return surface_;}

/*!\brief The strata of the Forest
  \return The vector of Stratum objects associated with the Forest.
*/
inline std::vector<Stratum> Forest::strata() const {return strata_;}

/*!\brief Overlap information
  \return A vector of type Forest::StrataOverlap containing the overlap information.
*/ 
inline std::vector<Forest::StrataOverlap> Forest::strataOverlaps() const {return strataOverlaps_;}

//other methods

/*!\brief Tests to see if the Forest contains any strata
  \return true if and only if the vector of strata is empty.
*/
inline bool Forest::empty() const {
  return strata_.empty();
}

/*!\brief Height for surface wind computation
  \return The height that should be used for surface wind computations. 
    
  This depends on whether or not the Forest contains a near-surface stratum.
*/
inline double Forest::heightForSurfaceWind() const {
  if (hasLevel(Stratum::NEAR_SURFACE)) 
    return std::max(ffm_settings::minHeightForWindComp, stratum(Stratum::NEAR_SURFACE).avMidHt());
  else
    return ffm_settings::minHeightForWindComp;
}

/*!\brief Printing
  \return A formatted string describing the Forest object
*/
inline std::string Forest::printToString() const {
  std::string str;
  str = "Surface characteristics:\n\n";
  str += surface_.printToString() + "\n";
  str += "List of strata:\n";
  for (Stratum s : strata_){
    str += "\n" + s.printToString();
  }
  str += "\nSpecified overlaps (others will default):\n\n";
  for (auto so : strataOverlaps_){
    str += overlapToString(so) + "\n";
  }
  str += "\n*** End of forest description ***\n";
  return str;
}


#endif //FOREST_INLINE_H
