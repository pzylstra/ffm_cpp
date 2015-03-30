
#ifndef IGNITION_PATH_INLINE_H
#define IGNITION_PATH_INLINE_H

#include <algorithm>
#include <float.h>

#include "flame.h"

//constructors

/*!\brief Default constructor produces an empty path.
 */
inline  IgnitionPath::IgnitionPath() : 
  type_(UNKNOWN_PATH),
  level_(Stratum::UNKNOWN_LEVEL), 
  species_(), 
  startTimeStep_(-99), 
  ignitedSegments_(), 
  preIgnitionData_()
  {
    ignitedSegments_.reserve(ffm_settings::maxTimeSteps);
  }

/*!\brief Standard constructor.
  \param pathType
  \param lev
  \param species
  \param startTimeStep The time step in which ignition of the first segment occurs.
*/
inline IgnitionPath::IgnitionPath(const PathType& pathType,
                                  const Stratum::LevelType& lev, 
                                  const Species& species, 
                                  const int& startTimeStep) : 
  type_(pathType),
  level_(lev),  
  species_(species),
  startTimeStep_(startTimeStep),
  ignitedSegments_(), 
  preIgnitionData_()
  {
    ignitedSegments_.reserve(ffm_settings::maxTimeSteps);
  }
                    
//accessors 

/*!\brief Path type
  \return The PathType of the IgnitionPath.
*/
inline IgnitionPath::PathType IgnitionPath::type() const {return type_;}

/*!\brief Level of the path
  \return The Stratum::LevelType of the IgnitionPath.
*/ 
inline Stratum::LevelType IgnitionPath::level() const {return level_;}

/*!\brief The Species
  \return The Species through which the IgnitionPath is burning.
*/
inline Species IgnitionPath::species() const {return species_;}

/*!\brief Time step of ignition
  \return The time step in which the first segment of the IgnitionPath ignited.
*/
inline int IgnitionPath::startTimeStep() const {return startTimeStep_;}

/*!\brief The ignited segments
  \return The vector of ignited segments.
*/
inline std::vector<Seg> IgnitionPath::ignitedSegments() const {return ignitedSegments_;}

/*!\brief Extract a segment
  \param i
  \return The i'th segment, counting from i = 0.
*/
inline Seg IgnitionPath::ignitedSegment(const int& i) const {return ignitedSegments_.at(i);}
  
/*!\brief Pre-ignition data (pre-heating, incident flames, and final value)
  \return The vector of PreIgnitionData instances.
*/
inline std::vector<PreIgnitionData> IgnitionPath::preIgnitionData() const {return preIgnitionData_;}

//mutators

/*!\brief Sets the starting time step.
  \param startTimeStep
*/
inline void IgnitionPath::startTimeStep(const int& startTimeStep) {
  startTimeStep_ = startTimeStep;
}

/*!\brief Adds a segment to the end of the IgntionPath
  \param seg
*/
inline void IgnitionPath::addSegment(const Seg& seg) {ignitedSegments_.push_back(seg);} 

/*!\brief Adds a pre-ignition data rec
  \param data
*/
inline void IgnitionPath::addPreIgnitionData(const PreIgnitionData& data) {preIgnitionData_.push_back(data);} 

//other methods

/*!\brief Number of segments
  \return The number of segments in the IgnitionPath object.
*/
inline int IgnitionPath::numSegments() const {
  return ignitedSegments_.size();
}
/*!\brief Test for ignited segments
  \return true if there is at least one ignited segment
*/
inline bool IgnitionPath::hasSegments() const {
  return !ignitedSegments_.empty();
}

/*!\brief Test for pre-ignition data
  \return true if there is pre-ignition data
*/
inline bool IgnitionPath::hasPreIgnitionData() const {
  return !preIgnitionData_.empty();
}

inline double IgnitionPath::maxPreIgnitionTemp() const {
  double t = -DBL_MAX;

  for (const PreIgnitionData& pid : preIgnitionData_) {
    t = std::max(t, pid.temperature());
  }

  return t;
}

