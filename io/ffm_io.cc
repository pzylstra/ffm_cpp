#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <iostream>

#include "pt.h"
#include "seg.h"
#include "poly.h"
#include "species.h"
#include "stratum.h"
#include "forest.h"
#include "location.h"
#include "surface.h"
#include "weather.h"
#include "flame.h"
#include "flame_series.h"
#include "ffm_util.h"
#include "ffm_io.h"
#include "results.h"


/*!\brief Processes a line from the input text file
  \param line
  \result A vector of type string of size at most 2.

  All characters after the comment sign '#' are removed
  including the '#' itself.
  If the '=' character exists in the line then the line
  is split by that character. The vector returned will 
  consist of the string to the left and the string to the 
  right of the '=' character. The string to the left is 
  converted to lower case and all white space is removed.
  The string to the right has leading and trailing white space 
  removed. 

  If the '=' sign does not exist in the line then the vector returned 
  has size at most 1 and consists of the contents of the line (excluding 
  comments) with all white space removed and all characters converted
  to lower case.
*/
std::vector<std::string> processLine(const std::string& line) {
  std::vector<std::string> retVal;
  std::string l;
  l = ffm_util::trim(line);
  l = l.substr(0, l.find('#')); //erase everything after a # character
  if (l.empty()) 
    return retVal;
  std::vector<std::string> strVec = ffm_util::split(l,'=');
  std::string firstString = ffm_util::reduce(strVec.front());
  std::transform(firstString.begin(), firstString.end(), firstString.begin(), ::tolower);
  retVal.push_back(firstString);
  if (strVec.size() > 1)
    retVal.push_back(strVec.at(1));
  return retVal;
}

/*!\brief Conversion from string to double
  \param str Assumes str is a comma separated list of strings, might be only one in the list
  \return The double representing by the FIRST element of the list
*/
double stringToDouble(const std::string& str) {
  //assumes str is a comma separated list of strings, might be only one in the list
  //returns the double representing by the FIRST element of the list
  std::vector<std::string> strVec = ffm_util::split(str, ',');
  return atof(strVec.at(0).c_str());
}

/*!\brief Initial parse of input file
  \param inPath input file path
  \return A pair consisting of the output level desired (Results::OutputLevelType) and the
  number of monte carlo iterations required
*/
std::pair<Results::OutputLevelType, int> prelimParseInputTextFile(std::string inPath) {
  //provides an intial parse of the input file, to find output type and the number of monte carlo
  //iterations if applicable

  //file parsing variables
  std::ifstream inFile(inPath);
  std::string line;
  std::string firstString, secondString;
  std::vector<std::string> strVec;

  //other variables
  Results::OutputLevelType outputLevel = Results::COMPREHENSIVE;
  int numIterations = -1;
  
  //map to parse Results::OutputLevelType
  std::map<std::string, Results::OutputLevelType> outputLevelTypeMap = { {"basic", Results::BASIC},
									 {"1", Results::BASIC},
									 {"detailed", Results::DETAILED},
									 {"2", Results::DETAILED},
									 {"comprehensive", Results::COMPREHENSIVE},
									 {"3", Results::COMPREHENSIVE},
									 {"montecarlo", Results::MONTE_CARLO},
									 {"4", Results::MONTE_CARLO} };

  while(getline(inFile, line)) {

    std::vector<std::string> strVec = processLine(line);
    if (strVec.size() < 2) continue;
    firstString = strVec.front();
    secondString = strVec.back();

    // output level
    if (firstString == "outputlevel"){
      secondString = ffm_util::reduce(secondString);
      std::transform(secondString.begin(), secondString.end(), secondString.begin(), ::tolower);
      outputLevel = outputLevelTypeMap.at(secondString);
      continue;
    }

    //number of Monte Carlo iterations
    if (firstString == "montecarloiterations") {
      numIterations = atoi(secondString.c_str()); 
      continue;
    }
  }

  return std::make_pair(outputLevel, numIterations);
}


