#ifndef SURFACE_H
#define SURFACE_H

#include "ffm_settings.h"

/*!\brief Represents the Surface of a Forest.

  The Surface class holds basic data about the forest surface and provides methods to
  compute surface fire characteristics.
*/
class Surface {

public:

  //constructors
  Surface();
  Surface(const double& slope, const double& deadFuelMoistureContent,
	  const double& fuelLoad, const double& meanFuelDiameter, const double& meanFinenessLeaves);

  //accessors

  double slope() const;
  double deadFuelMoistCont() const ;
  double fuelLoad() const ;
  double meanFuelDiam() const ;
  double meanFineLeaves() const ;

  //surface fire models

  double ros(const double& surfaceWindSpeed) const;
  double flameLength(const double& surfaceWindSpeed) const;
  double flameResidenceTime() const;
  double thicknessMultiplier() const;
  double backingROS() const; 
  double backingFlameLength() const;
  double headROS(const double& surfaceWindSpeed) const;
  double headFlameLength(const double& surfaceWindSpeed) const;


  //other methods
  std::string printToString() const;
  std::string printSlope() const;
  std::string printDeadFuelMoistCont() const;
  std::string printFuelLoad() const;
  std::string printMeanFuelDiam() const;
  std::string printMeanFineLeaves() const;

private:
  
  double slope_;
  double deadFuelMoistCont_ ;
  double fuelLoad_ ;
  double meanFuelDiam_ ;
  double meanFineLeaves_ ;


};

#include "surface_inline.h"

#endif //SURFACE_H
