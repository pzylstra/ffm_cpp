
#include <cmath>
#include <iostream>

#include "location.h"
#include "line.h"
#include "ray.h"
//#include "geometry.h"
#include "ignition_path.h"
#include "flame.h"
#include "flame_series.h"
#include "ffm_settings.h"
#include "ffm_util.h"
#include "pre_heating_flame.h"
#include "stratum_results.h"
#include "forest_ignition_run.h"


/*!\brief Main computation
  \return A Results object after performing the forest fire computations.
*/
Results Location::results() const {
  
  Results overallResults; 

  //do the ignition computation

  //with canopy
  ForestIgnitionRun fir1 = forestIgnitionRun(true);
  overallResults.addRun(fir1);

  ForestIgnitionRun fir2;
  bool runTwoExists = false;
  //if there is spread in the canopy do it with windfield computed using no canopy
  if (fir1.spreadsInStratum(Stratum::CANOPY)){
    fir2 = forestIgnitionRun(false);
    overallResults.addRun(fir2);
    runTwoExists = true;
  }
  //fir is a reference to the second run if it was done, else the first run
  const ForestIgnitionRun& fir = runTwoExists ? fir2 : fir1;

  //****************** surface results **************************************

  double sws = forest_.windProfile(incidentWindSpeed_, 
                                   forest_.heightForSurfaceWind(), 
                                   !runTwoExists);
  overallResults.surfaceROS(forest_.surface().ros(sws));
  double sfl = forest_.surface().flameLength(sws);
  overallResults.surfaceFlameLength(sfl);
  double sfa = flameAngle(sfl, sws, slope(), firelineLength_);
  overallResults.surfaceFlameAngle(sfa);
  overallResults.surfaceFlameHeight(sfl*sin(sfa) - sfl*cos(sfa)*tan(slope()));

  //****************** stratum results **************************************

  for (const Stratum& strat : forest_.strata()) {
    
    StratumResults stratResults(strat.level());

    //*************** stratum flame lengths *********************************

    //we'll find representative origins at the same time, these are used to 
    //compute flame heights

    double flameLength;
    Pt flameOrigin;
    if (strat.level() != Stratum::CANOPY) {

      double plantFlameLength = ffm_util::cappedMax(fir.laterallyMergedSpeciesWeightedPlantFlameLengths(strat.level(), 
                                                                                         firelineLength_));
      double stratumFlameLength = ffm_util::cappedMax(fir.speciesWeightedFlameLengths(strat.level(), 
                                                                            IgnitionPath::STRATUM_PATH));
      if (plantFlameLength > stratumFlameLength){
        flameLength = plantFlameLength;
        flameOrigin = fir.speciesWeightedOriginOfMaxFlame(strat.level(), IgnitionPath::PLANT_PATH);
      } else {
        flameLength = stratumFlameLength;
        flameOrigin = fir.speciesWeightedOriginOfMaxFlame(strat.level(), IgnitionPath::STRATUM_PATH);
      }
    } else { //the canopy
      double plantFlameLength = ffm_util::cappedMax(fir1.laterallyMergedSpeciesWeightedPlantFlameLengths(strat.level(), 
                                                                                          firelineLength_));
      double stratumFlameLength = ffm_util::cappedMax(fir1.speciesWeightedFlameLengths(strat.level(), 
                                                                             IgnitionPath::STRATUM_PATH));
      if (plantFlameLength > stratumFlameLength){
        flameLength = plantFlameLength;
        flameOrigin = fir1.speciesWeightedOriginOfMaxFlame(strat.level(), IgnitionPath::PLANT_PATH);
      } else {
        flameLength = stratumFlameLength;
        flameOrigin = fir1.speciesWeightedOriginOfMaxFlame(strat.level(), IgnitionPath::STRATUM_PATH);
      }
      
      if (runTwoExists) {
        plantFlameLength = ffm_util::cappedMax(fir2.laterallyMergedSpeciesWeightedPlantFlameLengths(strat.level(), 
                                                                                     firelineLength_));
        stratumFlameLength = ffm_util::cappedMax(fir2.speciesWeightedFlameLengths(strat.level(), 
                                                                        IgnitionPath::STRATUM_PATH));
        if(stratumFlameLength > flameLength)
          flameLength = stratumFlameLength;
        flameOrigin = fir2.speciesWeightedOriginOfMaxFlame(strat.level(), IgnitionPath::STRATUM_PATH);
      }
      if (plantFlameLength > flameLength){
        flameLength = plantFlameLength;
        flameOrigin = fir2.speciesWeightedOriginOfMaxFlame(strat.level(), IgnitionPath::PLANT_PATH);
      } 
    }
    stratResults.flameLength(flameLength);

    //************* stratum flame angles ***********************

    //use the second run if second run is done
    double flameAng;
    double stratumWindSpeed = forest_.windProfile(incidentWindSpeed_, strat.avMidHt(), !runTwoExists);

    if(strat.level() != Stratum::CANOPY)
      //use slope and wind affected flame angles
      flameAng = flameAngle(flameLength, stratumWindSpeed, slope(), firelineLength_);
    else 
      //canopy - use wind affected flame angles
      flameAng = windEffectFlameAngle(flameLength, stratumWindSpeed, slope());

    stratResults.flameAngle(flameAng);


    //************ stratum flame height ************************

    //here we calculate the flame height using the reported flame length and flame angles 
    //and representative flame origins computed above.
    //note this as as per specs but the maximum flame height might not be 
    //achieved by the flame with the maximum flame length, so maybe needs
    //to be changed later
    
    stratResults.flameHeight(flameOrigin.y() + flameLength*sin(flameAng) - 
                             (flameOrigin.x() + flameLength*cos(flameAng))*tan(slope()));
    
    //*********** stratum rate of spread ************************
    
    double stratROS = 0;

    if (strat.level() == Stratum::NEAR_SURFACE) {
      for (const Species& spec : strat.allSpecies()) {
        double specROS = 0;
        //find the plant path if it exists
        auto i = find_if(fir.beginPaths(), fir.endPaths(), 
                         [spec,strat](IgnitionPath ip){return ip.species().sameSpecies(spec) &&
                                                       ip.level() == strat.level() &&
                                                       ip.type() == IgnitionPath::PLANT_PATH;});
        if (i < fir.endPaths()) {
          //weighted sum of plant and surface ros
          double tmp = std::min(1.0,(*i).species().width()/strat.modelPlantSep());
          specROS = tmp * (*i).basicROS() + (1 - tmp)*overallResults.surfaceROS();
        }
        if (fir.spreadsInStratum(strat.level())) {
          //find the stratum path if it exists
          i = find_if(fir.beginPaths(), fir.endPaths(), 
                      [strat,spec](IgnitionPath ip){return ip.species().sameSpecies(spec) &&
                                                    ip.level() == strat.level() &&
                                                    ip.type() == IgnitionPath::STRATUM_PATH;});
          if (i < fir.endPaths())
            //species ROS is the max of those of the plant and stratum
            specROS = std::max(specROS, (*i).basicROS());
        }
        
        //species-weighted sum
        stratROS += specROS * spec.composition();
      } //end loop over strat.allSpecies()
    }

    if (strat.level() == Stratum::ELEVATED || strat.level() == Stratum::MID_STOREY) {
      //if no spread in stratum then ros = 0
      if(!fir.spreadsInStratum(strat.level()))
        stratROS = 0;
      else {
        //is the stratum fire independent? If so, use basic ROS. NOTE that the criterion
        //for independence uses the reported flame angle for strat, which explains why
        //the following code is not contained in ForestIgnitionRun.
        bool independent = false;
        if (stratResults.flameAngle() <= slope() + ffm_settings::independentSpreadSensitivity) {
          //check species-weighted sum of average ros in last two time steps of stratum fire
          double specWeightedSum = 0;
          for (const IgnitionPath& ip : fir.paths()){
            if (ip.type() != IgnitionPath::STRATUM_PATH || ip.level() != strat.level())
              continue;
            if (ip.fullSize())
              specWeightedSum += 0.5*(ip.ros(ip.numSegments() - 1) + ip.ros(ip.numSegments() - 2))*ip.species().composition();
          }
          if (specWeightedSum >= ffm_settings::minRateForStratumSpread)
            independent = true;
        }
        if (independent) 
          stratROS = fir.speciesWeightedBasicROS(strat.level());
        else {
          //computed species weighted max distance travelled in stratum, and species 
          //weighted time
          double distance = 0, time = 0;
          for (const IgnitionPath& ip : fir.paths()) {
            if (ip.type() != IgnitionPath::STRATUM_PATH || ip.level() != strat.level())
              continue;
            if (ip.spreads()){
              distance += ip.maxHorizontalRun()*ip.species().composition();
              time += (ip.startTimeStep() + ip.numSegments())*ffm_settings::computationTimeInterval*
                ip.species().composition();
            }
          }
          stratROS = time > 0 ? distance/time : 0;
        }
      }
    }

    if (strat.level() == Stratum::CANOPY) {

      if (!fir1.spreadsInStratum(Stratum::CANOPY)) 
        stratROS = 0;
      else
        stratROS = std::min(fir1.speciesWeightedBasicROS(Stratum::CANOPY), 
                            fir1.activeCrownFireROS(overallResults));

      if (runTwoExists){
        double stratROS2;
        if (!fir2.spreadsInStratum(Stratum::CANOPY)) 
          stratROS2 = 0;
        else
          stratROS2 = std::min(fir2.speciesWeightedBasicROS(Stratum::CANOPY), 
                               fir2.activeCrownFireROS(overallResults));
        stratROS = std::max(stratROS, stratROS2);
      }
    }

    stratResults.ros(stratROS);

    //********************* Stratum proportion burnt *************************************

    double maxHeight = fir1.speciesWeightedMaxHeightBurnt(strat.level());
    if (runTwoExists)
      maxHeight = std::max(maxHeight, fir2.speciesWeightedMaxHeightBurnt(strat.level()));
    double propBurnt = (maxHeight - strat.avBottom()) / (strat.avTop() - strat.avBottom());
    propBurnt = std::min(1.0,std::max(0.0,propBurnt));
    stratResults.proportionBurnt(propBurnt);

    overallResults.addStratumResults(stratResults);

  } //end of loop over strata

      
  //the rest of the overall results

  //********************** Overall rate of spread **************************************

  //max of the ros of each strata and the surface
  if (overallResults.strataResults().empty())
     overallResults.ros(overallResults.surfaceROS());
  else {
    auto i = max_element(overallResults.strataResultsBegin(), overallResults.strataResultsEnd(), 
                         [](StratumResults sr1, StratumResults sr2){return sr1.ros() < sr2.ros();});
    overallResults.ros(std::max((*i).ros(), overallResults.surfaceROS()));
  }

  //****************** Wind reduction factor *******************************************

  //computed with canopy on
  
  if (incidentWindSpeed_ <= 0)
    overallResults.windReductionFactor(1);
  else
    overallResults.windReductionFactor(incidentWindSpeed_ / 
                                       forest_.windProfile(incidentWindSpeed_, 1.5, true));

  //*********************** Overall flame angle ****************************************

  //flame length weighted sum of all the strata flame angles
  double flameAngleSum = overallResults.surfaceFlameAngle()*overallResults.surfaceFlameLength();
  double flameLengthSum = overallResults.surfaceFlameLength();
  for (const auto& sr : overallResults.strataResults()) {
    flameAngleSum += sr.flameLength()*sr.flameAngle();
    flameLengthSum += sr.flameLength();
  }
  overallResults.flameAngle(flameLengthSum == 0 ? 0 : flameAngleSum / flameLengthSum);

  //********************** Overall flame length ****************************************

  double overallFlameLength = 0;
  //first find mean flame length of combined flames
  if(!fir.combinedFlames().empty()) {
    std::vector<double> flameLengths;
    flameLengths.resize(fir.combinedFlames().size());
    transform(fir.beginCombinedFlames(), fir.endCombinedFlames(), flameLengths.begin(),
              [](Flame f){return f.flameLength() < 0.01 ? 0 : f.flameLength();});
    overallFlameLength = ffm_util::mean(flameLengths,true);
  }
  //overall flame length should be at least as large as the stratum flame lengths
  for (const auto& sr : overallResults.strataResults())
    overallFlameLength = std::max(overallFlameLength, sr.flameLength());

  overallResults.flameLength(overallFlameLength);

  //********************* Overall flame height *****************************************

  //max of the flame heights of each strata and the surface
  if (overallResults.strataResults().empty())
    overallResults.flameHeight(overallResults.surfaceFlameHeight());
  else {
    auto i = max_element(overallResults.strataResultsBegin(), overallResults.strataResultsEnd(), 
                         [](StratumResults sr1, StratumResults sr2){return sr1.flameHeight() < sr2.flameHeight();});
    overallResults.flameHeight(std::max((*i).flameHeight(), overallResults.surfaceFlameHeight()));
  }


  //********************** Scorch heights **********************************************
  
  //McArthur and Luke - McArthur
  overallResults.scorchHeightMcarthur(5.232*pow(overallResults.flameHeight(), 0.7));
  overallResults.scorchHeightLukeMcarthur(6*overallResults.flameHeight());

  //byram intensity
  //R.S. McAlpine - Testing the Effect of Fuel Consumption on Fire Spread Rate
  //Int. J. Wildland Fire 5(3): 143-152,1995
  double byramIntensity = 18000*forest_.surface().fuelLoad()*overallResults.ros();

  //Van Wagner scorch heights - from 
  //Martin E. Alexander, and Miguel G. Cruz
  //Interdependencies between flame length and fireline intensity in predicting crown fire initiation
  //and crown scorch height
  //International Journal of Wildland Fire 2012, 21, 95–113
  overallResults.scorchHeightVanWagner(0.1483*pow(byramIntensity,0.667)); 
  //for wind use second run (no canopy) if it exists, else use first run
  //note conversion to km/h
  double wind =  forest_.windProfile(incidentWindSpeed_, 1.2, !runTwoExists)*3.6;
  overallResults.scorchHeightVanWagnerWithWind(0.74183*pow(byramIntensity,0.667)/
                                               (pow(0.025574*byramIntensity + 0.021433*pow(wind,3),0.5)*
                                                (60 - weather_.airTempC())));

  //********************** Type of crown fire *****************************************

  if (overallResults.strataResults().empty())
    overallResults.crownFireType(Results::UNCLASSIFIED);
  else {
    auto i = find_if(overallResults.strataResultsBegin(), overallResults.strataResultsEnd(),
                     [](StratumResults sr){return sr.level() == Stratum::CANOPY;});
    if (i == overallResults.strataResultsEnd() || (*i).flameLength() < 0.5)
      overallResults.crownFireType(Results::UNCLASSIFIED);
    else
      //using second run if it exists (fir)
      overallResults.crownFireType(fir.spreadsInStratum(Stratum::CANOPY) ? Results::ACTIVE : Results::PASSIVE);
  }

  //********************* Crown run length ********************************************

  if (!fir.spreadsInStratum(Stratum::CANOPY))
    overallResults.crownRunLength(0.0);
  else{
    //want the maximum horizontal run in any canopy stratum path for any species
    double maxRun = 0;
    for (const IgnitionPath& ip : fir.paths()) {
      if (ip.level() != Stratum::CANOPY || ip.type() != IgnitionPath::STRATUM_PATH)
        continue;
      maxRun = std::max(maxRun, ip.maxHorizontalRun());
    }
    overallResults.crownRunLength(maxRun);
  }
  
  //******************** Velocity of crown runs ***************************************

  overallResults.crownRunVelocity(fir.speciesWeightedBasicROS(Stratum::CANOPY));

  //******************** Flame depth **************************************************

  //first do surface
  double maxDepth = forest_.surface().flameResidenceTime() * overallResults.surfaceROS();
  //now iterate over all strata results
  for(const StratumResults& sr : overallResults.strataResults()) {
    if(!fir.spreadsInStratum(sr.level())) continue;
    maxDepth = std::max(maxDepth, 
                        forest_.stratum(sr.level()).avFlameDuration() * sr.ros());
  }
  overallResults.flameDepth(maxDepth);

  return overallResults;

}