/*!\brief Parse input file
\param inPath input file path
\param monteCarlo
\return A location object that is constructed from the contents of inputFileName
*/
Location parseInputTextFile(const std::string& inPath, const bool& monteCarlo) {

  // file parsing variables
  std::ifstream inFile(inPath);
  std::string line;
  std::string firstString;
  std::string secondString;
  std::vector<std::string> strVec;
  bool speciesInFlag = false, stratumInFlag = false;

  // stratum variables
  std::vector<Species> specVec;
  std::vector<Stratum> stratVec;
  double psep;
  Stratum::LevelType level;

  // species variables
  std::string spname;
  Poly crn;
  double hc, he, ht, hp, hpMean, w;
  double lmoist,sfac,itemp,lthick,lwidth,llength,lsep,sord,csep,cdiam,pdead;
  Species::LeafFormType lform;
  double comp;

  // surface variables
  double slope, dfmc, fuelload, fueldiam, thickl;

  //weather variables
  double airtemp;

  // other variables
  double firelineLength;
  double incidentWindSpeed;
  std::vector<Forest::StrataOverlap> strataOverlapVec;

  //map to parse Stratum::LevelType
  std::map<std::string, Stratum::LevelType> levelTypeMap;
  levelTypeMap = { {"nearsurface", Stratum::NEAR_SURFACE},
		   {"ns", Stratum::NEAR_SURFACE},
		   {"elevated", Stratum::ELEVATED},
		   {"e", Stratum::ELEVATED},
		   {"midstorey", Stratum::MID_STOREY},
		   {"m", Stratum::MID_STOREY},
		   {"canopy", Stratum::CANOPY},
		   {"c", Stratum::CANOPY} };

  //map to parse Forest::StrataOverlapType
  std::map<std::string, Forest::StrataOverlapType> overlapTypeMap;
  overlapTypeMap = { {"automatic", Forest::AUTO_CALC_OVERLAP},
		     {"auto", Forest::AUTO_CALC_OVERLAP},
		     {"notoverlapped", Forest::NOT_OVERLAPPED},
		     {"no", Forest::NOT_OVERLAPPED},
		     {"false", Forest::NOT_OVERLAPPED},
		     {"overlapped", Forest::OVERLAPPED},
		     {"yes", Forest::OVERLAPPED},
		     {"true", Forest::OVERLAPPED} };

  //map to parse Species::LeafFormType
  std::map<std::string, Species::LeafFormType> leafFormTypeMap;
  leafFormTypeMap = { {"round", Species::ROUND_LEAF},
		      {"flat", Species::FLAT_LEAF} };

 
  //At the moment there is no input for species  dead fuel moisture content, although Species
  //objects have deadLeafMoisture_ data member. So the surface dead fuel moisture content is used
  //to fill the deadLeafMoisture_ data member for each instantiated Species object. So we need a
  //preliminary file read to get surface dead fuel moisture content which will be used in each species
  //this obviates the need for the surface variables to occur first in the input text file
  //this hack can be eliminated if/when dfmc is specified on a species by species basis
  dfmc = -99;
  while(getline(inFile, line)) {

    std::vector<std::string> strVec = processLine(line);
    if (strVec.size() < 2) continue;
    firstString = strVec.front();
    secondString = strVec.back();
    if (firstString == "surfacedeadfuelmoisturecontent") {
      dfmc = monteCarlo ? ffm_util::randomNormal(secondString) : stringToDouble(secondString); 
      if (dfmc <= 0){
	if (monteCarlo) 
	  return Location();
	else {
	  std::cout << "Problem with input file - surface dead fuel moisture content" << std::endl;
	  exit(1);
	}
      }
      break;
    }
  }
  
  //rewind the file and start again
  inFile.seekg(0);

  //read the file line by line
  while(getline(inFile, line)) {

    std::vector<std::string> strVec = processLine(line);
    if (strVec.empty()) continue;
    firstString = strVec.front();
    secondString = strVec.back();

    //first we treat all the valid input lines that are not assigning a value
    //in other words strVec will have size 1

    if (firstString == "beginstratum") {
      if (stratumInFlag) {
	std::cout << "Problem with input file - misplaced begin stratum" << std::endl;
	exit(1);
      }
      stratumInFlag = true;
      specVec.clear();
      level = Stratum::UNKNOWN_LEVEL;
      psep = -99;
      continue;
    }

    if (firstString == "endstratum") {
      if (!stratumInFlag) {
	std::cout << "Problem with input file - misplaced end stratum" << std::endl;
	exit(1);
      }
      stratumInFlag = false;

      //if level was not set or level is already in stratVec then skip this stratum
      if (level == Stratum::UNKNOWN_LEVEL) continue;
      if (find_if(stratVec.begin(), 
		  stratVec.end(), 
		  [level](Stratum s){return level == s.level();}) 
	  < stratVec.end()) continue;
     
      stratVec.push_back(Stratum(level, specVec, psep));
      continue;
    }
      
    if (firstString == "beginspecies") {
      if (speciesInFlag) { 
	std::cout << "Problem with input file - misplaced begin species" << std::endl;
	exit(1);
      }
      speciesInFlag = true;
      //clear species variables ready for read
      comp = 0;
      hc = -99;
      he = -99;
      ht = -99;
      hp = -99;
      spname = "";
      crn = Poly();
      lmoist = -99;
      sfac = -99;
      itemp = -99;
      lthick = -99;
      lwidth = -99;
      llength = -99;
      lsep = -99;
      sord = -99;
      cdiam = -99;
      csep = -99;
      pdead = -99;
      continue;
    }

    if (firstString == "endspecies") {
      if (!speciesInFlag) {
	std::cout << "Problem with input file - misplaced end species" << std::endl; 
	exit(1);
      }
      speciesInFlag = false;

      //if monte carlo then scale the plant geometry. 
      if (monteCarlo) {
	hc *= hp/hpMean;
	he *= hp/hpMean;
	ht *= hp/hpMean;
	w  *= hp/hpMean;
      }

      //check the species values
      bool speciesOkay = true;
      if (ht < he                   || 
	  hp <= hc                  ||
	  comp < 0                  ||
	  lmoist < 0                ||
	  lthick < 0                ||
	  lwidth < 0                ||
	  llength < 0               ||
	  lsep <= 0                 ||
	  pdead < 0                 ||
	  pdead > 1                 ||
	  sord <= 0                 ||
	  csep < 0                  ||
	  cdiam <= 0                ||
	  (itemp <= 0 && sfac <= 0)
	  ) speciesOkay = false;

      if (!speciesOkay) {
	if (monteCarlo) 
	  return Location();
	else {
	  std::cout << "Problem with input values for species " + spname;
	  exit(1);
	}
      }

      specVec.push_back(Species(comp, spname, hc, he, ht, hp, w, lmoist, dfmc, pdead, sfac,  
				itemp, lform,lthick,lwidth, llength, lsep, sord, cdiam, csep));
      continue;
    }

    //now we treat the input lines that are making an assignment, so that there 
    //should be something to the right of the '=' sign, ie strVec.size() should be 2

    if (strVec.size() < 2) continue;

    if (speciesInFlag && stratumInFlag) {

      if (firstString == "composition") {
	comp = monteCarlo ? ffm_util::randomNormal(secondString) : stringToDouble(secondString);
	continue;
      }

      if (firstString == "name") {spname = secondString; continue;}

      // crown geometry - note that if monteCarlo then hc, he, ht and w will be 
      // scaled by hp / mean_of_hp, but this has to happen when the entire species 
      // has been read, ie when firstString == endspecies

      if (firstString == "hc" ) {
	hc = monteCarlo ? ffm_util::randomUniform(secondString) : stringToDouble(secondString);
	continue;
      }
 
      if (firstString == "he" ) {
	he = monteCarlo ? ffm_util::randomUniform(secondString) : stringToDouble(secondString);
 	continue;
      }

      if (firstString == "ht" ) {
	ht = monteCarlo ? ffm_util::randomUniform(secondString) : stringToDouble(secondString);
 	continue;
      }

      if (firstString == "hp" ) { 
	hp = monteCarlo ? ffm_util::randomUniform(secondString) : stringToDouble(secondString);
	hpMean = stringToDouble(secondString);
	continue;
      }
 
      if (firstString == "w" ) {
	w = monteCarlo ? ffm_util::randomUniform(secondString) : stringToDouble(secondString);
	continue;
      }
 
      if (firstString == "liveleafmoisture") {
	lmoist = monteCarlo ? ffm_util::randomNormal(secondString) : stringToDouble(secondString);
	continue;
      }

      if (firstString == "silicafreeashcontent") {
	sfac = stringToDouble(secondString); 
	continue;
      }

      if (firstString == "ignitiontemperature") {
	itemp = monteCarlo ? ffm_util::randomUniform(secondString) : stringToDouble(secondString);
	continue;
      }

      if (firstString == "leafform"){
	std::string tmp = secondString;
	std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
	lform = leafFormTypeMap.at(tmp);
	continue;
      }

      if (firstString == "leafthickness") {
	lthick = monteCarlo ? ffm_util::randomUniform(secondString) : stringToDouble(secondString);
	continue;
      }

      if (firstString == "leafwidth") {
	lwidth = monteCarlo ? ffm_util::randomUniform(secondString) : stringToDouble(secondString);
	continue;
      }

      if (firstString == "leaflength") {
	llength = monteCarlo ? ffm_util::randomUniform(secondString) : stringToDouble(secondString);
	continue;
      }

      if (firstString == "leafseparation") {
	lsep = monteCarlo ? ffm_util::randomUniform(secondString) : stringToDouble(secondString);
	continue;
      }

      if (firstString == "stemorder") {
	sord = monteCarlo ? ffm_util::randomUniform(secondString) : stringToDouble(secondString);
	continue;
      }

      if (firstString == "clumpseparation") {
	csep = monteCarlo ? ffm_util::randomNormal(secondString) : stringToDouble(secondString);
	continue;
      }

      if (firstString == "clumpdiameter") {
	cdiam = monteCarlo ? ffm_util::randomNormal(secondString) : stringToDouble(secondString);
	continue;
      }

      if (firstString == "proportiondead") {
	pdead = monteCarlo ? ffm_util::randomUniform(secondString) : stringToDouble(secondString);
	continue;
      }
    }

    if (stratumInFlag) {
      if (firstString == "level") {
	std::string tmp = secondString;
	tmp = ffm_util::reduce(tmp);
	std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
	level = levelTypeMap.at(tmp);
	continue;
      }
 
      if (firstString == "plantseparation") {
	psep = monteCarlo ? ffm_util::randomNormal(secondString) : stringToDouble(secondString);
	continue;
      }
    }


    // Surface variables
    if (firstString == "slope") {slope = atof(secondString.c_str())*PI/180.0; continue;}

    //// the following is commented out because the surface dead fuel moisture content is currently
    //// read in a preliminary pass through the file for reasons noted above. This might be uncommented
    //// in future versions of the model

    // if (firstString == "surfacedeadfuelmoisturecontent") {
    //   dfmc = monteCarlo ? randomNormal(secondString) : stringToDouble(secondString);
    //   continue;
    // }

    if (firstString == "fuelloadtonnesperhectare") {
      //note conversion to kg/m^2
      fuelload = monteCarlo ? ffm_util::randomNormal(secondString)*0.1 : stringToDouble(secondString)*0.1;
      if (fuelload < 0.4) { 
	if (monteCarlo) 
	  return Location();
	else { 
	  std::cout << "Problem with input file - check fuel load tonnes per hectare" << std::endl;
	  exit(1);
	}
      }
      continue;
    } 

    if (firstString == "meanfueldiameter") {fueldiam = atof(secondString.c_str()); continue;}

    if (firstString == "meanfinenessleaves") {thickl = atof(secondString.c_str()); continue;}

    //Weather variables
    if (firstString == "airtemperature") {
      airtemp = monteCarlo ? ffm_util::randomNormal(secondString) : stringToDouble(secondString);
      continue;
    }

    //the other stuff
    if (firstString == "firelinelength") {firelineLength = atof(secondString.c_str()); continue;}

    //incident windspeed is converted from km/h to m/s
    if (firstString == "incidentwindspeed") {
      incidentWindSpeed = monteCarlo ? ffm_util::randomNormal(secondString)/3.6 : stringToDouble(secondString)/3.6;
      continue;
    }
 
    if (firstString == "overlapping"){
      std::vector<std::string> tmp = ffm_util::split(secondString, ',');
      if (tmp.size() == 3) {
	strataOverlapVec.push_back(std::make_tuple(levelTypeMap.at(ffm_util::reduce(tmp[0])), 
						   levelTypeMap.at(ffm_util::reduce(tmp[1])), 
						   overlapTypeMap.at(ffm_util::reduce(tmp[2]))
						   )
				   );
      }
      continue;
    }


  }  //end of file read

  return Location(Forest(Surface(slope,dfmc,fuelload,fueldiam,thickl), stratVec, strataOverlapVec),
		  Weather(airtemp),
		  incidentWindSpeed,
		  firelineLength);


}


