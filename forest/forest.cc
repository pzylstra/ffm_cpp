#include <numeric>
#include <algorithm>
#include <map>

#include "forest.h"

using std::vector;

//constructors

/*!\brief Standard constructor
  \param surface A Surface object
  \param strata A vector of Stratum objects
  \param so A vector of type Forest::StrataOverlap specifying the overlap structure
  of the Forest
*/
Forest::Forest(const Surface& surface, const std::vector<Stratum>& strata, const std::vector<StrataOverlap>& so)
  : surface_(), strata_(), strataOverlaps_() {
  std::vector<Stratum::LevelType> levelCheck;
  for(const auto& st : strata){
    //if the level already exists then return empty strata_ 
    if(!levelCheck.empty() && find(levelCheck.begin(), levelCheck.end(), st.level()) < levelCheck.end()){
      strata_.clear();
      return;
    }
    if(!st.allSpecies().empty()) {
      strata_.push_back(st);
      levelCheck.push_back(st.level());
    }
  }
  if (!strata_.empty())
    sort (strata_.begin(), strata_.end());

  surface_ = surface;
  strataOverlaps_ = so;
}


/*!\brief Constructs a Forest with no overlap information

  \param surface A Surface object
  \param strata A vector of Stratum objects
*/
Forest::Forest(const Surface& surface, const std::vector<Stratum>& strata)
  : surface_(), strata_(), strataOverlaps_() {
  std::vector<Stratum::LevelType> levelCheck;
  for(const auto& st : strata){
    //if the level already exists then return empty strata_ 
    if(!levelCheck.empty() && find(levelCheck.begin(), levelCheck.end(), st.level()) < levelCheck.end()){
      strata_.clear();
      return;
    }
    if(!st.allSpecies().empty()) {
      strata_.push_back(st);
      levelCheck.push_back(st.level());
    }
  }
  if (!strata_.empty()){
    sort (strata_.begin(), strata_.end());
    surface_ = surface;
  }
}

/*!\brief The level of the next Stratum above thisLevel
  \param  thisLevel The Stratum::LevelType of the Stratum in question
  \return The next highest Stratum::LevelType of the Forest

  If thisLevel does not exist 
  in the forest, or is the highest level then returns Stratum::UNKNOWN_LEVEL
*/
Stratum::LevelType Forest::nextLevel(const Stratum::LevelType& thisLevel) const{
  //relies on the fact that the strata are ordered bottom to top
  auto i = find_if(strata_.begin(),strata_.end(),
		   [thisLevel](Stratum s){return s.level() == thisLevel;});
  if (i == strata_.end())
    return Stratum::UNKNOWN_LEVEL;

  i = find_if(strata_.begin(), strata_.end(),
		   [thisLevel](Stratum s){return s.level() > thisLevel;});
  if (i == strata_.end()) 
    return Stratum::UNKNOWN_LEVEL;
  else
    return (*i).level();
}

/*!\brief The bands (layers) of constant forest composition
  \param includeCanopy If false then the Canopy layer will be ignored
  in the computation of the layers
  \return A vector of Layer type containing all the layers of the Forest object.
  */
std::vector<Layer> Forest::layers(const bool& includeCanopy) const {
  //this method returns the horizontal Layers of uniform composition 
  //of the forest. Such a layer may consist of 0,1 or more than 1 Strata
  //NOTE: method returns layers sorted top to bottom, and note also that 
  //the bottom of the lower most layer is at height 0.

  //find limits of layers 
  std::vector<double> hts;
  for(auto& st : strata_){
    if (st.level() == Stratum::CANOPY && !includeCanopy)
      continue;
    hts.push_back(st.avBottom());
    hts.push_back(st.avTop());
  }
  hts.push_back(0);
  sort(hts.begin(), hts.end());
  hts.erase(unique(hts.begin(), hts.end(),ffm_numerics::almostEq), hts.end()); 
  reverse(hts.begin(), hts.end());
  std::vector<Layer> returnVec;
  std::vector<Stratum::LevelType> levelVec;
  for(auto i = hts.begin(); i != hts.end()-1; ++i){
    levelVec.clear();
    double x = 0.5*(*i + *(i+1)); //sample height in the middle of the band
    //loop over all strata and add to vector if sample height is in the stratum
    for(const Stratum& s : strata_) { 
      if (s.level() == Stratum::CANOPY && !includeCanopy)
	continue;
      if (x > s.avBottom() && x < s.avTop()) levelVec.push_back(s.level());
    }
    returnVec.push_back(Layer(*(i+1), *i, levelVec));
  }
  return returnVec;
}