/*!\brief Test for IgnitionPath of maximum size
  \brief true if and only if size() is equal to ffm_settings::maxTimeSteps, which is the maximum size
  of an IgnitionPath.
*/
inline bool IgnitionPath::fullSize() const {
  return ignitedSegments_.size() == ffm_settings::maxTimeSteps;
}

/*!\brief Sorts ignited segments into descending order of length.
 */
inline void IgnitionPath::sortSegments() {
  //sorts the segments longest to shortest 
  //note the direction of cmp
  auto cmp = [](Seg s1, Seg s2){return s1.length() > s2.length();};
  sort(ignitedSegments_.begin(), 
       ignitedSegments_.end(), 
       cmp);
} 

/*!\brief Length of longest segment
  \return The length of the longest segment.
*/
inline double IgnitionPath::maxSegmentLength() const {
  if (!hasSegments()) return 0;
  auto i = max_element(ignitedSegments_.begin(), 
                       ignitedSegments_.end(), 
                       [](Seg s1, Seg s2){return s1.length() < s2.length();});
  return (*i).length();
}

/*!\brief Find the longest segment
  \return The index (counting from 0) of the first segment whose length
  is equal to the maximum segment length.
*/
inline int IgnitionPath::indexOfMaxSegment() const {
  if (!hasSegments()) return -1;
  auto i = max_element(ignitedSegments_.begin(), 
                       ignitedSegments_.end(), 
                       [](Seg s1, Seg s2){return s1.length() < s2.length();});
  return static_cast<int>(std::distance(ignitedSegments_.begin(),i));
}

/*!\brief Origin of longest segment
  \return The origin of the first segment whose length
  is equal to the maximum segment length.
*/
inline Pt IgnitionPath::originOfMaxSegment() const {
  return origin(indexOfMaxSegment());
}

/*!\brief Maximum height burnt
  \param slope
  \return The maximum height above the surface achieved by any part of any segment.
*/
inline double IgnitionPath::maxHeightBurnt(const double& slope) const {
  if (!hasSegments()) return 0;
  auto i = max_element(ignitedSegments_.begin(), ignitedSegments_.end(), 
                       [slope](Seg s1, Seg s2){return s1.end().y() - s1.end().x()*tan(slope)  < 
                                               s2.end().y() - s2.end().x()*tan(slope);});
  return (*i).end().y() - (*i).end().x()*tan(slope);
}

/*!\brief Maximum x-coordinate
  \return The maximum x-coordinate of any part of any ignited segment.
*/
inline double IgnitionPath::maxX() const {
  if (!hasSegments()) return 0;
  auto i = max_element(ignitedSegments_.begin(), ignitedSegments_.end(), 
                       [](Seg s1, Seg s2){return std::max(s1.start().x(),s1.end().x()) < 
                                          std::max(s2.start().x(), s2.end().x());});
  return std::max((*i).start().x(), (*i).end().x());
}

/*!\brief Maximum y-coordinate
  \return The maximum y-coordinate of any part of any ignited segment.
*/
inline double IgnitionPath::maxY() const {
  if (!hasSegments()) return 0;
  auto i = max_element(ignitedSegments_.begin(), ignitedSegments_.end(), 
                       [](Seg s1, Seg s2){return std::max(s1.start().y(),s1.end().y()) < 
                                          std::max(s2.start().y(), s2.end().y());});
  return std::max((*i).start().y(), (*i).end().y());
}

/*!\brief Maximum horizontal run
  \return The maximum distance to the right of the left-hand edge of the 
  species crown that is achieved by any part of any ignited segment.
*/
inline double IgnitionPath::maxHorizontalRun() const {
  //only intended for use with stratum runs
  if(type_ != STRATUM_PATH) return 0;
  if (!hasSegments()) return 0;
  return maxX() - species().crown().left();
}

/*!\brief Time to ignition
  \return startTimeStep() * ffm_settings::computationTimeInterval.
*/
inline double IgnitionPath::timeToIgnition() const {
  return startTimeStep() * ffm_settings::computationTimeInterval;
}

