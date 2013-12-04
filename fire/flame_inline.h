#ifndef FLAME_INLINE_H
#define FLAME_INLINE_H

//constructors

/*!\brief Default constructor*/
inline Flame::Flame() : flameLength_(-99), angle_(-99), depthIgnited_(-99), deltaTemperature_(-99), 
			origin_(Pt()) {};

/*!\brief Standard constructor.

  \param flame_length Combined length (m) of Zones I and II of flame (see Zylstra's thesis)
  \param flame_angle Angle of flame (radians) measured anti-clockwise from positive x-axis 
  \param origin (Pt) Origin of the flame
  \param depth_ignited Length of Zone I of the flame.
  \param delta_temperature Temperature (C) above ambient of Zone I of flame.  
*/
inline Flame::Flame(const double& flame_length, const double& flame_angle, const Pt& origin,
		    const double& depth_ignited, const double& delta_temperature) 
  : flameLength_(std::max(flame_length,depth_ignited)), angle_(flame_angle), origin_(origin), 
    depthIgnited_(depth_ignited), deltaTemperature_(delta_temperature)  {}

//other methods



/*!\brief Plume temperature above ambient
  \param dist Distance along plume to origin of flame 
  \return The increase above ambient temperature of the plume at specified distance.
*/
inline double Flame::plumeDeltaTemperature(const double& dist) const {
  //This is from Section 5.21 of Zylstra's thesis, with corrections
  //see also Weber et al as referenced in thesis
  //NOTE: takes no account of flame angle, ie assumes that dist is along plume
  if (isNull()) return -99;
  if (dist <= depthIgnited_) return deltaTemperature_;
  double a = -1/(2*flameLength_*(flameLength_ - depthIgnited_));
  if (dist > depthIgnited_ && dist <= flameLength_) 
    return deltaTemperature_*exp(a*(dist - depthIgnited_)*(dist - depthIgnited_));
  if (dist > flameLength_) 
    return deltaTemperature_ * flameLength_/dist*exp(a*(flameLength_ - depthIgnited_)*(flameLength_ - depthIgnited_));
}  


/*!\brief Plume temperature
  \param dist Distance along plume to origin of flame
  \param ambientTemp
  \return The temperature inside the plume at specified distance from
  the flame origin.
*/
inline double Flame::plumeTemperature(const double& dist, const double& ambientTemp) const {
  return plumeDeltaTemperature(dist) + ambientTemp;
}  

/*!\brief Plume temperature
  \param point The Pt at which the temperature is required
  \param ambientTemp
  \return The temperature at point inside the plume. 

  It is assumed that point 
  lies on the plume, and the computation is based solely on the distance from point 
  to the origin of the flame.
*/
inline double Flame::plumeTemperature(const Pt& point, const double& ambientTemp) const {
  //NOTE: assumes that point is on flame plume
  return plumeTemperature((point - origin_).norm(), ambientTemp);
}

/*!\brief Inverse plume temperature computation.
  \param targetTemp
  \param ambientTemp
  \return The distance (m) at which targetTemp will be achieved, measured along the plume 
  from the flame origin. 

  Returns -99 if targetTemp cannot be achieved.
*/
inline double Flame::inversePlumeTemperature(const double& targetTemp, const double& ambientTemp) const {
  // inversion of deltaPlumeTemp, returns distance from origin at which targetTemp is achieved
  if (flameLength_ < 0 || depthIgnited_ < 0 || flameLength_ < depthIgnited_) 
    return -99; 
  if (ffm_numerics::gt(targetTemp, deltaTemperature_ + ambientTemp)) 
    return -99;
  if (ffm_numerics::almostEq(targetTemp, deltaTemperature_ + ambientTemp))
    return depthIgnited_;
  double deltaT = targetTemp - ambientTemp;
  double a = -1/(2*flameLength_*(flameLength_ - depthIgnited_));
  if (ffm_numerics::gt(deltaT,deltaTemperature_*exp(a*(flameLength_ - depthIgnited_)*(flameLength_ - depthIgnited_)))) 
    return sqrt(log(deltaT/deltaTemperature_)/a) + depthIgnited_;
  else 
    return deltaTemperature_*flameLength_/deltaT*exp(a*(flameLength_ - depthIgnited_)*(flameLength_ - depthIgnited_));
}

/*!\brief Indicates null flames
  \return True if and only if flame length <= 0
*/
inline bool Flame::isNull() const {
  return ffm_numerics::leq(flameLength_,0);
}

/*!\brief Tip of the flame.
  \return Position (Pt) of the end of Zone II of the flame (see Zylstra's thesis).
*/
inline Pt Flame::tip() const {
  return origin_ + flameLength_*Pt(cos(angle_), sin(angle_));
}

/*!\brief Plume geometry
  \return Ray with same origin and direction as the Flame.
*/
inline Ray Flame::plume() const {
  return Ray(origin_, angle_);
}



//accessors

/*!\brief Flame angle
  \return Flame angle (rad) measured anti-clockwise from positive x-axis.
*/
inline double Flame::angle() const {return angle_;}

/*!\brief Flame origin.
  \return Position (Pt) of origin of flame
*/
inline Pt Flame::origin() const {return origin_;}

/*!\brief Flame length. 
  \return The combined length (m) of Zones I and II (see Zylstra's thesis).
*/
inline double Flame::flameLength() const {return flameLength_;}

/*!\brief Depth of burning material.
  \return Length (m) of Zone I of the flame (see Zylstra's thesis).
*/
inline double Flame::depthIgnited() const {return depthIgnited_;}

  /*!\brief Flame temperature above ambient.
    \return The difference (C) between the temperature in Zone I and the ambient temperature.
  */
inline double Flame::deltaTemperature() const {return deltaTemperature_;}

//mutators

/*!\brief Sets flame origin.
  \param newOrigin
*/
inline void Flame::origin(const Pt& newOrigin) {origin_ = newOrigin;}

//printing

/*!\brief Printing 
  \return Formatted string describing the Flame.
*/
inline std::string Flame::printToString() const {
  char s[20];
  std::string str;
  str = "Len (m): ";
  str += sprintf(s, "%0.2f", flameLength_);
  str += " Angle (deg): ";
  str += sprintf(s, "%.0f", angle_*180/PI);
  str += " Origin: ";
  str += origin_.printToString();
  str += " Depth ignited (m): ";
  str += sprintf(s, "%0.2f", depthIgnited_);
  str += " Delta temperature (C): ";
  str += sprintf(s, "%.0f", deltaTemperature_);
  return str;
}



#endif //FLAME_INLINE_H