/*!\brief Find the Forest::StrataOverlapType associated with level1 and level2.
  \param level1
  \param level2
  \return If strataOverlaps() returns a Forest::StrataOverlap associated with level1 and level2 then
  the associated Forest::StrataOverlapType is returned. Returns Forest::AUTO_CALC_OVERLAP if there is 
  no Forest::StrataOverlap associated with level1 and level2. 

  Order of level1 and level2 is not important. 
*/
Forest::StrataOverlapType Forest::strataOverlap(const Stratum::LevelType& level1, 
						const Stratum::LevelType& level2) const {
  //this method returns the overlap type for two strata (supplied as parameters). If there is no element
  //in strataOverlaps with these levels, some defaults are applied.
  Stratum::LevelType lev1 = level1;
  Stratum::LevelType lev2 = level2;
  if (lev1 == Stratum::UNKNOWN_LEVEL || lev2 == Stratum::UNKNOWN_LEVEL || lev2 == lev1) 
    return AUTO_CALC_OVERLAP;
  if (lev2 < lev1) std::swap(lev1, lev2);
  if (strataOverlaps_.empty()) return (((lev2 - lev1) <= 2) ? AUTO_CALC_OVERLAP : OVERLAPPED);
  auto i = find_if(strataOverlaps_.begin(), strataOverlaps_.end(),
		   [lev1,lev2](StrataOverlap v){
		     return (std::get<0>(v) == lev1 && std::get<1>(v) == lev2) || 
		     (std::get<0>(v) == lev2 && std::get<1>(v) == lev1);});
  if (i < strataOverlaps_.end()) 
    return std::get<2>(*i);
  else
    return (((lev2 - lev1) <= 2) ? AUTO_CALC_OVERLAP : OVERLAPPED);
}

  /*!\brief Determines whether the Stratum at level1 can exist under the Stratum at level2
    \param level1
    \param level2
    \return true if and only if the Stratum at the lower of level1 and level2 can exist
    under the Stratum at the higher of level1 and level2.

    The order of level1 and level2 is not important. Assuming level1 < level2 then the function
    computes whether 
    level1 can exist under level2 based on the value of 
    Forest::strataOverlap(const Stratum::LevelType& level1, const Stratum::LevelType& level2) const. If this value is 
    Forest::AUTO_CALC_OVERLAP then the function returns true if Stratum::avTop() of the lower
    level is less than Stratum::avBottom() of the upper level.
  */
bool Forest::verticalAssociation(const Stratum::LevelType& level1, const Stratum::LevelType& level2) const {
  Stratum::LevelType lev1 = level1;
  Stratum::LevelType lev2 = level2;
  if (lev2 < lev1) std::swap(lev1,lev2);
  if (strataOverlap(lev1, lev2) == Forest::OVERLAPPED)
    return true;
  else if (strataOverlap(lev1, lev2) == Forest::NOT_OVERLAPPED)
    return false;
  else if (strataOverlap(lev1, lev2) == Forest::AUTO_CALC_OVERLAP) {
    return stratum(lev1).avTop() <= stratum(lev2).avBottom(); 
  }
}


