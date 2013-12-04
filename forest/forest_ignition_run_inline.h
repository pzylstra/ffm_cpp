#ifndef FOREST_IGNITION_RUN_INLINE_H
#define FOREST_IGNITION_RUN_INLINE_H

//constructors

/*!\brief Default constructor
 */
inline ForestIgnitionRun::ForestIgnitionRun() {}

/*!\brief Constructor
  \param f 

  Produces a ForestIgnitionRun containing the Forest object f but with 
  unspecified Runtype and an empty set of IgnitionPath objects.
*/
inline ForestIgnitionRun::ForestIgnitionRun(const Forest& f) : forest_(f) {}

/*!\brief Constructor
  \param rt The run type
  \param f The Forest object


  Produces a ForestIgnitionRun containing a Forest object f and with 
  specified RunType rt but an empty set of IgnitionPath objects.
*/
inline ForestIgnitionRun::ForestIgnitionRun(const RunType& rt, const Forest& f) 
  : type_(rt), forest_(f) {}
  
//accessors

/*!\brief Run type (ie with or without canopy)
  \return The ForestIgnitionRun::RunType
*/
inline ForestIgnitionRun::RunType ForestIgnitionRun::type() const {return type_;}

/*!\brief The Forest 
  \return The forest associated with the ForestIgnitionRun object.
*/
inline Forest ForestIgnitionRun::forest() const {return forest_;}

/*!\brief All the ignition paths
  \return The vector of IgnitionPath objects.
*/
inline std::vector<IgnitionPath> ForestIgnitionRun::paths() const {return paths_;}

/*!\brief The combined flames
  \return The vector (time series) of combined flames from the surface and all strata.
*/
inline std::vector<Flame> ForestIgnitionRun::combinedFlames() const {return combinedFlames_;}

//mutators

/*!\brief Set the run type
  \param run_type

  Sets the ForestIgnitionRun::RunType
*/
inline void ForestIgnitionRun::type(const RunType& run_type) {
  type_ = run_type;
}

/*!\brief Add an IgnitionPath object
  \param ip The ignition path to be added


  Adds an IgnitionPath object to the vector of all ignition paths
*/
inline void ForestIgnitionRun::addPath(const IgnitionPath& ip) {
  paths_.push_back(ip);
  std::sort(paths_.begin(), paths_.end(), 
	    [](IgnitionPath p1, IgnitionPath p2) {
	      return p1.level() < p2.level();});
}
  
/*!\brief Set the combined flames
  \param flames 

  Sets the vector of combined flames to the supplied vector of Flame objects.
*/
inline void ForestIgnitionRun::combinedFlames(const std::vector<Flame>& flames) {
  combinedFlames_ = flames;
}

//other methods

 
/*!\brief Iterator
  \return A constant iterator pointing to the beginning of the vector of 
  IgnitionPath objects.
*/ 
inline std::vector<IgnitionPath>::const_iterator ForestIgnitionRun::beginPaths() const {
  return paths_.cbegin();
}

/*!\brief Iterator
  \return A constant iterator pointing to (one past) the end of the vector of 
  IgnitionPath objects.
*/ 
inline std::vector<IgnitionPath>::const_iterator ForestIgnitionRun::endPaths() const {
  return paths_.cend();
}

/*!\brief Iterator
  \return A constant iterator pointing to the beginning of the vector of 
  combined flames.
*/ 
inline std::vector<Flame>::const_iterator ForestIgnitionRun::beginCombinedFlames() const{
  return combinedFlames_.cbegin();
}

/*!\brief Iterator
  \return A constant iterator pointing to (one past) the end of the vector of 
  combined flames.
*/ 
inline std::vector<Flame>::const_iterator ForestIgnitionRun::endCombinedFlames() const{
  return combinedFlames_.cend();
}

#endif //FOREST_IGNITION_RUN_INLINE_H
