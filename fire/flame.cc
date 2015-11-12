#include <algorithm>
#include <numeric>

#include "ffm_settings.h"
#include "ffm_util.h"
#include "flame.h"

//flame angle model

/**\brief Effective slope for point fires

   Implements Section 5.222 of Zylstra's thesis. 
*/
double effectiveSlope(const double& slope) {
  // see Zylstra's thesis section 5.222
  // want to find the mean of arcsin(sin(theta_g)*cos(theta)) for theta in [-PI/2,PI/2]
  // Use Newton-Cotes quadrature of arcsin(sin(theta_g)*cos(theta)) for theta in [0,PI/2] 
  // because of symmetry

  //increase n for more accurate result, 20 seems adequate
  int n = 20;               
  double h = 0.5*PI/n;
  double s = sin(slope);

  double sum = 0;
  for(int k = 1; k <= n-1; ++k) sum += asin(s*cos(k*h));

  return (0.5*slope + sum)/n;
}

/*!\brief Flame angle model using wind effect only
  \param flameLength
  \param windSpeed
  \param slope
  \return Flame angle (radians) using wind effect flame angle model only.
*/
double windEffectFlameAngle(const double& flameLength,     
			    const double& windSpeed,
			    const double& slope){
  if(ffm_numerics::almostZero(flameLength))
    return 0;
  if (ffm_numerics::almostZero(windSpeed)) 
    return PI*0.5;

  double tmp;
  if (windSpeed >  0) 
    tmp = atan(0.88664*pow(flameLength,1.085)/pow(windSpeed,1.5));
  else 
    tmp = PI - atan(0.88664*pow(flameLength,1.085)/pow(std::abs(windSpeed),1.5));

  return std::min(PI + slope - ffm_settings::minFlameSepFromSlope,
		  std::max(tmp, slope + ffm_settings::minFlameSepFromSlope));
}

/*!\brief Flame angle model using slope effect only
  \param flameLength
  \param slope
  \param fireLineLength
  \return Flame angle (radians) using slope effect flame angle model only
*/
double slopeEffectFlameAngle (const double& flameLength,
			      const double& slope,
			      const double& fireLineLength) {
  if(ffm_numerics::almostZero(flameLength))
    return 0;
  double effSlope = (flameLength < fireLineLength ? slope : effectiveSlope(slope));
  double tmp;
  if (effSlope >= 0) 
    tmp = std::max(PI - effSlope, 0.25*PI);
  else
    tmp = std::min(PI - effSlope, 0.75*PI);

  return std::min(PI + slope - ffm_settings::minFlameSepFromSlope,
		  std::max(tmp, slope + ffm_settings::minFlameSepFromSlope));
 
}

/*!\brief Flame angle model
  \param flameLength
  \param windSpeed
  \param slope
  \param fireLineLength
  \return Flame angle (radians)
*/
double flameAngle(const double& flameLength,
		  const double& windSpeed,
		  const double& slope,
		  const double& fireLineLength
		  ) {
  if(ffm_numerics::almostZero(flameLength))
    return 0;
  if (ffm_numerics::almostZero(slope)) 
    return windEffectFlameAngle(flameLength, windSpeed, slope);
  
  if (slope > 0) {
    if (windSpeed >= 0) 
      return std::min(windEffectFlameAngle(flameLength, windSpeed, slope),
		      slopeEffectFlameAngle(flameLength,slope,fireLineLength));
    if (windSpeed < 0){
      if (std::abs(windSpeed) <= ffm_settings::slopeDominanceWindThreshold) 
	return slopeEffectFlameAngle(flameLength,slope,fireLineLength);
      else
	return windEffectFlameAngle(flameLength,windSpeed,slope);
    }
  }
  
  if (slope < 0) {
    if (windSpeed <= 0) 
      return std::max(windEffectFlameAngle(flameLength, windSpeed, slope),
		      slopeEffectFlameAngle(flameLength,slope,fireLineLength));
    if (windSpeed > 0) {
      if (std::abs(windSpeed) <= ffm_settings::slopeDominanceWindThreshold) 
	return slopeEffectFlameAngle(flameLength,slope,fireLineLength);
      else
	return windEffectFlameAngle(flameLength,windSpeed,slope);
    }
  }
}