/*!\brief Time steps to maximum flame length
  \return The number of time steps elapsed after ignition to 
  when the maximum segment length is first achieved.
*/
inline int IgnitionPath::timeStepsIgnitionToMaxFlame() const {
  if (!hasSegments()) return 0;
  auto cmp = [](Seg s1, Seg s2){return s1.length() < s2.length();};
  return static_cast<int> (std::distance(ignitedSegments_.begin(),
                                             std::max_element(ignitedSegments_.begin(), 
                                                              ignitedSegments_.end(), cmp)));
}


/*!\brief Time to maximum flame length
  \return The time from ignition to maximum flame length (seconds)
*/
inline double IgnitionPath::timeIgnitionToMaxFlame() const {
  return timeStepsIgnitionToMaxFlame()*ffm_settings::computationTimeInterval;
}


/*!\brief Maximum flame length
  \return The maximum flame length produced by any ignited segment.
*/
inline double IgnitionPath::maxFlameLength() const {
  if (!hasSegments()) return 0;
  return species().flameLength(maxSegmentLength());
}

/*!\brief Flame length of last ignited segment
  \return The flame length that will be generated by the last ignited
  segment.
*/
inline double IgnitionPath::flameLength() const {
  //returns the flame length from the most recent ignited segment of the ignition path
  if (!hasSegments()) return 0;
  return species_.flameLength(ignitedLength());
}
  
/*!\brief Extract a flame length
  \param i
  \return The flame length generated by the i-th segment, counting from 0.
*/
inline double IgnitionPath::flameLength(const int& i) const {
  return species_.flameLength(ignitedLength(i));
}

/*!\brief Length of the last segment.
  \return Length of the last segment.
*/
inline double IgnitionPath::ignitedLength() const {
  //returns the ignited length from the most recent ignited segment of the ignition path
  if (!hasSegments()) return 0;
  return ignitedSegments_.back().length();
}
 
/*!\brief Extract a segment length
  \param i
  \return Length of the i-th segment, counting from i = 0.
*/
inline double IgnitionPath::ignitedLength(const int& i) const {
  return ignitedSegments_.at(i).length();
}

/*!\brief Origin of the last segment
  \return Origin of the last segment.
*/
inline Pt IgnitionPath::origin() const {
  if (!hasSegments()) return Pt();
  return ignitedSegments_.back().start();
}

/*!\brief Extract a segment origin.
  \param i
  \return Origin of the i-th segment conting from i = 0.
*/
inline Pt IgnitionPath::origin(const int& i) const {
  if (!hasSegments()) return Pt();
  return ignitedSegments_.at(i).start();
}


/*!\brief The last flame
  \param windSpeed The relevant wind speed used to compute the flame angle of the flame. 
  \param slope
  \return The Flame generated by the last segment.
*/
inline Flame IgnitionPath::flame(const double& windSpeed, const double& slope) const {
  if (!hasSegments()) return Flame();
  return Flame(flameLength(),
               windEffectFlameAngle(flameLength(), windSpeed, slope), 
               origin(), 
               ignitedLength(),
               level_ == Stratum::NEAR_SURFACE && species_.isGrass() ? 
               ffm_settings::grassFlameDeltaTemp : ffm_settings::mainFlameDeltaTemp);
}


/*!\brief Extract a flame
  \param idx
  \param windSpeed The relevant wind speed used to compute the flame angle.
  \param slope
  \return The Flame generated by the idx-th segment counting from idx = 0.
*/
inline Flame IgnitionPath::flame(const int& idx, const double& windSpeed, const double& slope) const {
  if (!hasSegments()) return Flame();
  return Flame(flameLength(idx),
               windEffectFlameAngle(flameLength(idx), windSpeed, slope), 
               origin(idx), 
               ignitedLength(idx),
               level_ == Stratum::NEAR_SURFACE && species_.isGrass() ? 
               ffm_settings::grassFlameDeltaTemp : ffm_settings::mainFlameDeltaTemp);
}

#endif //IGNITION_PATH_INLINE_H