/*!\brief Provides a complete description of the ignition of the Forest
\param includeCanopy If false then the canopy layer is left out when 
computing the wind field.
\return A ForestIgnitionRun object which provides a complete description 
of the ignition of the forest, including all plant and stratum 
ignition paths and the time series (vector) of combined flames 
resulting from the ignition of all strata.

The computation proceeds from the surface up through the strata. 
To begin with, the incident flame series and the pre-heating flames 
are initialised using the surface fire characteristics. The 
computation then proceeds in a loop through the strata, beginning 
at the lowest stratum. As each stratum computation has been completed 
the results are used to update the incident flame series and the 
pre-heating flames for use in the calculations for the next stratum.
For each stratum there are two parts to the computation. Initially 
the computation focuses on plant ignition for individual species, 
which is handled by a loop over the species in that stratum. 
For each species, ignition paths through the plant crown are computed 
for each of five ignition scenarios whose initial points are evenly 
spaced along the bottom edge of the crown. These ignition paths are 
computed by calling the computeIgnitionPath() method, and use the 
combined flame series from the surface and lower strata. The 
ignition scenario that produces the greatest maximum flame length is 
chosen as representative for that species, and the corresponding 
plant ignition path is added to the ForestIgnitionRun object being returned. 
The series of flames produced by this “most successful” scenario is 
then reordered in decreasing order of flame length. This procedure 
is carried out for each of the species in the stratum, resulting in an 
ignition path and corresponding re-ordered series of flames for each 
species. These series of flames are weighted according to the 
composition of the species, and combined into a single species-weighted 
plant flame series. 

The second part of the computation involves a second loop over the 
species in the stratum and uses the species weighted plant flame series 
from the first loop as the incident flames to compute stratum ignition 
paths for each species. Again, these are computed using the 
computeIgnitionPath() method. For each species the stratum ignition 
path is added to the ForestIgnitionRun object being returned, and the 
series of flames produced by this ignition path is reordered in 
decreasing order of flame length. These flame series from each 
species are weighted by species composition and combined to produce a 
single species-weighted stratum flame series.
The species-weighted plant flame series and species-weighted stratum 
flame series are compared. The series with the greater maximum 
flame length is selected as that which will represent the stratum 
fire, which will be used to estimate the pre-heating component 
due to this stratum, and which will be combined with those from the 
lower strata to produce the incident flames for plant ignition scenarios 
for the next stratum.

The computation then proceeds to the next highest stratum, and the process is repeated.
*/
ForestIgnitionRun Location::forestIgnitionRun(const bool& includeCanopy) const {

  const bool PLANT_IGNITION_RUN = true;

  //ignition paths for species and strata. This is what will be returned by this method
  ForestIgnitionRun ignitionRun(forest()); 
  ignitionRun.type(includeCanopy ? ForestIgnitionRun::WITH_CANOPY : ForestIgnitionRun::WITHOUT_CANOPY);

  //initialise surface flame timeseries with surface flame characteristics
  double surfWindSpeed = forest_.windProfile(incidentWindSpeed_, forest_.heightForSurfaceWind(), includeCanopy);
  double surfFlameLength = forest_.surface().flameLength(surfWindSpeed);
  double surfFlameAngle = flameAngle(surfFlameLength, surfWindSpeed, slope(), firelineLength_);
  double sfrt = forest_.surface().flameResidenceTime();
  std::vector<Flame> surfaceFlames(static_cast<int>(round(sfrt/ffm_settings::computationTimeInterval)), 
                                   Flame(surfFlameLength,surfFlameAngle,Pt(0,0), 0,ffm_settings::mainFlameDeltaTemp));

  //initialise vector of pre-heating flames 
  //Note this is a small vector no bigger than 4, one element for each layer that contributes to 
  //the preheating
  std::vector<PreHeatingFlame> preHeatingFlames;
  double cumulativePreHeatingStartTime = 0;
  preHeatingFlames.push_back(PreHeatingFlame(Stratum::SURFACE, 
                                             Flame(surfFlameLength,surfFlameAngle,
                                                   Pt(0,0),0,ffm_settings::mainFlameDeltaTemp),
                                             cumulativePreHeatingStartTime, 
                                             sfrt));
    
  //initialise preHeatingEndTime to -1 which means that duration will be limited only by the 
  //end times of each individual preHeatingFlame. preHeatingEndTime will be set to the 
  //start time of direct heating as computation progresses through the strata
  double preHeatingEndTime = -1;

  //vector of FlameSeries for species weighted stratum flames. These together with the surface
  //flames form the incident flames for the plant ignition sequence calculation
  std::vector<FlameSeries> allSpeciesWeightedStratumFlameSeries;

  //if a level exists in the vector flameConnections then a flame connection with 
  //higher strata is guaranteed from this level, else have to look
  //at Forest::StrataOverlaps etc. 
  std::vector<Stratum::LevelType> flameConnections;

  //loop over the strata 
  for ( const auto& strat : strata() ) {

    //compute incident flames for this strata from surface flames and strata flames from lower strata
    //only include lower strata that have a conection with strat, ie vertical association etc
    std::vector<Flame> incidentFlames(surfaceFlames);
    
    if (!allSpeciesWeightedStratumFlameSeries.empty()) {

      //first we need to compute a flame-weighted wind for use in the flame combination
      double flameSum = surfaceFlames.at(0).flameLength();
      double flameWeightedWind = surfWindSpeed*flameSum;
      for (const auto& s : strata()) {
        if (s >= strat) break; 
        //find the flameseries from stratum s
        auto iter = find_if(allSpeciesWeightedStratumFlameSeries.begin(), 
                            allSpeciesWeightedStratumFlameSeries.end(), 
                            [s](FlameSeries fs){return fs.level() == s.level();});
        if (iter == allSpeciesWeightedStratumFlameSeries.end()) 
          continue; //no output for stratum s
        //if there is a connection then add the winds
        if (find(flameConnections.begin(), flameConnections.end(), s.level()) < flameConnections.end() ||
            forest_.verticalAssociation(s.level(),strat.level())) {
          flameWeightedWind += (*iter).cappedMaxFlameLength() * 
            forest_.windProfile(incidentWindSpeed_, s.avMidHt(), includeCanopy);
          flameSum += (*iter).cappedMaxFlameLength();
        }
      }
      flameWeightedWind = flameSum > 0 ? flameWeightedWind / flameSum : 0; 

      //now compute the combined flames
      for (const auto& s : strata()) {
        if (s >= strat) break; 
        //find the flameseries for stratum s
        auto iter = find_if(allSpeciesWeightedStratumFlameSeries.begin(), 
                            allSpeciesWeightedStratumFlameSeries.end(), 
                            [s] (FlameSeries fs) {return fs.level() == s.level();});
        if (iter == allSpeciesWeightedStratumFlameSeries.end()) 
          continue; //no FlameSeries for stratum s
        //if there is a connection then combine the flames
        if (find(flameConnections.begin(), flameConnections.end(), s.level()) < flameConnections.end() ||
            forest_.verticalAssociation(s.level(),strat.level())) {
          //combine the flames
          incidentFlames = combineFlames(incidentFlames, (*iter).flames(),
                                         flameWeightedWind,
                                         slope(),
                                         firelineLength_);
        }
      }
    }

    //initialise (with zeros) vectors to hold plant flame lengths depth ignited, origins and temperatures. 
    std::vector<double> speciesWeightedFlameLengths(ffm_settings::maxTimeSteps,0);
    std::vector<double> speciesWeightedFlameDepths(ffm_settings::maxTimeSteps,0);
    std::vector<Pt> speciesWeightedFlameOrigins(ffm_settings::maxTimeSteps,Pt(0,0));
    std::vector<double> speciesAndFlameWeightedFlameTemps(ffm_settings::maxTimeSteps,0);

    //initialise Pt that will hold the species weighted sum of flame origins of the largest flame from the 
    //most influential of the scenarios for each species
    Pt speciesWeightedFlameOrigin(0,0);

    //double to hold species weighted ignition time. This is used for pre-heating calc
    double speciesWeightedIgnitionTime = 0;

    double stratumWindSpeed = forest_.windProfile(incidentWindSpeed_, strat.avMidHt(), includeCanopy);

    //bool for whether connection 
    bool connection = false;

    //first loop over the species - plant ignition
    for (const Species& spec : strat.allSpecies()) {

      double comp = spec.composition();

      //initialise ignition path for this species. This will eventually hold the ignition 
      //path for the most influential scenario
      IgnitionPath speciesIgnitionPath;

      //loop over ignition point scenarios
      for (int ignitPtScenario = -2; ignitPtScenario <= 2; ++ignitPtScenario) {

        //compute scenario initial ignition point, make sure this is not below the surface
        Pt iPt = spec.crown().pointInBase( 0.25*ignitPtScenario*spec.width());
        if (iPt.y() < iPt.x()*tan(slope()))
          iPt = Pt(iPt.x(), iPt.x()*tan(slope()));

        //compute ignition path for this scenario
        IgnitionPath iPath = computeIgnitionPath(incidentFlames, 
                                                 PLANT_IGNITION_RUN, 
                                                 preHeatingFlames, 
                                                 preHeatingEndTime, 
                                                 strat.level(), 
                                                 spec,
                                                 0,
                                                 stratumWindSpeed, 
                                                 iPt);

        if (iPath.hasSegments()) {  
          // ignition occurred

          if (speciesIgnitionPath.hasSegments()) {
            // there was a prior path with ignition...
            // check to see if this ignition scenario has a longer flame length than any of the previous ones. 
            // and replace stored IgnitionPath if necessary. Note we compare segment lengths
            if (ffm_numerics::gt(iPath.maxSegmentLength(), speciesIgnitionPath.maxSegmentLength())) 
              speciesIgnitionPath = iPath;

          } else { 
            // no prior path with ignited segments so save this one
            speciesIgnitionPath = iPath;
          }

        } else { 
          // ignition did not occur...
          // if the prior path also didn't have ignition, keep the one with the max temperature 
          if (!speciesIgnitionPath.hasSegments() && ffm_numerics::gt(iPath.maxPreIgnitionTemp(), speciesIgnitionPath.maxPreIgnitionTemp())) {
            speciesIgnitionPath = iPath;
          }
        }

      }//end of loop over ignition scenarios
      
      //add the plant ignition path to ignitionRun
      if (speciesIgnitionPath.hasSegments() || speciesIgnitionPath.hasPreIgnitionData()) {
        ignitionRun.addPath(speciesIgnitionPath);
        
        if (speciesIgnitionPath.hasSegments()) {
          //add component of species weighted ignition time 
          speciesWeightedIgnitionTime += (speciesIgnitionPath.timeToIgnition() + 
              speciesIgnitionPath.timeIgnitionToMaxFlame()) * comp;
          //order the vector of ignited segments according to flamelength
          speciesIgnitionPath.sortSegments();

          //loop over segments and add species-weighted flame information
          unsigned i = 0;
          for (const Seg& sg : speciesIgnitionPath.ignitedSegments()){
            if (ffm_numerics::leq(sg.length(), 0))
              break; //because the segments are sorted max to min
            double tmpFlameLength = spec.flameLength(sg.length());
            //if flame from any segment burns out past the edge of the plant then set connection to true
            if (!connection && 
                sg.start().x() + 
                tmpFlameLength*cos(windEffectFlameAngle(tmpFlameLength, stratumWindSpeed, slope()) > 0.5*spec.width())
               )
              connection = true;
            //add (species weighted) flame info to vector of flame lengths
            speciesWeightedFlameLengths.at(i) += comp*tmpFlameLength;
            speciesWeightedFlameDepths.at(i) += comp*sg.length();
            speciesWeightedFlameOrigins.at(i) += comp*sg.start();
            speciesAndFlameWeightedFlameTemps.at(i) += comp*tmpFlameLength *
              (spec.isGrass() && strat.level() == Stratum::NEAR_SURFACE ? 
               ffm_settings::grassFlameDeltaTemp : ffm_settings::mainFlameDeltaTemp);
            ++i;
          }

          //add the composition weighted origin from the largest (first after sorting) flame length
          speciesWeightedFlameOrigin += comp*speciesIgnitionPath.ignitedSegments().front().start();
        }
      }

    } //end of first loop over species - plant ignition

    //If there is a non-trivial vector of species-weighted flame data from the plant ignition 
    //calculation then we continue on to the stratum ignition calc
    //note that the flame lengths were sorted so we only check the first one
    if (!ffm_numerics::leq(speciesWeightedFlameLengths.front(),0)) {
      //finish the flame weighting
      unsigned i = 0;
      for (auto& temp : speciesAndFlameWeightedFlameTemps) {
        if (ffm_numerics::leq(speciesWeightedFlameLengths.at(i),0))
          break; //because the segments were ordered max to min
        temp /= speciesWeightedFlameLengths.at(i);
        ++i;
      }

      //do lateral merging of combined flame timeseries (Eq 5.74, 5.75) eg Fuels!G132
      for (double& fl : speciesWeightedFlameLengths) {
        if (ffm_numerics::leq(fl,0)) break; //because ignited segments were ordered max to min
        fl = laterallyMergedFlameLength(fl, firelineLength_, strat.avWidth(), strat.modelPlantSep());
      }

      //make a FlameSeries to hold species-weighted plant flames
      FlameSeries speciesWeightedPlantFlames(strat.level()); 
      for (unsigned i = 0; i < (ffm_settings::maxTimeSteps); ++i) {
        if (ffm_numerics::leq(speciesWeightedFlameLengths.at(i),0)) break; //because ordered max to min
        speciesWeightedPlantFlames.addFlame(Flame(speciesWeightedFlameLengths.at(i),
                                                  windEffectFlameAngle(speciesWeightedFlameLengths.at(i), 
                                                                       stratumWindSpeed, slope()),
                                                  speciesWeightedFlameOrigins.at(i), 
                                                  speciesWeightedFlameDepths.at(i),
                                                  speciesAndFlameWeightedFlameTemps.at(i)));
      }
   
      //accumulate pre heating start time which is determined by the plant ignition sequences
      cumulativePreHeatingStartTime += speciesWeightedIgnitionTime;
      //set preHeatingEndTime for the next pass through strata loop
      //the current strata will supply direct heating to the next 
      //strata so the preheating will stop at this time. 
      preHeatingEndTime = cumulativePreHeatingStartTime;

      //if strat is canopy then compute distance out along stratum for which bottom edge of canopy is heated
      double canopyHeatingDistance = 0;
      if (strat.level() == Stratum::CANOPY) {
        double originOffsetX = 0;
        //line to represent the bottom of the canopy
        Line canopyLine(Pt(0,strat.avBottom()), slope());

        //loop over the all the flame series. NOTE: we don't need to start with the surface, and the 
        //flameseries are in order bottom stratum to top
        for (std::vector<FlameSeries>::const_iterator it = allSpeciesWeightedStratumFlameSeries.begin();
             it != allSpeciesWeightedStratumFlameSeries.end();
             ++it) {
          Flame f = (*it).flames().front(); //first and largest flame because flameseries were sorted
          Ray plume = f.plume();

          //if the plume is hot enough at the point where it intersects bottom of the canopy then 
          //update the canopyHeatingDistance
          Pt intersectionPt;
          plume.intersects(canopyLine, intersectionPt);
          if (f.plumeTemperature(intersectionPt, weather().airTempC()) >= ffm_settings::minTempForCanopyHeating)
            canopyHeatingDistance = std::max(canopyHeatingDistance, intersectionPt.x() + originOffsetX); 
          
          //now update the offset according to where plume intersects the next stratum
          //NOTE that if next stratum is the canopy then intersectionPt was found above
          if (it != allSpeciesWeightedStratumFlameSeries.end() - 1) //the next stratum is not the canopy
            plume.intersects(Line(Pt(0,forest_.stratum((*(it + 1)).level()).avBottom()), slope()), 
                             intersectionPt);

          originOffsetX += intersectionPt.x();
        }
        canopyHeatingDistance -= originOffsetX;   
      } 

      //reset vectors that hold species weighted flame information
      speciesWeightedFlameLengths = std::vector<double>(ffm_settings::maxTimeSteps,0);
      speciesWeightedFlameDepths = std::vector<double>(ffm_settings::maxTimeSteps,0);
      speciesAndFlameWeightedFlameTemps = std::vector<double>(ffm_settings::maxTimeSteps,0);
      speciesWeightedFlameOrigins = std::vector<Pt>(ffm_settings::maxTimeSteps, Pt(0,0));
    
      //second loop over species - stratum ignition
      for (const auto& spec : strat.allSpecies()) {

        double comp = spec.composition();

        //make a stratum polygon. 
        std::vector<Pt> verts;
        double xx = strat.modelPlantSep() - 0.5*strat.avWidth();
        verts.push_back(Pt(xx, strat.avTop() + xx*tan(slope())));
        verts.push_back(Pt(xx, strat.avBottom() + xx*tan(slope())));      
        xx += 10000;
        verts.push_back(Pt(xx, strat.avBottom() + xx*tan(slope())));      
        verts.push_back(Pt(xx, strat.avTop() + xx*tan(slope())));
        Poly stratumPoly(verts);

        //make a species based on this polygon
        Species bigSpecies(comp,
                           spec.name(),
                           stratumPoly,
                           spec.liveLeafMoisture(),
                           spec.deadLeafMoisture(),
                           spec.propDead(),
                           spec.silFreeAshCont(),
                           spec.ignitTemp(),
                           spec.leafForm(),
                           spec.leafThick(),
                           spec.leafWidth(),
                           spec.leafLength(),
                           spec.leafSep(),
                           spec.stemOrder(),
                           spec.width(),
                           std::max(spec.clumpSep(), strat.modelPlantSep() - strat.avWidth()));
      
        //initialise an ignition path for the stratum ignition
        IgnitionPath speciesIgnitionPath;

        //compute the initial point for the ignition
        Ray r(speciesWeightedFlameOrigin, speciesWeightedPlantFlames.flames().front().angle());
        Pt iPt;
        if (!r.intersects(stratumPoly, iPt)) {
          ;//what happens here? nothing, we are left with an empty ignition path
        } else {
          //compute ignition path, note no preheating here
          speciesIgnitionPath = computeIgnitionPath(speciesWeightedPlantFlames.flames(),
                                                    !PLANT_IGNITION_RUN,
                                                    std::vector<PreHeatingFlame>(),
                                                    0,
                                                    strat.level(),
                                                    bigSpecies, 
                                                    canopyHeatingDistance,
                                                    stratumWindSpeed,
                                                    iPt); 
        }


        if (speciesIgnitionPath.hasSegments()) {
          ignitionRun.addPath(speciesIgnitionPath);

          //add component of species weighted ignition time 
          // speciesWeightedIgnitionTime += (speciesIgnitionPath.timeToIgnition() + 
          //                              speciesIgnitionPath.timeIgnitionToMaxFlame()) * comp;
          //order the vector of ignited segments according to flamelength
          speciesIgnitionPath.sortSegments();

          int i = 0;
          for (const Seg& sg : speciesIgnitionPath.ignitedSegments()){
            double flameLen = spec.flameLength(sg.length());
            speciesWeightedFlameLengths.at(i) += comp*flameLen;
            speciesWeightedFlameDepths.at(i) += comp*sg.length();
            speciesAndFlameWeightedFlameTemps.at(i) += comp*flameLen*
              (spec.isGrass() && strat.level() == Stratum::NEAR_SURFACE ? 
               ffm_settings::grassFlameDeltaTemp : ffm_settings::mainFlameDeltaTemp);
            speciesWeightedFlameOrigins.at(i) += comp*sg.start();
            ++i;
          }
        }

      }//end of second loop over species - stratum ignition

      //finish the flame weighting
      i = 0;
      for (auto& temp :  speciesAndFlameWeightedFlameTemps) {
        if (ffm_numerics::almostZero(speciesWeightedFlameLengths.at(i)))
          break; //because the segments were ordered max to min
        temp /= speciesWeightedFlameLengths.at(i);
        ++i;
      }

      //make a species weighted flame series from the stratum calculations. This flame series will 
      //become part of the incident flames for the next stratum
      FlameSeries speciesWeightedStratumFlames(strat.level()); 
      for (unsigned i = 0; i < (ffm_settings::maxTimeSteps); ++i) {
        if (ffm_numerics::leq(speciesWeightedFlameLengths.at(i),0)) break; //because ordered max to min
        speciesWeightedStratumFlames.addFlame(Flame(speciesWeightedFlameLengths.at(i),
                                                    windEffectFlameAngle(speciesWeightedFlameLengths.at(i), 
                                                                         stratumWindSpeed, slope()),
                                                    speciesWeightedFlameOrigins.at(i), 
                                                    speciesWeightedFlameDepths.at(i),
                                                    speciesAndFlameWeightedFlameTemps.at(i)));
      }
      //compare maximum flame lengths from the species weighted plant flames and the 
      //species weighted stratum flames to determine which of these will be used to
      //form the incdent flames for the higher strata
      const FlameSeries& speciesWeightedFlames = (!speciesWeightedStratumFlames.isNull() && 
                                                  speciesWeightedStratumFlames.flames().at(0).flameLength() >= 
                                                  speciesWeightedPlantFlames.flames().at(0).flameLength() 
                                                  ?
                                                  speciesWeightedStratumFlames : speciesWeightedPlantFlames);
      //push the species weighted flame series onto the vector from which the incident flames will be 
      //computed for the next stratum 
      allSpeciesWeightedStratumFlameSeries.push_back(speciesWeightedFlames);
      //add the relevant preheating info to the vector of pre heating flames. Note that the start
      //time of the preheating flame is computed from the plant ignition sequences whereas the 
      //preheating flame itself comes from the stratum ignition sequences
      double mfl = speciesWeightedFlames.meanFlameLength();
      PreHeatingFlame phf = PreHeatingFlame(strat.level(),
                                                 Flame(mfl,
                                                       windEffectFlameAngle(mfl,stratumWindSpeed,slope()),
                                                       speciesWeightedFlames.meanOrigin(),
                                                       speciesWeightedFlames.meanDepthIgnited(),
                                                       speciesWeightedFlames.meanDeltaTemperature()),
                                                 cumulativePreHeatingStartTime,
                                                 cumulativePreHeatingStartTime 
                                                 + speciesWeightedFlames.nonNullCount()
                                                 *ffm_settings::computationTimeInterval);
      
      preHeatingFlames.push_back(phf);
      //check whether largest (ie first after sorting) species weighted stratum flame is longer than
      //the largest species weighted plant flame and if so set connection. This is part of the (strange)
      //calculation from the spreadsheet to determine whether or not a stratum can ignite an upper stratum 
      //see for example Fuels!B266 and B269
      if (!connection && !speciesWeightedStratumFlames.isNull() &&
          speciesWeightedStratumFlames.flames().at(0).flameLength() > 
          speciesWeightedPlantFlames.flames().at(0).flameLength())
        connection = true;
    }//end of stratum ignition computation

    if (connection) 
      flameConnections.push_back(strat.level());

  } //end of loop over strata_


  //compute combined flames for all strata, including canopy, from surface flames and strata flames
  //only include lower strata that have a conection with canopy, ie vertical association etc
  std::vector<Flame> allStrataCombinedFlames(surfaceFlames);
    
  if (!allSpeciesWeightedStratumFlameSeries.empty()) {

    //first we need to compute a flame-weighted wind for use in the flame combination
    double flameSum = surfaceFlames.at(0).flameLength();
    double flameWeightedWind = surfWindSpeed*flameSum;
    for (const auto& s : strata()) {
      //find the flameseries from stratum s
      auto i = find_if(allSpeciesWeightedStratumFlameSeries.begin(), 
                       allSpeciesWeightedStratumFlameSeries.end(), 
                       [s](FlameSeries fs){return fs.level() == s.level();});
      if (i == allSpeciesWeightedStratumFlameSeries.end()) 
        continue; //no output for stratum s
      //if there is a connection then add the winds
      if (find(flameConnections.begin(), flameConnections.end(), s.level()) < flameConnections.end() ||
          forest_.verticalAssociation(s.level(),Stratum::CANOPY) ||
          s.level() == Stratum::CANOPY) {
        flameWeightedWind += (*i).cappedMaxFlameLength() * 
          forest_.windProfile(incidentWindSpeed_, s.avMidHt(), includeCanopy);
        flameSum += (*i).cappedMaxFlameLength();
      }
    }
    flameWeightedWind = flameSum > 0 ? flameWeightedWind / flameSum : 0; 

    //now compute the combined flames
    for (const auto& s : strata()) {
      //find the flameseries for stratum s
      auto i = find_if(allSpeciesWeightedStratumFlameSeries.begin(), 
                       allSpeciesWeightedStratumFlameSeries.end(), 
                       [s] (FlameSeries fs) {return fs.level() == s.level();});
      if (i == allSpeciesWeightedStratumFlameSeries.end()) 
        continue; //no FlameSeries for stratum s
      //if there is a connection then combine the flames
      if (find(flameConnections.begin(), flameConnections.end(), s.level()) < flameConnections.end() ||
          forest_.verticalAssociation(s.level(),Stratum::CANOPY) ||
          s.level() == Stratum::CANOPY) {
        //combine the flames
        allStrataCombinedFlames = combineFlames(allStrataCombinedFlames, (*i).flames(),
                                                flameWeightedWind,
                                                slope(),
                                                firelineLength_);
      }
    }
  }
  ignitionRun.combinedFlames(allStrataCombinedFlames);
  return ignitionRun;
}