/*!\brief Flame combination
  \param f The other Flame to be combined with *this
  \param windSpeed
  \param slope
  \param fireLineLength
  \return Resultant flame after combining *this with f. 

  The angle of the combined flame is computed using the flame angle model flameAngle().
*/
Flame Flame::combine(const Flame& f, const double& windSpeed, 
		    const double& slope, const double& fireLineLength) const {
  if (isNull()) return f;
  if (f.isNull()) return *this;
  //uses origin of *this unless f origin is lower
  Pt orig = f.origin().y() < origin_.y() ? f.origin() : origin_;
  //add depthIgnited values
  double depthIgnit = depthIgnited_ + f.depthIgnited();
  //flameLength weighted flame temperature average
  double temp = ( deltaTemperature_*flameLength_ + 
		  f.deltaTemperature()*f.flameLength() ) / 
    (flameLength_ + f.flameLength());
  //grotty calculation of combined flame length based on vertical overlaps
  double verticalOverlap = std::max(0.0,
				    std::min(tip().y(), f.tip().y())
				    - std::max(origin_.y(), f.origin().y()));
  double overlap1 = verticalOverlap/(tip().y() - origin_.y()) * flameLength_;
  double overlap2 = verticalOverlap/(f.tip().y() - f.origin().y()) * f.flameLength();
  double len = flameLength_ + f.flameLength() - 0.5*(overlap1 + overlap2);
  len = std::max(len,std::max(flameLength_,std::max(f.flameLength(),depthIgnit)));
  //combined flame angle depends on combined length and other stuff
  double ang = flameAngle(len, windSpeed, slope, fireLineLength);
  return Flame(len,ang,orig,depthIgnit,temp);
}


/*!\brief Flame combination
  \param f The Flame to be combined with *this
  \return Resulting flame after combining *this with f. 

  The angle of the combined flame is the flame-length 
  weighted average of the constituent flames.
*/
Flame Flame::combine(const Flame& f) const {
  if (isNull()) return f;
  if (f.isNull()) return *this;
  //uses origin of *this unless f origin is lower
  Pt orig = f.origin().y() < origin_.y() ? f.origin() : origin_;
  //add depthIgnited values
  double depthIgnit = depthIgnited_ + f.depthIgnited();
  //flameLength weighted flame temperature average
  double temp = ( deltaTemperature_*flameLength_ + 
		  f.deltaTemperature()*f.flameLength() ) / 
    (flameLength_ + f.flameLength());
  //grotty calculation of combined flame length based on vertical overlaps
  double verticalOverlap = std::max(0.0,
				    std::min(tip().y(), f.tip().y())
				    - std::max(origin_.y(), f.origin().y()));
  double overlap1 = verticalOverlap/(tip().y() - origin_.y()) * flameLength_;
  double overlap2 = verticalOverlap/(f.tip().y() - f.origin().y()) * f.flameLength();
  double len = flameLength_ + f.flameLength() - 0.5*(overlap1 + overlap2);
  len = std::max(len,std::max(flameLength_,std::max(f.flameLength(),depthIgnit)));
  //combined flame angle depends on combined length and other stuff
  double ang = (flameLength_*angle_ + f.flameLength()*f.angle()) / 
        (flameLength_ + f.flameLength());
  return Flame(len,ang,orig,depthIgnit,temp);
}