/*!\brief Produces header part of CSV file for monte carlo run
  \param loc
  \return A comma separated string
*/
std::string printMonteCarloHeader(const Location& loc) {
  const char sepChar = ',';
  const std::string nl = "\n";
  char s[60];

  std::string str;
  std::string sep(1,sepChar);

  str = "Monte Carlo Results\n\nSITE CHARACTERISTICS\n";

  str += "Fireline length (m)" + sep + loc.printFirelineLength() + nl;

  str += "Slope (deg)" + sep + loc.forest().surface().printSlope() + nl;

  str += "Mean surface fuel diameter (m)" + sep + loc.forest().surface().printMeanFuelDiam() + nl;
  
  str += "Mean leaf fineness (m)" + sep + loc.forest().surface().printMeanFineLeaves() + nl;

  str += "Specified overlaps" + sep;
  auto overlaps = loc.forest().strataOverlaps();
  if (!overlaps.empty()) {
    bool firstOne = true;
    for (const auto& so : overlaps){
      if (firstOne) 
	firstOne = false;
      else
	str += sep;
      str += overlapToString(so) + nl;
    }
  }

  str += nl;

  str += "INPUTS";
  str += std::string(4,sepChar);
  int advance = 0;
  for (const auto& st : loc.strata())
    advance += 1 + st.allSpecies().size()*19;
  str += std::string(advance,sepChar);
  str += "OUTPUTS" + nl;

  if (!loc.strata().empty()) {
    str += std::string(4,sepChar);
    advance = 0;
    for (const auto& st : loc.strata()) {
      str += std::string(advance, sepChar);
      str += st.name();
      advance = st.allSpecies().size() * 19 + 1;
    } 
    str += nl;
  
    str += "Conditions" + std::string(4,sepChar);
    advance = 0;
    for (const auto& st : loc.strata()) {
      str += sep;
      for (const auto& sp : st.allSpecies()) {
	str += std::string(advance, sepChar);
	str += sp.name();
	advance = 19;
      }
    }
    str += std::string(advance, sepChar);
    str += "Flame length";
    advance = 2 + loc.strata().size();
    str += std::string(advance, sepChar);
    str += "Flame tip height";
    str += std::string(advance, sepChar);
    str += "Flame origin height";
    str += std::string(advance, sepChar);
    str += "Flame angle";
    str += std::string(advance, sepChar);
    str += "ROS";
    advance = 7 + loc.strata().size();
    str += std::string(advance, sepChar);
    str += "Scorch height";
    str += nl;
  }

  str += "Wind speed (km/h)" + sep + "Air temperature (deg C)" + sep + "Dead FMC";
  str += sep + "Surface fuel load (t/ha)";
  for (const auto& st : loc.strata()){
    str += sep + "Plant separation (m)";
    for (const auto& sp : st.allSpecies()) {
      str += sep + "Composition" + sep + "Live FMC" + sep + "Silica free ash content"; 
      str += sep + "Ignition temp input" + sep + "Ignition temp" + sep + "Proportion dead" + sep  + "Leaf thickness";
      str += sep + "Leaf width" + sep + "Leaf length" + sep;
      str += "Leaf separation" + sep + "Stem order" + sep + "Clump separation" + sep;
      str += "Clump diameter" + sep + "Crown" + sep + "Hc" + sep + "He";
      str += sep + "Ht" + sep + "Hp" + sep + "w";
    }
  }
  std::string tmpStr = sep + "Overall" + sep  + "Surface";
  for (const auto& st : loc.strata())
    tmpStr += sep + st.name();
  str += tmpStr + tmpStr + tmpStr + tmpStr;
  str += sep + "Flame depth";
  str +=  sep + "Crown fire type";
  str +=  sep + "Crown run length";
  str +=  sep + "Crown run velocity";
  str +=  sep + "Wind reduction factor";
  str +=  sep + "McArthur";
  str +=  sep + "Luke & McArthur";
  str +=  sep + "Van Wagner";
  str +=  sep + "Van Wagner with wind";
  str += nl;

  return str;
}

