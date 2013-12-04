#ifndef SURFACE_INLINE_H
#define SURFACE_INLINE_H

#include "ffm_numerics.h"

/*!\brief Default constructor*/
inline Surface::Surface() : slope_(-99), deadFuelMoistCont_(-99),
	       fuelLoad_(-99), meanFuelDiam_(-99), meanFineLeaves_(-99){}

/*!\brief Standard constructor
  \param slope (radians)
  \param dfmc Dead fuel moisture content
  \param fuelLoad (kg / m^2)
  \param fuelDiam Mean fuel diameter (m)
  \param thickL Mean fineness of leaves (m)
*/
inline Surface::Surface(const double& slope,  const double& dfmc,
			const double& fuelLoad, const double& fuelDiam, const double& thickL) : 
  slope_(slope), deadFuelMoistCont_(dfmc), 
	       fuelLoad_(fuelLoad), meanFuelDiam_(fuelDiam), meanFineLeaves_(thickL){
  bool okay = true;
  if (slope <= -PI || slope >= PI) okay = false;
  if (dfmc < 0) okay = false;
  if (fuelLoad < 0) okay = false;
  if (fuelDiam < 0) okay = false;
  if (thickL < 0) okay = false;
  if (!okay){
    slope_ = -99;
    fuelLoad_ = -99;
    meanFuelDiam_ = -99;
    meanFineLeaves_ = -99;
  }
}
/*!\brief Slope
  \return Slope of the surface (radians)
*/
inline double Surface::slope() const {return slope_;}

/*!\brief Dead fuel moisture content
  \return Dead fuel moisture content
*/
inline double Surface::deadFuelMoistCont() const {return deadFuelMoistCont_;}

/*!\brief Fuel load
  \return Fuel load (kg/m^2)
*/
inline double Surface::fuelLoad() const {return fuelLoad_;}

/*!\brief Mean fuel diameter
  \return Mean fuel diameter (m)
*/
inline double Surface::meanFuelDiam() const {return meanFuelDiam_;}

/*!\brief Mean fineness of leaves
  \return Mean fineness of leaves (ask P Zylstra for more information on this parameter)
*/
inline double Surface::meanFineLeaves() const {return meanFineLeaves_;}

//surface fire models

/*!\brief Ask Phil Zylstra for more information on this method*/
inline double Surface::thicknessMultiplier() const{
  //Taken from McArthur 1966 and rendered to equation form in Zylstra 2013
  //thicknessMultiplier = 0.5314*meanFineLeaves_^-0.401
  //where meanFineLeaves_ is in mm. Convert meanFineLeaves_ to mm from m
  return 0.5314*pow(meanFineLeaves_*1000,-0.401);
}

/*!\brief Rate of spread model for backing surface fire
  \return Backing fire rate of spread (m/s)
*/
inline double Surface::backingROS() const {
  // formula in Zylstra's spreadsheet amounts to 
  // (0.973*fuel_load + 4.23)/1000*EXP(0.069*slope) 
  // where output is in km/hr and slope is in degrees. We want all inputs
  // and outputs in m/s and radians
  // Multiply by thicknessMultiplier_
  if (deadFuelMoistCont_ >= ffm_settings::extinctionDFMC) return 0.0;
  if (fuelLoad_ < ffm_settings::minFuelLoadForSurfaceBackingFire) return 0.0;
  return thicknessMultiplier()*(2.703e-3*fuelLoad_ + 1.175e-3)*exp(3.9534*slope_);
}

/*!\brief Flame length model for backing surface fire
  \return Backing surface fire flame length (m)
*/
inline double Surface::backingFlameLength() const {
  if (deadFuelMoistCont_ >= ffm_settings::extinctionDFMC) return 0.0;
  if (fuelLoad_ < ffm_settings::minFuelLoadForSurfaceBackingFire) return 0.0;
  return 0.317*fuelLoad_ + 0.0167;
}

