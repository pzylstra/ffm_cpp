#ifndef STRATUM_RESULTS_H
#define STRATUM_RESULTS_H

#include <string>
#include "stratum.h"

/*!\brief The StratumResultsClass holds and manipulates Stratum specific results of the fire computation
 */
class StratumResults{
public:

  /*!\brief Classification of Stratum fire
   */
  enum StratumFireType {UNCLASSIFIED, PASSIVE, ACTIVE, INDEPENDENT};

  //constructors

  StratumResults();
  StratumResults(const Stratum::LevelType& level);

  //accessors

  Stratum::LevelType level() const;
  double ros() const;
  double flameTipHeight() const;
  double flameOriginHeight() const;
  double flameLength() const;
  double flameAngle() const;
  double proportionBurnt() const;
  std::map<std::string, double> speciesFlameTipHeights() const;

  //mutators

  void ros(const double&);
  void flameTipHeight(const double&);
  void flameOriginHeight(const double&);
  void flameLength(const double&);
  void flameAngle(const double&);
  void proportionBurnt(const double&);
  void addSpeciesFlameTipHeight(const std::string& name, const double& ht);

  //printing and conversion

  std::string printROS() const;
  std::string printFlameTipHeight() const;
  std::string printFlameOriginHeight() const;
  std::string printFlameLength() const;
  std::string printFlameAngle() const;
  std::string printProportionBurnt() const;

private:

  Stratum::LevelType level_ = Stratum::UNKNOWN_LEVEL;
  double ros_ = 0;
  double flameTipHeight_ = 0;
  double flameOriginHeight_ = 0;
  double flameLength_ = 0;
  double flameAngle_ = 0;
  double proportionBurnt_ = 0;

  std::map<std::string, double> speciesFlameTipHeights_ ;

};

#include "stratum_results_inline.h"

#endif //STRATUM_RESULTS_H