/*!\brief Computes an IgnitionPath through the leaf crown of a Species
  \param incidentFlames This is the vector (time series) of flames that will 
  be applied to the crown of the Species spec. In the case plantFlameRun == true, 
  ie plant ignition, incidentFlames will be a combination of flames that come 
  from surface computations and flames that come from lower strata. If 
  plantFlameRun == false, ie stratum ignition, then the incident flames are 
  derived from the plant ignition run which will have already been done.
  \param plantFlameRun If true then the method is computing a plant ignition path. 
  If false then a stratum ignition path is being computed
  \param preHeatingFlames The pre-heating flames are derived from averaged values of 
  flames in the surface and lower strata. They produce heating that occurs before the 
  direct heating from the stratum immediately below.
  \param preHeatingEndTime The pre-heating ceases at preHeatingEndTime, the time 
  that the direct heating starts. This may be earlier than the end time of any or 
  all of the pre-heating flames.
  \param level The Stratum::LevelType in which the ignition path is being computed.
  \param spec This is the Species object through which the ignition path is being computed. 
  In the case that this is a stratum ignition run then spec will be a Species object 
  with a modified crown and a modified clump separation attribute, and will represent 
  a stratum made up of this single species.
  \param canopyHeatingDistance If plantFlameRun == false and level == CANOPY then 
  the flame residence time is reduced to ffm_settings::reducedCanopyFlameResidenceTime 
  for points whose x-coordinate exceed canopyHeatingDistance.
  \param windSpeed The wind speed for the computation. Typically this is the wind speed 
  at mid height of the stratum under consideration.
  \param initialPt A point on the boundary of the crown of spec which forms the initial 
  point of the sequence of ignited segments that is being computed.
  \return An ignitionPath object representing the path of ignition through spec

The computation produces a sequence (vector) of ignited segments in the crown of spec.
It loops over an indeterminate number of time steps, but no more than ffm_settings::maxTimeSteps 
steps after ignition first occurs. At each time step after initial ignition, the 
computation produces a segment representing the part of the crown that is burning at that
time. The computation ends when the plant flames and incident flames are both extinguished, 
when the ignition path has burnt completely through the crown, or when
ffm_settings::maxTimeSteps steps have elapsed. At any time step there are two sources of 
heating being applied to the crown: the appropriate element of the incidentFlames and the 
plant flame that was computed at the previous time step. The computation determines, 
at each time step, a potential path of ignition through the crown beginning at the end 
point of the segment computed in the previous time step. This path is determined 
by the incident and the plant flames, and is chosen so as to maximise the length of 
the segment of the crown that can potentially ignite during the time step. The 
length of this potentially ignitable segment is computed using the ignition temperature 
of spec, flame angles, the geometry of the crown, and plume temperature calculations. 
However the portion of this segment that will actually ignite during the current time 
step will also depend on the ignition delay time of the species, as well as the drying 
that has occurred as a result of the pre-heating flames and the incident and plant flames 
from the previous time steps. To compute the distance along the potential ignition path 
for which ignition occurs during the current time step, the path is divided into 
ffm_settings::numPenetrationSteps equal segments, and the end point (the test point) of 
each of these segments is tested for ignition during that time step. The test point is 
subjected to drying from the preHeatingFlames flames, from the incidentFlames, and from the 
plant flames from previous time steps. The magnitude of the drying, along with the ignition 
delay time at the test point, determines whether or not ignition occurs during that time step.
Drying is conceptualised as a multiplicative reduction factor applied to the ignition delay 
time of the species. If a point within the crown is subjected to a temperature T for a time t 
then this produces a drying factor of 1 - t/idt(T) where idt(T) is the ignition delay time 
of the species at temperature T. This is applied repeatedly at the test point for the 
preHeatingFlames, the incidentFlames and the plant flames, and the resultant drying factor 
is the product of each of these results. The test point is said to ignite when the product 
of its ignition delay time and the drying factor is less than the length of a time step, 
ffm_settings::computationTimeInterval. If the test point is found to ignite during the 
time step then the next of the test points along the potential ignition path is tested 
in the same fashion, and so on. The distance along the potential ignition path that is 
actually ignited, and therefore the end point of the ignited segment for that time step, 
is so determined. The start point of the ignited segment depends on the flame residence 
time and the previously computed segments.
Having computed the ignited segment, the computation proceeds to the next time step. 
It ceases when ffm_settings::maxTimeSteps have elapsed, when both the incident and plant 
flames have extinguished, or when there is no further potential path of ignition through 
the plant, ie when the ignition path has reached the top or side of the plant.
*/
IgnitionPath Location::computeIgnitionPath(const std::vector<Flame>& incidentFlames,
                                           const bool& plantFlameRun,
                                           std::vector<PreHeatingFlame> preHeatingFlames,
                                           const double& preHeatingEndTime,
                                           const Stratum::LevelType& level,
                                           const Species& spec,
                                           const double& canopyHeatingDistance,
                                           const double& windSpeed,
                                           const Pt& initialPt) const { 

  //initialise ignition path 
  IgnitionPath iPath(plantFlameRun ? IgnitionPath::PLANT_PATH : IgnitionPath::STRATUM_PATH,
                     level, spec, -99);

  //pop the last pre-heating flame off the vector of pre-heating flames because that level will
  //provide the direct heating
  if(!preHeatingFlames.empty())
    preHeatingFlames.pop_back();

  //initialise a vector to hold plant flames 
  //this saves having to recompute them from iPath multiple times
  std::vector<Flame> plantFlames;
  plantFlames.reserve(ffm_settings::maxTimeSteps);

  bool isGrass = spec.isGrass() && level == Stratum::NEAR_SURFACE;
  Pt iPt = initialPt;
  bool ignition = false;
  int safetyCounter = 1;

  //we loop over an indeterminate number of time steps, but no more 
  //than maxTimeSteps counted from when ignition occurs. This is why
  //we use safetyCounter instead of timeStep in the loop condition. 

  for (int timeStep = 1; safetyCounter <= (ffm_settings::maxTimeSteps) ; ++timeStep) {

    //for plant flame, and only if required, we modify wind speed by 
    //reducing it by speed of flame progression
    double modifiedWindSpeed = windSpeed;
    if (!plantFlameRun && iPath.hasSegments()) {
      int sz = iPath.numSegments();
      if (sz == 1)
        modifiedWindSpeed = windSpeed - 
          std::max(0.0,
                   iPath.ignitedSegment(0).end().x() - initialPt.x())/ffm_settings::computationTimeInterval;
      else
        modifiedWindSpeed = windSpeed - 
          std::max(0.0,iPath.ignitedSegment(sz-1).end().x() - iPath.ignitedSegment(sz-2).end().x())/
          ffm_settings::computationTimeInterval;
    }

    //get plant flame from previous time step
    Flame plantFlame = plantFlames.empty() ? Flame() : plantFlames.back();
    //get incident flame
    Flame incidentFlame = timeStep <= incidentFlames.size() ? incidentFlames.at(timeStep - 1) : Flame();

    //if no plant flame or incident flame decide what to do
    //we allow for the possibility that incident flame may flare up later
    //though this is not possible under current model because of reordering
    if (plantFlame.isNull() && incidentFlame.isNull()) {
      if (timeStep <= incidentFlames.size())
        continue; 
      else
        break;
    }

    //compute potential ignition distance for plant flame 
    double maxPlantPath = 0;
    if (!plantFlame.isNull()) {
      Ray r(iPt, plantFlame.angle());
      double intsctLen = r.intersectionLength(spec.crown());
      double ignitLen = plantFlame.inversePlumeTemperature(spec.ignitionTemp(),weather_.airTempC());
      maxPlantPath = std::min(r.intersectionLength(spec.crown()),
                              plantFlame.inversePlumeTemperature(spec.ignitionTemp(),weather_.airTempC()));
    }

    //compute incident flame characteristics. maxIncidentPath is the length of the segment in the 
    //plant that could potentially be ignited by the incident flame, based on iPt, geometry of the
    //plant crown, plume temperature and plant ignition temperature
    double maxIncidentPath = 0;
    Pt incidentFlameOrigin;
    Line surfaceLine(Pt(0,0), slope());
    if (!incidentFlame.isNull()) {
      if (plantFlameRun) {
        //if possible set the incidentFlameOrigin to a point on the surface such that a ray starting
        //at that point with angle incidentFlame.angle() will pass through iPt. If this is not possible
        //return false, but this should never occur because flame angles are always greater than the slope.
        if (!surfaceLine.originOnLine(incidentFlame.angle(), iPt, incidentFlameOrigin))
          ; //this should not happen 
      } else
        incidentFlameOrigin = incidentFlame.origin();
      Ray r(iPt, incidentFlame.angle());
      double pathDistance = r.intersectionLength(spec.crown());
      double ignitionDistance = 
        std::max(0.0,
                 incidentFlame.inversePlumeTemperature(spec.ignitionTemp(),weather_.airTempC()) 
                 - (iPt - incidentFlameOrigin).norm());
      maxIncidentPath = std::min(pathDistance, ignitionDistance);
    } 

    //if there is any more of the plant to burn then compute ignition for the next time step
    Pt ePt = iPt;
    if (ffm_numerics::gt(maxIncidentPath, 0.0) || ffm_numerics::gt(maxPlantPath, 0.0)) {
      //the direction and max possible extent of the next ignition segment is determined by 
      //whichever path has the greatest length
      double pathLength = maxPlantPath > maxIncidentPath ? maxPlantPath : maxIncidentPath;
      double pathAngle = maxPlantPath > maxIncidentPath ? plantFlame.angle() : incidentFlame.angle();

      int i = 0;
      for (auto& phf : preHeatingFlames) {
        if (!phf.flame().isNull()) {
          Pt originPt;
          Line tmpLine(phf.flame().origin(), slope());
          if (!tmpLine.originOnLine(phf.flame().angle(), ePt, originPt))
            ; 

	  // This line does not update the PreHeatingFlame's Flame field as intended. 
	  // Instead it is modifying a copy which is then silently discarded, leaving 
	  // the origin point at (0,0).
	  //
          //preHeatingFlames.at(i).flame().origin(originPt);

	  // Hack fix for above problem: create a new PreHeatingFlame object - MB
	  Flame flame = phf.flame();
	  flame.origin(originPt);
          preHeatingFlames.at(i) = PreHeatingFlame(phf.level(), flame, phf.startTime(), phf.endTime());
        }
	i++ ;
      }
      //the possible ignition distance is divided into numPenetrationSteps segments and we test each
      //segment in turn for ignition
      for(int step = 1; step <= ffm_settings::numPenetrationSteps; ++step) {
        Pt testPt = ePt + (pathLength/ffm_settings::numPenetrationSteps)*Pt(cos(pathAngle),sin(pathAngle));
        double dryingFactor = 1;
        double dryingTemp;

        //compute the drying at testPt from preheating flames. NOTE that we have already popped the last
        //of the preheating flames off the vector because that level will provide the direct heating
        for (const auto& phf : preHeatingFlames){
          if (!phf.flame().isNull()) {
            //compute the temperature at the test point from the drying flame
            double distToFlame = (testPt - phf.flame().origin()).norm();
            dryingTemp = phf.flame().plumeTemperature(distToFlame, weather_.airTempC());
            //compute the IDT at the test point
            double idt = spec.ignitionDelayTime(dryingTemp) * 
              (isGrass ? ffm_settings::grassIDTReduction : 1.0);
            
            double duration = phf.duration(preHeatingEndTime);
            dryingFactor *= std::max(0.0, 1 - duration / idt);

            if (iPt == initialPt && timeStep == 1 && step == 1) {
              iPath.addPreIgnitionData( PreIgnitionData::preheating(
                                      phf.flame().flameLength(), phf.flame().depthIgnited(), 
                                      distToFlame, dryingFactor, dryingTemp, duration) );
            }
          }

          if (dryingFactor <= 0) 
            break; 
        }

        //drying from the incident flames, but only if testPt is not already completely dry
        if (dryingFactor > 0) {
          int numDryingSteps = std::min(timeStep - 1, static_cast<int>(incidentFlames.size()));
          for(int i = 1; i <= numDryingSteps; ++i){
            Flame dryingFlame = incidentFlames.at(i - 1);
            if (dryingFlame.isNull()) continue;
            Pt dryingFlameOrigin;
            if (plantFlameRun) {
              if (!surfaceLine.originOnLine(dryingFlame.angle(), iPt, dryingFlameOrigin))
                ; //this should not happen 
            }   else
              dryingFlameOrigin = dryingFlame.origin();
            dryingTemp = dryingFlame.plumeTemperature((testPt - dryingFlameOrigin).norm(), weather_.airTempC());
            double dryingIDT = spec.ignitionDelayTime(dryingTemp)* 
              (isGrass ? ffm_settings::grassIDTReduction : 1.0);
            dryingFactor *= std::max(0.0,1 - ffm_settings::computationTimeInterval / dryingIDT);
          }
        }

        //drying from the plant flames, but only if testPt is not already completely dry
        if (dryingFactor > 0) {
          for (const Flame& dryingFlame : plantFlames){
            if (dryingFlame.isNull()) continue;
            dryingTemp = dryingFlame.plumeTemperature(testPt, weather_.airTempC());
            double dryingIDT = spec.ignitionDelayTime(dryingTemp)* 
              (isGrass ? ffm_settings::grassIDTReduction : 1.0);
            dryingFactor *= std::max(0.0, 1 - ffm_settings::computationTimeInterval / dryingIDT);
          }
        }

        //compute temperatures at test Pt from incident flame and plant flame
        double distToIncidentFlame = (testPt - incidentFlameOrigin).norm();
        double incidentTemp = incidentFlame.plumeTemperature(distToIncidentFlame, weather_.airTempC());
        double plantTemp = plantFlame.plumeTemperature((testPt - plantFlame.origin()).norm(), weather_.airTempC());
        double maxTemp = std::max(incidentTemp, plantTemp);
        double idt = dryingFactor * spec.ignitionDelayTime(maxTemp)* 
          (isGrass ? ffm_settings::grassIDTReduction : 1.0);

        if (iPt == initialPt && step == 1) {
          iPath.addPreIgnitionData( 
              PreIgnitionData::incident(
                      incidentFlame.flameLength(), incidentFlame.depthIgnited(), 
                      distToIncidentFlame, dryingFactor, incidentTemp, idt) );
        }

        //if ignition does not occur for testPt then break from loop over penetration steps
        if (idt > ffm_settings::computationTimeInterval || maxTemp < spec.ignitionTemp())
          break;

        //if we get here ignition has occurred, so reset end pt and continue
        ePt = testPt;
      }//end of loop over penetrations steps
    }

    //set startTimeStep  when plant first ignites
    if (!ignition && iPt != ePt) {
      iPath.startTimeStep(timeStep);
      ignition = true;
    }
    
    if (ignition){
      if (!iPath.hasSegments()) {
        iPath.addSegment(Seg(iPt,ePt));
        plantFlames.push_back(iPath.flame(modifiedWindSpeed, slope()));
      } else {
        //compute flame duration and hence start point of new segment
        int fd;
        if (!plantFlameRun && (level == Stratum::CANOPY) && (iPt.x() > canopyHeatingDistance))
          //flame residence time is reduced for stratum ignition in canopy if the canopy has not
          //been heated to sufficient temperature
          //fd = round(ffm_settings::reducedCanopyFlameResidenceTime/ffm_settings::computationTimeInterval);
          fd = ceil(ffm_settings::reducedCanopyFlameResidenceTime/ffm_settings::computationTimeInterval);
        else
          // fd = round(spec.flameDuration()
          //         /ffm_settings::computationTimeInterval);
          fd = ceil(spec.flameDuration()
                    /ffm_settings::computationTimeInterval);
        Pt segStart = iPath.numSegments() < fd ? 
          iPath.ignitedSegments().front().start() : iPath.ignitedSegments().at(iPath.numSegments()-fd).end();
        //If the potential incident flame and plant flame path lengths are both zero and 
        //the start point of the new segment is equal to its end point then break from the loop and
        //therefore end the ignition path computation.
        //There is a VERY slight problem with this in that later incident flames may have a different
        //angle and therefore a non-zero potential path. But this is unlikely and the alternative is 
        //to keep the loop going for the full maxTimeSteps after ignition, which seems wasteful.
        if(!ffm_numerics::almostZero(maxIncidentPath) || !ffm_numerics::almostZero(maxPlantPath) || segStart != ePt) {
          iPath.addSegment(Seg(segStart, ePt));
          plantFlames.push_back(iPath.flame(modifiedWindSpeed, slope()));
        } else
          break; //from loop over time steps
      }
      //reset ignition point
      iPt = ePt;
      //increment counter if ignition has occurred
      ++safetyCounter;
    }
  }//end of loop over time steps
  return iPath;
}