/*!\brief Rate of spread model for head surface fire
  \param surfaceWind Wind speed at surface (m/s)
  \return Head fire rate of spread (m/s)
*/
inline double Surface::headROS(const double& surfaceWind) const {
  // formula in Zylstra's spreadsheet amounts to:
  // (0.0245*wind^2.22+0.071)/(0.003+0.000922*dfmc)/1000*EXP(0.069*empirical_slope)
  // where wind is in km/hr, slope is in degrees and output is in km/hr. We want all inputs
  // and outputs in m/s and radians
  // Multiply by thicknessMultiplier()
  if (deadFuelMoistCont_ >= ffm_settings::extinctionDFMC) return 0.0;
  if (fuelLoad_ < ffm_settings::minFuelLoadForSurfaceHeadFire) return 0.0;
  double s = std::max(0.0, std::min(ffm_settings::maxSlopeForSurfROS, slope_));
  return thicknessMultiplier()*(0.42088*pow(surfaceWind,2.22) + 0.071)/(10.8 + 3.3192*deadFuelMoistCont_)*exp(3.9534*s);
}

/*!\brief Flame length model for head surface fire
  \param surfaceWind Wind speed at surface (m/s)
  \return Head surface fire flame length (m)
*/
inline double Surface::headFlameLength(const double& surfaceWind) const {
  if (deadFuelMoistCont_ >= ffm_settings::extinctionDFMC) return 0.0;
  if (fuelLoad_ < ffm_settings::minFuelLoadForSurfaceHeadFire) return 0.0;
  return 8.64*headROS(surfaceWind) + 0.36*fuelLoad_; //note 8.64 = 2.4*3.6 (conversion from m/s)
}

/*!\brief Overall surface fire rate of spread model
  \param surfaceWind Wind speed at surface (m/s)
  \return Rate of spread of surface fire (m/s)

  This method is applicable to both head and backing fires
*/
inline double Surface::ros(const double& surfaceWind) const {
  return std::min(ffm_settings::maxSurfROS, 
		  std::max(backingROS(), headROS(surfaceWind))
		  );
}

/*!\brief Overall surface fire flame length model
  \param surfaceWind Wind speed at surface (m/s)
  \return Flame length of surface fire (m)

  This method is applicable to both head and backing fires
*/
inline double Surface::flameLength(const double& surfaceWind) const {
  return std::min(ffm_settings::maxSurfFlameLength, 
		  surfaceWind > 0 ? headFlameLength(surfaceWind) : backingFlameLength()
		  );
}

/*!\brief Surface fire flame residence time model
  \return Flame residence time (s)
*/
inline double Surface::flameResidenceTime() const {
  // from spreadsheet =0.87*fuel_diameter^1.875 where fuel_diameter is in mm
  // need to convert fuel diameter to mm from m
  return 0.87*pow(meanFuelDiam_*1000,1.875);
}

//printing 

/*!\brief Printing
  \return A formatted string describing the contents of the Surface object
*/
inline std::string Surface::printToString() const {
  std::string str;
  str = "Slope (deg): " + printSlope() +"\n";
  str += "Dead fuel moisture content: " + printDeadFuelMoistCont() +"\n";
  str += "Fuel load (tonnes per hectare): " + printFuelLoad() +"\n";
  str += "Mean fuel diameter (m): " + printMeanFuelDiam() +"\n";
  str += "Mean leaf fineness (m): " + printMeanFineLeaves() +"\n";
  return str;
}

/*!\brief Conversion and printing
  \return A formatted string describing the slope in degrees
*/
inline std::string Surface::printSlope() const {
  char s[10];
  sprintf(s,"%.1f", slope_*180/PI);
  return std::string(s);
}

/*!\brief Printing
  \return A formatted string describing the dead fuel moisture content
*/
inline std::string Surface::printDeadFuelMoistCont() const {
  char s[10];
  sprintf(s,"%.3f", deadFuelMoistCont_);
  return std::string(s);
}

/*!\brief Conversion and printing
  \return A formatted string describing the fuel load in tonnes per hectare
*/
inline std::string Surface::printFuelLoad() const {
  //converts to tonnes/ha
  char s[10];
  sprintf(s,"%.1f", fuelLoad_*10);
  return std::string(s);
}

/*!\brief Printing
  \return A formatted string describing mean fuel diameter (m)
*/
inline std::string Surface::printMeanFuelDiam() const{
  char s[10];
  sprintf(s,"%.5f", meanFuelDiam_);
  return std::string(s);
}

/*!\brief Printing
  \return A formatted string describing mean fineness of leaves (m)
*/
inline std::string Surface::printMeanFineLeaves() const{
  char s[10];
  sprintf(s,"%.5f", meanFineLeaves_);
  return std::string(s);
}



#endif //SURFACE_INLINE_H
