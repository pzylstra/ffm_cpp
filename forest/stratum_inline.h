#ifndef STRATUM_INLINE_H
#define STRATUM_INLINE_H

//constructors

/*!\brief Default constructor produces empty Stratum*/
inline Stratum::Stratum() : level_(Stratum::UNKNOWN_LEVEL),
			    allSpecies_(std::vector<Species>()),
			    plantSep_(-99) {}

//accessors

/*!\brief Level type
 \return The level of the Stratum
*/
inline Stratum::LevelType Stratum::level() const {return level_;}

/*!\brief Constituent species
  \return A vector containing the Species objects comprising the Stratum
*/
inline std::vector<Species> Stratum::allSpecies() const {
  return allSpecies_;}

/*!\brief Plant separation
  \return The plant separation attribute of the Stratum
*/
inline double Stratum::plantSep() const {return plantSep_;}

//operators

/*!\brief Inequality
  \param otherStratum
  \return true if and only if *this.level() < otherStratum.level()

  Reflects the natural ordering of strata bottom to top.
*/
inline bool Stratum::operator<(const Stratum& otherStratum) const {return level_ < otherStratum.level();}

/*!\brief Inequality
  \param otherStratum
  \return true if and only if *this.level() >= otherStratum.level()

  Reflects the natural ordering of strata bottom to top.
*/
inline bool Stratum::operator>=(const Stratum& otherStratum) const {return level_ >= otherStratum.level();}

/*!\brief Inequality
  \param otherStratum
  \return true if and only if *this.level() > otherStratum.level()

  Reflects the natural ordering of strata bottom to top.
*/
inline bool Stratum::operator>(const Stratum& otherStratum) const {return otherStratum.level() > level_;}

/*!\brief Inequality
  \param otherStratum
  \return true if and only if *this.level() <= otherStratum.level()

  Reflects the natural ordering of strata bottom to top.
*/
inline bool Stratum::operator<=(const Stratum& otherStratum) const {return otherStratum.level() <= level_;}

//other methods

/*!\brief Printing
  \return The name of the (level of the) Stratum.
*/
std::string inline Stratum::name() const {
  return levelStringMap.at(level_);
}


//printing 

/*!\brief Printing
  \return A formatted string describing the Stratum object
*/
inline std::string Stratum::printToString() const{
  std::string str;

  str = "Stratum: " + levelStringMap.at(level_) + "\n\n";
  str += "Plant separation (m): " + printPlantSep() + "\n";
  
  if (!allSpecies_.empty()){
    for(const auto& e : allSpecies_){
      str += "\nSpecies:\n";
      str += e.printToString();
    }
  }
  str += "\nEnd of stratum: " + levelStringMap.at(level_) + "\n";
  return str;
}

/*!\brief Printing
  \return A formatted string describing the plant separation attribute
*/
inline std::string Stratum::printPlantSep() const {
  char s[10];
  sprintf(s,"%.3f",plantSep_);
  return std::string(s);
}



#endif //STRATUM_INLINE_H
