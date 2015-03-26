#ifndef RESULTS_H
#define RESULTS_H

#include "forest_ignition_run.h"
#include "results.h"
#include "stratum_results.h"

/*!\brief Holds and manipulates all the data resulting from the computation at a single location.

  This class is merely a storeage class and does not compute the results. The results are computed by the method Location::results()
 */
class Results{

public:
  
  /*!\brief Specifies type of output level required by user*/
  enum OutputLevelType{BASIC, DETAILED, COMPREHENSIVE, MONTE_CARLO}; 

  /*!\brief Specifies crown fire type*/
  enum CrownFireType {UNCLASSIFIED, PASSIVE, ACTIVE};

  //constructors

  Results ();

  //accessors

  double ros() const;
  double flameTipHeight() const;
  double flameOriginHeight() const;
  double flameLength() const;
  double flameAngle() const;
  double flameDepth() const;
  double surfaceROS() const;
  double surfaceFlameLength() const;
  double surfaceFlameHeight() const;
  double surfaceFlameAngle() const;
  CrownFireType crownFireType() const;
  double crownRunLength() const;
  double crownRunVelocity() const;
  double windReductionFactor() const;
  double scorchHeightMcarthur() const;
  double scorchHeightLukeMcarthur() const;
  double scorchHeightVanWagner() const;
  double scorchHeightVanWagnerWithWind() const;
  std::vector<StratumResults> strataResults() const;
  std::vector<ForestIgnitionRun> runs() const;
  bool runTwoExists() const;

  //mutators

  void ros(const double& rateOfSpread);
  void flameTipHeight(const double& flameHt);
  void flameOriginHeight(const double& flameHt);
  void flameLength(const double& flameLen);
  void flameAngle(const double& flameAng);
  void surfaceROS(const double& rateOfSpread);
  void surfaceFlameLength(const double& flameLen);
  void surfaceFlameHeight(const double& flameHt);
  void surfaceFlameAngle(const double& flameAng);
  void flameDepth(const double& flameDep);
  void crownFireType(const CrownFireType& type);
  void crownRunLength(const double& length);
  void crownRunVelocity(const double& velocity);
  void windReductionFactor(const double& factor);
  void scorchHeightMcarthur(const double& ht);
  void scorchHeightLukeMcarthur(const double& ht);
  void scorchHeightVanWagner(const double& ht);
  void scorchHeightVanWagnerWithWind(const double& ht);
  void addStratumResults(const StratumResults&);
  void addRun(const ForestIgnitionRun&);

  //other methods

  bool strataResultsEmpty() const;
  std::vector<StratumResults>::const_iterator strataResultsBegin() const;
  std::vector<StratumResults>::const_iterator strataResultsEnd() const;

  //printing functions
  std::string printROS() const;
  std::string printFlameTipHeight() const;
  std::string printFlameOriginHeight() const;
  std::string printFlameLength() const;
  std::string printFlameAngle() const;
  std::string printFlameDepth() const;
  std::string printSurfaceROS() const;
  std::string printSurfaceFlameLength() const;
  std::string printSurfaceFlameHeight() const;
  std::string printSurfaceFlameAngle() const;
  std::string printCrownFireType() const;
  std::string printCrownRunLength() const;
  std::string printCrownRunVelocity() const;
  std::string printWindReductionFactor() const;
  std::string printScorchHeightMcarthur() const;
  std::string printScorchHeightLukeMcarthur() const;
  std::string printScorchHeightVanWagner() const;
  std::string printScorchHeightVanWagnerWithWind() const;
  std::string printToString(const OutputLevelType&) const;
  
  

private:

  //basic outputs
  double ros_ = 0;
  double flameTipHeight_ = 0;
  double flameOriginHeight_ = 0;
  double flameLength_ = 0;
  double flameAngle_ = 0;

  //detailed outputs are those above plus
  double surfaceROS_ = 0;
  double surfaceFlameLength_ = 0;
  double surfaceFlameHeight_ = 0;
  double surfaceFlameAngle_ = 0;
  double flameDepth_ = 0;
  CrownFireType crownFireType_= UNCLASSIFIED;
  double crownRunLength_ = 0;
  double crownRunVelocity_ = 0;
  double windReductionFactor_ = 0;
  double scorchHeightMcarthur_ = 0;
  double scorchHeightLukeMcarthur_ = 0;
  double scorchHeightVanWagner_ = 0;
  double scorchHeightVanWagnerWithWind_ = 0;
  std::vector<StratumResults> strataResults_ = std::vector<StratumResults>();

  //comprehensive outputs are those above plus the following vector which 
  //has at most two components
  std::vector<ForestIgnitionRun>  runs_ =  std::vector<ForestIgnitionRun>();
  
};

const std::map<Results::CrownFireType, std::string> crownFireTypeStringMap = 
  {{Results::UNCLASSIFIED, "Unclassified"},
   {Results::PASSIVE, "Passive"},
   {Results::ACTIVE, "Active"}};

const std::map<Results::OutputLevelType, std::string> outputLevelTypeStringMap = 
  {{Results::BASIC, "Basic"},
   {Results::DETAILED, "Detailed"},
   {Results::COMPREHENSIVE, "Comprehensive"},
   {Results::MONTE_CARLO, "Monte Carlo"}};

#include "results_inline.h"

#endif //RESULTS_H
