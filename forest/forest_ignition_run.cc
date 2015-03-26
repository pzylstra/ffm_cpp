#include <algorithm>

#include "forest_ignition_run.h"
#include "ffm_util.h"
#include "results.h"

// Initialize static key string used with map returned by speciesWeightedFlameLengths
// and laterallyMergedSpeciesWeightedPlantFlameLengths.
//
const std::string ForestIgnitionRun::KEY_WEIGHTED_AVERAGE("WEIGHTED_AVERAGE");


/*!\brief Determines whether the fire spreads in a particular Stratum
  \param lev The Stratum::LevelType under consideration

  \return true if and only if the run has at least one IgnitionPath with Stratum::LevelType 
  equal to lev that is classified as spreading
*/
bool ForestIgnitionRun::spreadsInStratum(const Stratum::LevelType& lev) const {
  for (const auto& ip : paths_) {
    if (ip.level() != lev || ip.type() != IgnitionPath::STRATUM_PATH) continue;
    if (ip.spreads() && ip.species().composition() > 0) return true;
  }
  return false;
}


/*!\brief Composition-weighted flame lengths
  \param lev
  \param ptype
  \return Composition-weighted flame lengths for all species with Stratum::LevelType equal to lev 
  and IgnitionPath::PathType equal to ptype. Flame lengths are sorted in decreasing order before 
  weighted sum is taken. The results are returned as a map with keys for species names (individual
  species flame lengths used for averaging) and KEY_WEIGHTED_AVERAGE (weighted average flame
  lengths).
*/
std::map<std::string, std::vector<double>>  ForestIgnitionRun::speciesWeightedFlameLengths(
    const Stratum::LevelType& lev, 
    const IgnitionPath::PathType& ptype) const {

  std::map<std::string, std::vector<double>> res;
  
  std::vector<double> weightedLengths(ffm_settings::maxTimeSteps, 0);

  for (IgnitionPath ip : paths_) {
    if (!ip.hasSegments() || ip.level() != lev || ip.type() != ptype) continue;
    
    double comp = ip.species().composition();
    double flen;
    std::vector<double> spLengths(ip.numSegments(), 0);

    ip.sortSegments();
    for (int i = 0; i < ip.numSegments(); ++i) {
      flen = ip.flameLength(i);
      spLengths.at(i) = flen;
      weightedLengths.at(i) += comp * flen;
    }

    res.insert( std::pair<std::string, std::vector<double>>(ip.species().name(), spLengths) );
  }

  res.insert( std::pair<std::string, std::vector<double>>(KEY_WEIGHTED_AVERAGE, weightedLengths) );
  
  return res;
}


  /*!\brief Lateral merging and combination of plant ignition paths
    \param lev The Stratum::LevelType of the Stratum under consideration
    \param firelineLen Length of the fire line (m)
    \return Laterally merged composition-weighted plant flame lengths

    Finds each IgnitionPath object of type IgnitionPath::PLANT_PATH in the Stratum with 
    Stratum::LevelType equal to lev, 
    computes the vector of laterally merged flame lengths from each of these ignition paths, 
    sorts these vectors in order of decreasing size, and returns their 
    composition-weighted component-wise sum.
  */
std::map<std::string, std::vector<double>>  ForestIgnitionRun::laterallyMergedSpeciesWeightedPlantFlameLengths(
    const Stratum::LevelType& lev, 
    const double& firelineLen) const {


  // Get basic species weighted flame lengths.
  // Returned map contains vector of weighted lengths plus N
  // vectors of contributing flame lengths for the N species
  // in the stratum.
  std::map<std::string, std::vector<double>> flameLengths = speciesWeightedFlameLengths(lev, IgnitionPath::PLANT_PATH);

  Stratum strat = forest_.stratum(lev);
  double w = forest_.stratum(lev).avWidth();
  double sep = forest_.stratum(lev).modelPlantSep();

  // Do lateral merging of both the weighted average flame lengths, and the
  // the individual species flame lengths. Note we use the stratum plant
  // separation parameter for both average and species lengths here.
  for (std::map<std::string, std::vector<double>>::iterator it = flameLengths.begin();
      it != flameLengths.end(); ++it) {

    std::vector<double>& flens = it->second;

    transform(flens.begin(), flens.end(), flens.begin(), 
        [firelineLen, w, sep](const double& fl) {
        return fl > 0? laterallyMergedFlameLength(fl, firelineLen, w, sep) : 0;});
  }

  return flameLengths;
}

