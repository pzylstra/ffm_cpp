#include "stratum.h"

using std::string;

/*!\brief Standard constructor.
  \param level
  \param speciesVector
  \param plantSeparation

  The constructor will not add an empty species or one with a 
  non-positive composition to the Stratum. The composition
  of each of the Species in the Stratum will be adjusted
  so that they sum to 1
*/

Stratum::Stratum(const Stratum::LevelType& level,
		 const std::vector<Species>& speciesVector, 
		 const double& plantSeparation,
		 bool includeForIgnition) :
  level_(UNKNOWN_LEVEL),
  allSpecies_(),
  plantSep_(-99)
{
  //must have a known level
  if (level == UNKNOWN_LEVEL) return;
  //species  vector must be non-empty and the 
  //plant separation is allowed to be negative
  //in which case it is considered to be N/A and is set to -99
  if(speciesVector.empty()) return;

  double sum(0);
  for (const Species& s : speciesVector){
    if (s.composition() > 0 && !s.crown().vertices().empty()){
      //will not add a species with zero or negative composition or 
      //an empty species to a stratum
      allSpecies_.push_back(s);
      sum += s.composition();
    }
  }
  if (sum == 0) return; //an empty stratum

  for(auto& s : allSpecies_) 
    s.composition(s.composition() / sum);
  level_ = level;
  if (plantSeparation >= 0) plantSep_ = plantSeparation;

  includeForIgnition_ = includeForIgnition;
}

// average width, top, bottom, midheight computations for stratum

/*!\brief Composition-weighted average of Species width
  \return The composition-weighted average of Species width
*/
double Stratum::avWidth() const {
  double av(0);
  for(const auto& s : allSpecies_) av += s.composition() * s.crown().width();
  return av;
}

/*!\brief Composition-weighted average of Species height
  \return The composition-weighted average of Species height
*/
double Stratum::avTop() const {
  double av(0);
  for(const auto& s : allSpecies_) av += s.composition() * s.crown().top();
  return av;
}

/*!\brief Composition-weighted average of Species bottom
  \return The composition-weighted average of the lowest coordinate
  vertical coordinate of the Species crown
*/
double Stratum::avBottom() const {
  double av(0);
  for(const auto& s : allSpecies_) av += s.composition() * s.crown().bottom();
  return av;
}

/*!\brief Composition-weighted average of Species mid-height
  \return 0.5*(avTop() + avBottom())
*/
double Stratum::avMidHt() const {return 0.5*(avTop() + avBottom());}

/*!\brief Composition-weighted average of Species flame duration
  \return The composition-weighted average of Species flame duration
*/
double Stratum::avFlameDuration() const {
  double av = 0;
  for(const auto& s : allSpecies_) 
    av += s.composition() * s.flameDuration();
  return av;
}

// plant separation computation for stratum

/*!\brief Maximum of plant separation and composition-weighted species width
  \return The maximum of plantSep() and avWidth()
*/
double Stratum::modelPlantSep() const {
  return (plantSep_ > avWidth()) ? plantSep_ : avWidth();
}

// cover computation for stratum

double Stratum::cover() const {
  return pow(avWidth()/modelPlantSep(),2);
}

// leaf area index computation for stratum
double Stratum::leafAreaIndex() const {
  double lai(0);
  for(const auto& s : allSpecies_) lai += s.composition() * s.leafAreaIndex();
  return lai * cover();
}

