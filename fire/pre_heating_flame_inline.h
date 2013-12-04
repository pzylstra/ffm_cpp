#ifndef PRE_HEATING_FLAME_INLINE_H
#define PRE_HEATING_FLAME_INLINE_H

//constructors

/*!\brief Default constructor
 */
inline PreHeatingFlame::PreHeatingFlame() : level_(Stratum::UNKNOWN_LEVEL),
					    flame_(), startTime_(0), endTime_(0) {}
/*!\brief Standard constructor
  \param level
  \param flame
  \param startTime
  \param endTime
*/
inline PreHeatingFlame::PreHeatingFlame(const Stratum::LevelType& level, const Flame& flame, 
					const double& startTime, const double& endTime) 
  : level_(level), flame_(flame), startTime_(startTime), endTime_(endTime) {}

//accessors

/*!\brief The Stratum::LevelType of the PreHeatingFlame
  \return The Stratum::LevelType of the PreHeatingFlame object.
*/
inline Stratum::LevelType PreHeatingFlame::level() const {return level_;}

/*!\brief The Flame
\return The Flame object
*/
inline Flame PreHeatingFlame::flame() const {return flame_;}

/*!\brief Starting time
  \return The starting time.
*/
inline double PreHeatingFlame::startTime() const {return startTime_;}

/*!\brief The end time
  \return The end time.
*/
inline double PreHeatingFlame::endTime() const {return endTime_;}

//other methods

/*!\brief The duration
  \return endTime() - startTime().
*/
inline double PreHeatingFlame::duration() const {
  return std::max(0.0, endTime_ - startTime_);
}

/*!\brief The duration
  \param end

  \return If end <= 0 returns duration(). Otherwise, returns the duration 
  as computed using end instead of endTime().
*/
inline double PreHeatingFlame::duration(const double& end) const {
  if (end <= 0)
    return duration();
  else
    return std::max(0.0, std::min(endTime_, end) - startTime_);
}


#endif //PRE_HEATING_FLAME_INLINE_H
