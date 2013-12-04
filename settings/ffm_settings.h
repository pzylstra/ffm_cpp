#ifndef FFM_SETTINGS_H
#define FFM_SETTINGS_H

#include <cmath>

/*!\brief Various settings*/
namespace ffm_settings{
  
  /*!\brief The atomic unit of time for the computation*/
  const double computationTimeInterval = 1.0;

  /*!\brief Maximum Dead Fuel Moisture Content for a surface fire to exist*/
  const double extinctionDFMC = 0.2;

  /*!\brief Minimum fuel load for existence of a Surface backing fire*/
  const double minFuelLoadForSurfaceBackingFire = 0.4; // kg per m^2

  /*!\brief Minimum fuel load for existence of a Surface head fire*/  
  const double minFuelLoadForSurfaceHeadFire = 0.3;    // kg per m^2

  /*!\brief Minimum separation from slope when computing flame angles

    When computing flame angles the flame cannot adhere to the slope more 
    closely than this setting will allow
  */
  const double minFlameSepFromSlope = 0.01745;   // radians

  /*!\brief Threshold wind speed for flame angles to be computed using wind effect

    If the absolute value of the wind speed is less than slopeDominanceWindThreshold
    then flame angles are computed using the slope effect model, otherwise they are 
    computed using the wind effect model
  */
  const double slopeDominanceWindThreshold = 0.8333; // metres/sec

  /*!\brief Surface fire rate of spread cannot exceed this value (m/s)*/
  const double maxSurfROS = 9.72e-2;

  /*!\brief Surface fire flame lengths cannot exceed this value (m)*/
  const double maxSurfFlameLength = 2.0;

  /*!\brief This is the maximum slope that will be used to compute a surface 
    head fire rate of spread
  */
  const double maxSlopeForSurfROS = 0.698; // radians (40 degrees)

  /*!\brief Ignition delay time is reduced by this factor for a grass*/
  const double grassIDTReduction = 0.75;

  /*!\brief The increase above ambient temperature for flames in all species aside from grasses*/
  const double mainFlameDeltaTemp = 950;

  /*!\brief The increase above ambient temperature for flames in grasses*/
  const double grassFlameDeltaTemp = 750;

  /*!\brief The minimum height above surface for the computation of wind speed*/
  const double minHeightForWindComp = 0.1;

  /*!\brief Number of steps used to compute penetration at each time step

    At each time step the full possible path of ignition (based on flame angles, 
    plume temperatures and ignition temperatures, is divided into numPenetrationSteps
    and an iterative computation is performed to see how much of this path is ignited
  */
  const int numPenetrationSteps = 10;

  /*!\brief The maximum length of time for any single computation of an IgnitionPath
   */
  const double maxTime = 25; //used to initialise storage for some vectors

  /*!\brief The maximum number of time steps that will be used for the computation of any
    single IgnitionPath

    This is computed from ffm_settings::maxTime and ffm_settings::computationTimeInterval
  */
  const int maxTimeSteps = round(ffm_settings::maxTime/ffm_settings::computationTimeInterval);

  /*!\brief This is the minimum rate of spread for a stratum fire for it to be 
    classified as spreading in a particular time step
  
    A stratum fire must spread at at least ffm_settings::minRateForStratumSpread for 
    at least ffm_settings::minTimeStepsForStratumSpread time steps to be classified as 
    spreading through the stratum
  */
  const double minRateForStratumSpread = 2.7778e-3; //0.01 km/h

  /*!\brief This is the minimum number of time steps that a Stratum fire must spread
    for to be classified as spreading through the Stratum
  
    A stratum fire must spread at at least ffm_settings::minRateForStratumSpread for 
    at least ffm_settings::minTimeStepsForStratumSpread time steps to be classified as 
    spreading through the stratum
  */
  const unsigned minTimeStepsForStratumSpread = 2;

  /*!\brief Used as part of the criteria for independent spread in a stratum

    It is a necessary but not sufficient condition for a Stratum fire to be considered independent 
    that the reported flame angle of the stratum fire be less than or equal to the slope 
    plus independentSpreadSensitivity. Ask Phil Zylstra for an explanation of this requirement
  */
  const double independentSpreadSensitivity = 0.17453; //10 degrees

  /*!\brief Minimum temperature for canopy heating

  If the canopy is not heated to ffm_settings::minTempForCanopyHeating by the preheating flames
  then flame residence time is reduced to ffm_settings::reducedCanopyFlameResidenceTime
  */
  const double minTempForCanopyHeating = 100;         //degrees C

  /*!\brief Reduced canopy flame residence time

  If the canopy is not heated to ffm_settings::minTempForCanopyHeating by the preheating flames
  then flame residence time is reduced to ffm_settings::reducedCanopyFlameResidenceTime
  */
  const double reducedCanopyFlameResidenceTime = 1; //sec
    

		       
}


#endif
