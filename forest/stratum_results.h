#ifndef STRATUM_RESULTS_H
#define STRATUM_RESULTS_H

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
  StratumResults(const Stratum::LevelType& level, 
		const double& ros, 
		const double& flameHeight, 
		const double& flameLength, 
		const double& flameAngle, 
		const double& propBurnt);

  //accessors

  Stratum::LevelType level() const;
  double ros() const;
  double flameHeight() const;
  double flameLength() const;
  double flameAngle() const;
  double proportionBurnt() const;

  //mutators

  void ros(const double&);
  void flameHeight(const double&);
  void flameLength(const double&);
  void flameAngle(const double&);
  void proportionBurnt(const double&);

  //printing and conversion

  std::string printROS() const;
  std::string printFlameHeight() const;
  std::string printFlameLength() const;
  std::string printFlameAngle() const;
  std::string printProportionBurnt() const;

private:

  Stratum::LevelType level_ = Stratum::UNKNOWN_LEVEL;
  double ros_ = 0;
  double flameHeight_ = 0;
  double flameLength_ = 0;
  double flameAngle_ = 0;
  double proportionBurnt_ = 0;
};

#include "stratum_results_inline.h"

#endif //STRATUM_RESULTS_H
