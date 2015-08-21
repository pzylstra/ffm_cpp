#ifndef SPECIES_H
#define SPECIES_H

#include <string>
#include <cmath>
#include "poly.h"
#include "ffm_settings.h"

/*!\brief The Species class holds and manipulates data representing individual plant species.
*/
class Species{

 public:

  /*!\brief Categorises leaf type*/
  enum LeafFormType {ROUND_LEAF, FLAT_LEAF};

  // constructors
 
  Species();

  Species ( const double& composition,
	    const std::string& name,
	    const Poly&   crown,
	    const double& liveLeafMoisture,
	    const double& deadLeafMoisture,
	    const double& proportionDead,
	    const double& silicaFreeAshContent, 
	    const double& ignitionTemperature,      
	    const LeafFormType& leafForm,
	    const double& leafThickness,
	    const double& leafWidth,
	    const double& leafLength,
	    const double& leafSeparation,
	    const double& stemOrder,
	    const double& clumpDiameter,
	    const double& clumpSeparation
	    );

  Species ( const double& composition,
    const std::string& name,
    const double& hc,
    const double& he,
    const double& ht,
    const double& hp,
    const double& width,
    const double& liveLeafMoisture,
    const double& deadLeafMoisture,
    const double& propDead,
    const double& silFreeAshCont,
    const double& ignitTemp,     
    const LeafFormType& leafForm,
    const double& leafThick,
    const double& leafWidth,
    const double& leafLength,
    const double& leafSep,
    const double& stemOrder,
    const double& clumpDiam,
    const double& clumpSep
    );

  //accessors

  bool          isValid() const;
  double        composition() const;
  std::string   name() const;
  Poly          crown() const;
  double        liveLeafMoisture() const;
  double        deadLeafMoisture() const;
  double        propDead() const;
  double        silFreeAshCont() const;
  double        ignitTemp() const;
  LeafFormType  leafForm() const;
  double        leafThick() const;
  double        leafWidth() const;
  double        leafLength() const;
  double        leafSep() const;
  double        stemOrder() const;
  double        clumpDiam() const;
  double        clumpSep() const;

  //mutators

  void composition(const double& composition);

  // other methods

  double width() const;
  double leafMoisture() const;
  double ignitionTemp(bool modelIt = false) const;
  double ignitionDelayTime(const double& temperature) const;
  double flameDuration() const;
  bool isGrass() const;
  double leafFlameLength() const;
  double flameLength(const double& lengthIgnited) const;
  double leafAreaIndex() const;

  std::string printToString() const;

  std::string printComposition() const;
  std::string printLiveLeafMoisture() const;
  std::string printDeadLeafMoisture() const;
  std::string printPropDead() const;
  std::string printSilFreeAshCont() const;
  std::string printIgnitTemp() const;
  std::string printIgnitionTemp() const; //prints the result of ignitionTemp()
  std::string printLeafForm() const;
  std::string printLeafThick() const;
  std::string printLeafWidth() const;
  std::string printLeafLength() const;
  std::string printLeafSep() const;
  std::string printStemOrder() const;
  std::string printClumpSep() const;
  std::string printClumpDiam() const;
  std::string printCrown() const;

  double leavesPerClump() const;

  bool operator==(const Species&) const;  
  bool sameSpecies(const Species&) const; 

private:

  bool         isValid_ = false;

  double       composition_ = 0; 
  std::string  name_ = std::string();
  Poly         crown_ = Poly();;
  double       liveLeafMoisture_ = -99;         // proportion of oven dried weight, no percentages
  double       deadLeafMoisture_ = -99;         // proportion of oven dried weight, no percentages
  double       propDead_ = -99;         //proportion, no percentages
  double       silFreeAshCont_ = -99;    // proportion, no percentages
  double       ignitTemp_ = -99;
  LeafFormType leafForm_ = ROUND_LEAF;
  double       leafThick_ = -99;
  double       leafWidth_ = -99;
  double       leafLength_ = -99;
  double       leafSep_ = -99;
  double       stemOrder_ = -99;
  double       clumpSep_ = -99;
  double       clumpDiam_ = -99;


 };


#include "species_inline.h"


#endif