/*!\brief Average rate of spread
  \param lev
  \return The composition-weighted sum of the IgnitionPath::basicROS() of all stratum ignition paths
  with Stratum::LevelType equal to lev.
*/
double ForestIgnitionRun::speciesWeightedBasicROS(const Stratum::LevelType& lev) const {
  double returnValue = 0;
  for (const IgnitionPath& ip : paths_) {
    if (ip.level() != lev || ip.type() != IgnitionPath::STRATUM_PATH) continue;
    returnValue += ip.basicROS()*ip.species().composition();
  }
  return returnValue;
}

/*!\brief Average ignition time step
  \param lev
  \param ptype
  \return The composition-weighted sum of the ignition time steps over all ignition 
  paths with IgnitionPath::PathType equal to ptype and with Stratum::LevelType equal
  to lev.
*/
double ForestIgnitionRun::speciesWeightedIgnitionTimeStep(const Stratum::LevelType& lev,
							  const IgnitionPath::PathType ptype) const {
  double returnValue = 0;
  for (const IgnitionPath& ip : paths_) {
    if (!ip.hasSegments() || ip.level() != lev || ip.type() != ptype) continue;
    returnValue += ip.species().composition()*ip.startTimeStep();
  }
  return returnValue;
}

  /*!\brief Average time to maximum flame
    \param lev
    \param ptype
    \return The composition-weighted sum of the number of time steps from ignition to maximum
    flame length over all ignition paths with IgnitionPath::PathType equal to ptype
    and with Stratum::LevelType equal to lev.
  */
double ForestIgnitionRun::speciesWeightedTimeStepsIgnitionToMaxFlame(const Stratum::LevelType& lev,
					     const IgnitionPath::PathType ptype) const {
  double returnValue = 0;
  for (const IgnitionPath& ip : paths_) {
    if (!ip.hasSegments() || ip.level() != lev || ip.type() != ptype) continue;
    returnValue += ip.species().composition()*ip.timeStepsIgnitionToMaxFlame();
  }
  return returnValue;
}

/*!\brief Rate of spread of active crown fire
  \param res
  \return The rate of spread of an active crown fire. 

  Relies on stratum flame angle results
  previously computed and placed in res.
*/
double ForestIgnitionRun::activeCrownFireROS(const Results& res) const {
  // //basically implements equation 6.51 from the thesis. Note that there are several attempts at specs for this 
  // //computation. This computation appears to be too complex in my opinion.
  double ignitionTimeSum = 0;
  double horizontalDistanceSum = 0;
  for (const Stratum& st : forest_.strata()) {
    ignitionTimeSum  += ( speciesWeightedIgnitionTimeStep(st.level(), IgnitionPath::PLANT_PATH) +
    			  speciesWeightedTimeStepsIgnitionToMaxFlame(st.level(), IgnitionPath::PLANT_PATH) +
    			  speciesWeightedIgnitionTimeStep(st.level(),IgnitionPath::STRATUM_PATH) ) * 
      ffm_settings::computationTimeInterval; 
    //find plant ignition time step from next stratum
    if (st.level() != Stratum::CANOPY) {
      Stratum::LevelType nextLev = forest_.nextLevel(st.level());
      int nextIgnitTimeStep = static_cast<int>(floor(speciesWeightedIgnitionTimeStep(nextLev, 
   										     IgnitionPath::PLANT_PATH)));

      //where has the statum fire from the current stratum reached when the next 
      //stratum plant fire ignites? This is as per my (Chris Thomas) best understanding 
      //of the specs but in my opinion it is problematic to species weight results from
      //individual time steps like this. For a start, the individual species ignite at 
      //different times. 
      Pt weightedFlameOrigin(0,0);
      for (const Species& sp : st.allSpecies()) {
	//find the stratum ignition path for this species
	auto i = find_if(paths_.begin(), paths_.end(),
			 [sp,st](IgnitionPath ip){return ip.species().sameSpecies(sp) && 
						  ip.level() == st.level() &&
						  ip.type() == IgnitionPath::STRATUM_PATH;});
  	if (i < paths_.end())
  	  //there is a stratum path for this species
  	  weightedFlameOrigin += (*i).origin(std::min((*i).numSegments() - 1, nextIgnitTimeStep - 1)) * sp.composition();
  	else {
  	  //there was no stratum path for this species, we'll use the last origin from the plant path
  	  i = find_if(paths_.begin(), paths_.end(), 
		      [sp,st](IgnitionPath ip){return ip.species().sameSpecies(sp) && 
					       ip.level() == st.level() &&
					       ip.type() == IgnitionPath::PLANT_PATH;});
  	  if (i < paths_.end())
  	    weightedFlameOrigin += (*i).origin() * sp.composition();
  	}
      } //end of loop over species
      //horizontal distances are measured from left edge of "first" plant
      horizontalDistanceSum += (weightedFlameOrigin.x() + st.avWidth()*0.5);
      //find horizontal distance bridged from this stratum to next stratum
      double y = forest_.stratum(nextLev).avBottom();
      Line bottomOfNextStratum(Pt(0,y), forest_.surface().slope());
      if (res.strataResults().empty()) 
  	//problem - this should not happen
  	return -99;
      auto i = find_if(res.strataResultsBegin(), res.strataResultsEnd(),
  		    [st](StratumResults sr){return sr.level() == st.level();});
      if (i == res.strataResultsEnd())
  	//stratum st was not found in the results. Again this should not happen
  	return -99;
      Ray r(weightedFlameOrigin, (*i).flameAngle());
      Pt intsct;
      r.intersects(bottomOfNextStratum, intsct);
      //add the horizontal distance
      horizontalDistanceSum += (intsct.x() - weightedFlameOrigin.x());
    }
  } //end of loop over strata

  //add the species weighted distance travelled in the Canopy
  for (const Species& sp : forest_.stratum(Stratum::CANOPY).allSpecies()) {
    auto i = find_if(paths_.begin(), paths_.end(),
  		  [sp](IgnitionPath ip){return ip.level() == Stratum::CANOPY &&
  					ip.species().sameSpecies(sp) &&
  					ip.type() == IgnitionPath::STRATUM_PATH;});
    if (i < paths_.end())
      horizontalDistanceSum += ((*i).maxX() + sp.width()*0.5)*sp.composition();
    else {
      //did not find a stratum path for this species, look for the plant path
      i = find_if(paths_.begin(), paths_.end(),
  	       [sp](IgnitionPath ip){return ip.level() == Stratum::CANOPY &&
  				     ip.species().sameSpecies(sp) &&
  				     ip.type() == IgnitionPath::PLANT_PATH;});
      if (i < paths_.end())
  	horizontalDistanceSum += ((*i).maxX() + sp.width()*0.5)*sp.composition();
    }
  }
  //compute time when fire spread in the canopy. Presumably we weight this by species again
  double speciesWeightedTimeOfSpread = 0;
  for (const IgnitionPath& ip : paths_) {
    speciesWeightedTimeOfSpread += ip.timeOfSpread() * ip.species().composition();
  }

  return horizontalDistanceSum / (ignitionTimeSum + speciesWeightedTimeOfSpread);
 }