/*!\brief Element-wise combination of flames with flame angle computed using flame angle model
  \param flames1
  \param flames2
  \param w Windspeed in m/s
  \param s Slope in radians
  \param fll Length of fire line (m)
  \return A vector of flames representing the element-wise combination of the vectors
  flames1 and flames2
*/
std::vector<Flame> combineFlames(const std::vector<Flame>& flames1, const std::vector<Flame>& flames2,
				 const double& w, const double& s, const double& fll){
  if (flames1.empty()) return flames2;
  if (flames2.empty()) return flames1;
  std::vector<Flame> retVal;
  unsigned size1 = flames1.size();
  unsigned size2 = flames2.size();
  for (unsigned i = 0; i < std::min(size1, size2); ++i)
    retVal.push_back(flames1.at(i).combine(flames2.at(i),w,s,fll));
  if (size1 > size2)
    for(unsigned i = size2; i < size1; ++i)
      retVal.push_back(flames1.at(i));
  if (size2 > size1)
    for(unsigned i = size1; i < size2; ++i)
      retVal.push_back(flames2.at(i));
  return retVal;
} 

/*!\brief Element-wise combination of flames with flame angle computed using flame length
  weighted average of constituent flames
  \param flames1
  \param flames2
  \return A vector of flames representing the element-wise combination of flames1 and flames2, 
  with the combined flame angle computed using a flame length weighted average of the constituent 
  flames
*/
std::vector<Flame> combineFlames(const std::vector<Flame>& flames1, 
				 const std::vector<Flame>& flames2){
  if (flames1.empty()) return flames2;
  if (flames2.empty()) return flames1;
  std::vector<Flame> retVal;
  unsigned size1 = flames1.size();
  unsigned size2 = flames2.size();
  for (unsigned i = 0; i < std::min(size1, size2); ++i)
    retVal.push_back(flames1.at(i).combine(flames2.at(i)));
  if (size1 > size2)
    for(unsigned i = size2; i < size1; ++i)
      retVal.push_back(flames1.at(i));
  if (size2 > size1)
    for(unsigned i = size1; i < size2; ++i)
      retVal.push_back(flames2.at(i));
  return retVal;
} 

/*!\brief Lateral merging of flames
  \param flameLen Length of flame (m)
  \param firelineLen Length of fire line (m)
  \param w Width of the burning zone (m)
  \param sep Plant separation (m)
  \return Laterally merged flame length
*/
double laterallyMergedFlameLength(const double& flameLen, const double& firelineLen, 
				  const double& w, const double& sep) {
	//note that the formula below is modified from that in the thesis
	//by the additional factor of (1/9)^(2/3) ~ 0.2311, and also capped by firelineLen
	//as per spreadsheet.
  double sigma = std::min(0.23112*w*pow(flameLen/w,0.6667),firelineLen);
	//note that following merged flame length formula differs from that in thesis (Eq 5.75) 
	//by the addition of 1, as per advice from Phil Zylstra
  return flameLen * pow(sigma/sep + 1,0.4);
}

//stats for flame vectors

/*!\brief Number of non-null Flame objects in a vector of flames
  \param flames
  \return The number of elements of flames that have non-zero flame length
*/
int nonNullCount(const std::vector<Flame>& flames) {
  if (flames.empty()) return 0;
  return count_if(flames.begin(), flames.end(), [](const Flame& f) {return !f.isNull();});
}

/*!\brief Maximum flame length
  \param flames
  \return The maximum flame length of the elements of flames
*/
double maxFlameLength(const std::vector<Flame>& flames) {
  if (flames.empty()) return 0.0;
  auto iter = max_element(flames.begin(), flames.end(), 
			  [](const Flame& f1, const Flame& f2){return f1.flameLength() < f2.flameLength();});
  return (*iter).flameLength();
}

/*!\brief Maximum flame length capped by mean plus 1 standard deviation
  \param flames
  \return The maximum flame length of the elements of flames, with the 
  results being capped by the sum of the mean and the standard deviation of the flame lengths

  Zero flame lengths are ignored in the computation of the mean and standard deviation
*/
double cappedMaxFlameLength(const std::vector<Flame>& flames) {
  return std::min(maxFlameLength(flames), meanFlameLength(flames) + stdDevFlameLength(flames));
}