/*!\brief Extracts the Stratum at a prescribed Stratum::LevelType
  \param lev The Stratum::LevelType of the Stratum that is required
  \return The Stratum object corresponding to level. 

  If lev is not contained in the Forest then returns default empty stratum.
*/
Stratum Forest::stratum(const Stratum::LevelType& lev) const {
  auto i = find_if(strata_.begin(),strata_.end(), [lev](const Stratum& s){return s.level() == lev;});
  if (i == strata_.end()){
    Stratum tmp;
    return tmp;
  } else
    return *i;
}

/*!\brief Tests for the existence of a prescribed Stratum::LevelType in the Forest
  \param lev 
  \return true if and only if the Forest contains a Stratum with Stratum::LevelType equal to lev.
*/
bool Forest::hasLevel(const Stratum::LevelType& lev) const {
  for (Stratum s : strata_) if (s.level() == lev) return true;
  return false;
}

std::string overlapToString(const Forest::StrataOverlap& s){
  std::string str;
  str = levelStringMap.at(std::get<0>(s)) + " " +levelStringMap.at(std::get<1>(s)) + " : " 
    + overlapStringMap.at(std::get<2>(s));
  return str;
}

/*!\brief Wind speed at prescribed height
  \param w The wind speed above the canopy (m/s)
  \param z The height (metres above surface) at which the wind speed is desired
  \param includeCanopy
  \return wind speed (m/s) 
    
  The canopy is included when computing the wind speed  if and only if 
  includeCanopy == true.
*/
double Forest::windProfile(const double& w, const double& z, const bool& includeCanopy) const {
  //computes wind speed as follows:
  //1. Above the top Stratum, windspeed is equal to input windspeed
  //2. Between strata wind speed is constant
  //3. Within a layer containing strata, the LAIs of the strata are summed, and the 
  //   result is used to compute gamma using Eq 6.8 of Zylstra's thesis. Eq 6.7 is 
  //   then used to compute windspeed within the layer
  //4. If z < 0.1 then the wind speed is computed at 0.1
  //NOTE: The method Forest::layers() ensures that the layers start at the surface
  //      so the function always produces a return value. 

  if (w <= 0) return 0;

  double zz = z;
  if ( zz < ffm_settings::minHeightForWindComp ) zz = ffm_settings::minHeightForWindComp;

  std::vector<Layer> theLayers = layers(includeCanopy);

  if (theLayers.empty() || zz >= theLayers.front().top()) 
    return w;

  //lambda to compute the wind field within a layer that has strata
  auto f = [](const double& z, const double& w1, const double& z1, const double& gamma) -> double {
    // for a layer within the leaf mass
    return w1*exp(gamma*(z/z1 - 1));
  };


  //refW and refZ store the wind speed and height at the top of the current layer
  double refW = w;
  double refZ = theLayers.front().top();

  //loop over the layers, updating refW and refZ as we go. If z is in a layer
  //compute the windspeed at that height and return

  for (auto i = theLayers.begin(); i != theLayers.end(); ++i){

    if ( (*i).levels().empty() ) {
      //empty layer, we assume no loss in windspeed

      //if zz is in the layer return refW 
      if (zz >= (*i).bottom()) return refW;

      //if zz is not in the layer update the value of refZ,but refW stays the same
      refZ = (*i).bottom();

    } else {
      //the layer is not empty, so compute the sum of the lai's of the strata in the layer, and 
      //use this to compute gamma
      double lai = 0;
      for ( const Stratum::LevelType& lev : (*i).levels()) lai += stratum(lev).leafAreaIndex();
      double gamma = 1.785*pow(lai,0.372);

      //if zz is in the layer then return the wind speed at zz
      if (zz >= (*i).bottom()) return f(zz, refW, refZ, gamma);

      //otherwise z is not in the layer so compute the wind speed at the bottom of the layer
      //and update refW and refZ
      refW = f((*i).bottom(), refW, refZ, gamma);
      refZ = (*i).bottom();
    }
  }
}



