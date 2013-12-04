#ifndef FLAME_H
#define FLAME_H

#include "pt.h"
#include "ray.h"
#include "line.h"

/*! \brief Represents a single flame.

  A Flame object has an origin, a length, an angle, a depth of ignition and a temperature. The
  length is the combined length of Zones I and II of the flame (see Zylstra's thesis). The depth
  of ignition is the length of Zone I of the flame. The temperature is the increase above ambient
  temperature of Zone I of the flame. 
*/
class Flame{
public:

  //constructors

  Flame();
  Flame(const double& flameLength, const double& angle, const Pt& origin, 
	const double& depthIgnited, const double& deltaTemperature);

  //accessors

  double flameLength() const;
  double angle() const;
  Pt     origin() const;
  double depthIgnited() const;
  double deltaTemperature() const;

  //mutators

  void origin(const Pt& newOrigin);

  //other methods

  bool isNull() const;
  Pt tip() const;
  Ray plume() const;
  double plumeDeltaTemperature(const double& distToFlameOrigin) const;
  double plumeTemperature(const double& distToFlameOrigin, const double& ambientTemperature) const;
  double plumeTemperature(const Pt& point, const double& ambientTemperature) const;
  double inversePlumeTemperature(const double& targetTemperature, const double& ambientTemperature) const;

  //flame combination 

  Flame combine(const Flame& otherFlame, 
		const double& windSpeed, 
		const double& slope, 
		const double& fireLineLength) const; 

  Flame combine(const Flame& otherFlame) const; 

  std::string printToString() const;

private:
  double flameLength_;
  double angle_;
  Pt     origin_;
  double depthIgnited_;
  double deltaTemperature_;

}; //Flame


double effectiveSlope(const double& slope);

double windEffectFlameAngle(const double& flameLength,     
			    const double& windSpeed,
			    const double& slope);

double slopeEffectFlameAngle (const double& flameLength,
			      const double& slope,
			      const double& fireLineLength);

double flameAngle(const double& flameLength,
		  const double& windSpeed,
		  const double& slope,
		  const double& fireLineLength);


std::vector<Flame> combineFlames(const std::vector<Flame>& flames1, 
				 const std::vector<Flame>& flames2,
				 const double& windSpeed, 
				 const double& slope, 
				 const double& fireLineLength); 

std::vector<Flame> combineFlames(const std::vector<Flame>& flames1, 
				 const std::vector<Flame>& flames2); 

double laterallyMergedFlameLength(const double& flameLen, const double& firelineLen, 
				  const double& widthBurningZone, const double& plantSeparation);

//stats for flame vectors

int nonNullCount(const std::vector<Flame>& flames);
double maxFlameLength(const std::vector<Flame>& flames);
double cappedMaxFlameLength(const std::vector<Flame>& flames);
double meanFlameLength(const std::vector<Flame>& flames);
double stdDevFlameLength(const std::vector<Flame>& flames);
double meanAngle(const std::vector<Flame>& flames);
Pt meanOrigin(const std::vector<Flame>& flames);
double meanDepthIgnited(const std::vector<Flame>& flames);
double meanDeltaTemperature(const std::vector<Flame>& flames);
Flame meanFlame(const std::vector<Flame>& flames);

#include "flame_inline.h"

#endif //FLAME_H