/*!\brief Mean flame length
  \param flames 
  \return Mean of flame length of elements of flames

  Zero flame lengths are ignored in the computation of the mean
*/
double meanFlameLength(const std::vector<Flame>& flames) {
  if (flames.empty()) return 0.0;
  std::vector<double> tmp;
  tmp.resize(flames.size());
  transform(flames.begin(), flames.end(), tmp.begin(),
	    [](const Flame& f){return (f.isNull()? 0.0 : f.flameLength());});
  return ffm_util::mean(tmp,true);
}

/*!\brief Standard deviation of flame lengths
  \param flames 
  \return Standard deviation of flame length of elements of flames

  Zero flame lengths are ignored in the computation of the standard deviation
*/
double stdDevFlameLength(const std::vector<Flame>& flames) {
  if (flames.size() < 2) return 0.0;
  std::vector<double> tmp;
  tmp.resize(flames.size());
  transform(flames.begin(), flames.end(), tmp.begin(),
	    [](const Flame& f){return (f.isNull()? 0.0 : f.flameLength());});
  return ffm_util::stdDev(tmp,true);
}

/*!\brief Mean flame angle
  \param flames 
  \return Mean of flame angle of elements of flames

  Flames of zero length are ignored in the computation of the mean
*/
double meanAngle(const std::vector<Flame>& flames) {
  if (flames.empty()) return 0.0;
  std::vector<double> tmp;
  tmp.resize(flames.size());
  transform(flames.begin(), flames.end(), tmp.begin(),
	    [](const Flame& f){return (f.isNull()? 0.0 : f.angle());});
  return ffm_util::mean(tmp,true);
}

/*!\brief Mean flame origin
  \param flames 
  \return Mean of flame origin of elements of flames

  Zero flame lengths are ignored in the computation of the mean
*/
Pt meanOrigin(const std::vector<Flame>& flames) {
  if (flames.empty()) return Pt();
  Pt ptSum = std::accumulate(flames.begin(), flames.end(), Pt(0,0), 
			[](const Pt& val, const Flame& f) {return val + (f.isNull() ? Pt(0,0) : f.origin());});
  return ptSum * (1.0/static_cast<double>(nonNullCount(flames)));
}

/*!\brief Mean flame depth of ignition
  \param flames 
  \return Mean depth of ignition of elements of flames

  Zero flame lengths are ignored in the computation of the mean
*/
double meanDepthIgnited(const std::vector<Flame>& flames) {
  if (flames.empty()) return 0.0;
  std::vector<double> tmp;
  tmp.resize(flames.size());
  transform(flames.begin(), flames.end(), tmp.begin(),
	    [](const Flame& f){return (f.isNull()? 0.0 : f.depthIgnited());});
  return ffm_util::mean(tmp,true);
}

/*!\brief Mean flame delta temperature
  \param flames 
  \return Mean flame delta temperature of elements of flames

  Zero flame lengths are ignored in the computation of the mean
*/
double meanDeltaTemperature(const std::vector<Flame>& flames) {
  if (flames.empty()) return 0.0;
  std::vector<double> tmp;
  tmp.resize(flames.size());
  transform(flames.begin(), flames.end(), tmp.begin(),
	    [](const Flame& f){return (f.isNull()? 0.0 : f.deltaTemperature());});
  return ffm_util::mean(tmp,true);
}

/*!\brief Mean flame
  \param flames 
  \return Mean flame of elements of flames

  Each of the attributes of the mean flame is the mean attribute of the elements of flames.
  Flames with zero flame length are ignored in the computation of the means. 
*/
Flame meanFlame(const std::vector<Flame>& flames) {
  return Flame(meanFlameLength(flames),
	       meanAngle(flames),
	       meanOrigin(flames),
	       meanDepthIgnited(flames),
	       meanDeltaTemperature(flames)
	       );
}