/*!\brief Produces inputs part of CSV file for monte carlo run
  \param loc
  \return A comma separated string
*/
std::string printMonteCarloInputs(const Location& loc) {
  std::string str;
  
  str = loc.printWindSpeed();
  str += "," + loc.weather().printAirTempC();
  str += "," + loc.forest().surface().printDeadFuelMoistCont();
  str += "," + loc.forest().surface().printFuelLoad();
  for (const auto& st : loc.strata()) {
    str += "," + st.printPlantSep();
    for (const auto& sp : st.allSpecies()){
      str += "," + sp.printComposition();
      str += "," + sp.printLiveLeafMoisture();
      str += "," + sp.printSilFreeAshCont();
      str += "," + sp.printIgnitTemp();
      str += "," + sp.printIgnitionTemp();
      str += "," + sp.printPropDead();
      str += "," + sp.printLeafThick();
      str += "," + sp.printLeafWidth();
      str += "," + sp.printLeafLength();
      str += "," + sp.printLeafSep();
      str += "," + sp.printStemOrder();
      str += "," + sp.printClumpSep();
      str += "," + sp.printClumpDiam();
      str += "," + sp.printCrown();
      char s[10];
      sprintf(s,"%.3f",sp.crown().centreBottom());
      str += "," + std::string(s);
      sprintf(s,"%.3f",sp.crown().rightBottom());
      str += "," + std::string(s);
      sprintf(s,"%.3f",sp.crown().rightTop());
      str += "," + std::string(s);
      sprintf(s,"%.3f",sp.crown().centreTop());
      str += "," + std::string(s);
      sprintf(s,"%.3f",sp.width());
      str += "," + std::string(s);
    }
  }
  return str;
}