/*!\brief Average origin of maximum flame
  \param lev
  \param ptype
  \return The composition-weighted sum of the origins of the maximum flames over all ignition paths
  with IgnitionPath::PathType equal to ptype and with Stratum::LevelType equal to lev.
*/
Pt ForestIgnitionRun::speciesWeightedOriginOfMaxFlame(const Stratum::LevelType& lev, 
						      const IgnitionPath::PathType& ptype) const {
  Pt returnValue(0,0);
  for (const IgnitionPath ip : paths_) {
    if (!ip.hasSegments() || ip.level() != lev || ip.type() != ptype)
      continue;
    returnValue += ip.species().composition()*ip.originOfMaxSegment();
  }
  return returnValue;
}

/*!\brief Average maximum height burnt
  \param lev
  \return The composition-weighted sum of the maximum heights burnt over all species 
  with Stratum::LevelType equal to lev.
*/
double ForestIgnitionRun::speciesWeightedMaxHeightBurnt(const Stratum::LevelType& lev) const{ 
  if (paths_.empty()) return 0;
  double returnValue = 0; 
  //loop over the species
  for (const Species& spec : forest_.stratum(lev).allSpecies()){
    double tmp = 0;
    auto i = find_if(paths_.begin(), paths_.end(),
		     [spec](IgnitionPath ip){return ip.species().sameSpecies(spec) && 
					     ip.type() == IgnitionPath::STRATUM_PATH;});
    if (i != paths_.end())
      tmp = (*i).maxHeightBurnt(forest_.surface().slope());

    auto j = find_if(paths_.begin(), paths_.end(),
		     [spec](IgnitionPath ip){return ip.species().sameSpecies(spec) &&
					     ip.type() == IgnitionPath::PLANT_PATH;});
    if (j != paths_.end()){
      tmp = std::max(tmp, (*j).maxHeightBurnt(forest_.surface().slope()));
    }
    returnValue += spec.composition()*tmp;
  }
  return returnValue;
}

/*!\brief Printing
  \return A formatted string describing the ForestIgnitionRun object.
*/
std::string ForestIgnitionRun::printToString() const {
  std::string str = "";
  if (!paths_.empty())
    for (const IgnitionPath& ip : paths_){
      str += "Run type:                 " + runTypeStringMap.at(type_) + "\n";      
      str += ip.printToString();
      str += "\n";
    }
  return str;
}