/*!\brief Produces results part of CSV file for monte carlo run
  \param res
  \return A comma separated string
*/
std::string printMonteCarloResults(const Results& res) {
  std::string str;
  std::string sepStr = ",";

  str = sepStr + res.printFlameLength();
  str += sepStr + res.printSurfaceFlameLength();
  for (const auto& sr : res.strataResults())
    str += sepStr + sr.printFlameLength();

  str += sepStr + res.printFlameTipHeight();
  str += sepStr + res.printSurfaceFlameHeight();
  for (const auto& sr : res.strataResults())
    str += sepStr + sr.printFlameTipHeight();

  str += sepStr + res.printFlameOriginHeight();
  str += sepStr + "0.0"; // for surface origin height
  for (const auto& sr : res.strataResults())
    str += sepStr + sr.printFlameOriginHeight();

  str += sepStr + res.printFlameAngle();
  str += sepStr + res.printSurfaceFlameAngle();
  for (const auto& sr : res.strataResults())
    str += sepStr + sr.printFlameAngle();

  str += sepStr + res.printROS();
  str += sepStr + res.printSurfaceROS();
  for (const auto& sr : res.strataResults())
    str += sepStr + sr.printROS();

  str += sepStr + res.printFlameDepth();
  str += sepStr + res.printCrownFireType();
  str += sepStr + res.printCrownRunLength();
  str += sepStr + res.printCrownRunVelocity();
  str += sepStr + res.printWindReductionFactor();
  str += sepStr + res.printScorchHeightMcarthur();
  str += sepStr + res.printScorchHeightLukeMcarthur();
  str += sepStr + res.printScorchHeightVanWagner();
  str += sepStr + res.printScorchHeightVanWagnerWithWind();

  return str;
}
