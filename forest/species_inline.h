#ifndef SPECIES_INLINE_H
#define SPECIES_INLINE_H


//constructors

/*!\brief Default constructor*/
inline Species::Species() {}

  /*!\brief Basic constructor.
    \param composition 
    \param name
    \param crown
    \param live_leaf_moisture
    \param dead_leaf_moisture
    \param proportion_dead
    \param silica_free_ash_content
    \param ignition_temperature
    \param leaf_form
    \param leaf_thickness
    \param leaf_width
    \param leaf_length
    \param leaf_separation
    \param stem_order
    \param clump_diameter
    \param clump_separation


    Performs some basic consistency checks which if failed result in the construction 
    of a default Species object. In particular, at least one of silica_free_ash_content 
    and ignition_temperature must be specified with positive values. Note that composition 
    refers to the relative composition of the species within a Stratum, and as such is not 
    an intinsic property of a Species. However for convenience it is included as a Species 
    attribute. In practise this attribute will be modified when a Stratum object is 
    constructed, so that the compositions of all species in the Stratum object will sum to 1.  
  */
inline Species::Species(const double& composition,
			const std::string& name, 
			const Poly& crown,
			const double& live_leaf_moisture,
			const double& dead_leaf_moisture,
			const double& proportion_dead,
			const double& silica_free_ash_content,
			const double& ignition_temperature, 
			const LeafFormType& leaf_form, 
			const double& leaf_thickness,
			const double& leaf_width,
			const double& leaf_length,
			const double& leaf_separation,
			const double& stem_order,
			const double& clump_diameter,
			const double& clump_separation
			) 
  : composition_(std::max(composition,0.0)), 
    name_(name),
    crown_(crown),
    liveLeafMoisture_(live_leaf_moisture),
    deadLeafMoisture_(dead_leaf_moisture),
    propDead_(proportion_dead),
    silFreeAshCont_(silica_free_ash_content),
    ignitTemp_(ignition_temperature),
    leafForm_(leaf_form),
    leafThick_(leaf_thickness),
    leafWidth_(leaf_width),
    leafLength_(leaf_length),
    leafSep_(leaf_separation),
    stemOrder_(stem_order),
    clumpDiam_(clump_diameter),
    clumpSep_(clump_separation)
{
  bool okay = true;
  if (silica_free_ash_content <= 0) silFreeAshCont_ = -99;
  if (ignition_temperature <= 0) ignitTemp_ = -99;
  if(ignitTemp_ < 0 && silFreeAshCont_ < 0){
    okay = false;
  }
  if (name_.empty() || crown_.vertices().empty()) okay = false;
  if (leafThick_ <= 0 || leafWidth_ <= 0 || leafLength_ <= 0 || leafSep_ < 0) okay = false;
  if (stemOrder_ < 0 || clumpDiam_ <= 0 || clumpSep_ < 0 || propDead_ < 0) okay = false;

  if (!okay){
    composition_ = 0;
    name_.clear();
    crown_ = Poly();
    silFreeAshCont_ = -99;
    liveLeafMoisture_ = -99;
    deadLeafMoisture_ = -99;
    propDead_ = -99;
    ignitTemp_ = -99;
    leafThick_ = -99;
    leafWidth_ = -99;
    leafLength_ = -99;
    leafSep_ = -99;
    stemOrder_ = -99;
    clumpDiam_ = -99;
    clumpSep_ = -99;
  }
}

/*!\brief Constructs a species with hexagonal crown. 
  \param composition
  \param name
  \param hc The height of the bottom centre vertex
  \param he The height of the bottom outside vertices
  \param ht The height of the top outside vertices
  \param hp The height of the top centre vertex
  \param w The width
  \param live_leaf_moisture
  \param dead_leaf_moisture
  \param proportion_dead
  \param silica_free_ash_content
  \param ignition_temperature
  \param leaf_form
  \param leaf_thickness
  \param leaf_width
  \param leaf_length
  \param leaf_separation
  \param stem_order
  \param clump_diameter
  \param clump_separation

  
  Constructs hexagonal crown with vertices:
  (0,hc), (width/2,he), (width/2,ht), (0,hp), (-width/2,ht), (-width/2,he)
  as per Zylstra's original model. The same consistency checks as are 
  applied to the basic constructor are applied.
*/

inline Species::Species(const double& composition,
			const std::string& name, 
			const double& hc,
			const double& he,
			const double& ht,
			const double& hp,
			const double& w,
			const double& live_leaf_moisture,
			const double& dead_leaf_moisture,
			const double& proportion_dead,
			const double& silica_free_ash_content,
			const double& ignition_temperature, 
			const LeafFormType& leaf_form, 
			const double& leaf_thickness,
			const double& leaf_width,
			const double& leaf_length,
			const double& leaf_separation,
			const double& stem_order,
			const double& clump_diameter,
			const double& clump_separation
			) : 
  composition_(composition),
  name_(name),
  liveLeafMoisture_(live_leaf_moisture),
  deadLeafMoisture_(dead_leaf_moisture),
  propDead_(proportion_dead),
  silFreeAshCont_(silica_free_ash_content),
  ignitTemp_(ignition_temperature),
  leafForm_(leaf_form),
  leafThick_(leaf_thickness),
  leafWidth_(leaf_width),
  leafLength_(leaf_length),
  leafSep_(leaf_separation),
  stemOrder_(stem_order),
  clumpDiam_(clump_diameter),
  clumpSep_(clump_separation) {

  bool okay = true;
  if (silica_free_ash_content <= 0) silFreeAshCont_ = -99;
  if (ignition_temperature <= 0) ignitTemp_ = -99;
  if(ignitTemp_ < 0 && silFreeAshCont_ < 0){
    okay = false;
  }
  std::vector<Pt> v{Pt(0,hc), Pt(w/2,he), Pt(w/2,ht), Pt(0,hp), Pt(-w/2,ht), Pt(-w/2,he)};
  crown_ = Poly(v);
  if (name_.empty() || crown_.vertices().empty()) okay = false;
  if (leafThick_ <= 0 || leafWidth_ <= 0 || leafLength_ <= 0 || leafSep_ < 0) okay = false;
  if (stemOrder_ < 0 || clumpDiam_ <= 0 || clumpSep_ < 0 || propDead_ < 0) okay = false;

  if (!okay){
    name_.clear();
    crown_ = Poly();
    silFreeAshCont_ = -99;
    liveLeafMoisture_ = -99;
    deadLeafMoisture_ = -99;
    propDead_ = -99;
    ignitTemp_ = -99;
    leafThick_ = -99;
    leafWidth_ = -99;
    leafLength_ = -99;
    leafSep_ = -99;
    stemOrder_ = -99;
    clumpDiam_ = -99;
    clumpSep_ = -99;
    composition_ = 0;
  }
}

//accessors

/*!\brief Species composition
  \return The composition of the species within a Stratum (see basic constructor)
*/
inline double      Species::composition() const {return composition_;}

/*!\brief Species name
  \return The name of the species.
*/
inline std::string  Species::name() const {return name_;}

/*!\brief The crown
\return The Poly representing the species crown.
*/
inline Poly         Species::crown() const{return crown_;}

/*!\brief Live leaf moisture
  \return The live leaf moisture
*/
inline double       Species::liveLeafMoisture() const { return liveLeafMoisture_;}

/*!\brief Dead leaf moisture
  \return The dead leaf moisture
*/
inline double       Species::deadLeafMoisture() const { return deadLeafMoisture_;}

/*!\brief Silica free ash content
  \return The silica free ash content
*/
inline double       Species::silFreeAshCont() const {return silFreeAshCont_;}

/*!\brief Ignition temperature
  \return The ignition temperature
*/
inline double       Species::ignitTemp() const {return ignitTemp_;}

/*!\brief Leaf form
  \return The LeafFormType of the species
*/
inline Species::LeafFormType Species::leafForm() const {return leafForm_;}

/*!\brief Leaf thickness
  \return The leaf thickness
*/
inline double      Species::leafThick() const {return leafThick_;} 

/*!\brief Leaf width
  \return The leaf width
*/
inline double      Species::leafWidth() const {return leafWidth_;}

/*!\brief Leaf length
  \return The leaf length
*/
inline double      Species::leafLength() const {return leafLength_;}

/*!\brief Leaf separation
  \return The leaf separation
*/
inline double      Species::leafSep() const {return leafSep_;}

/*!\brief Stem order
  \return The stem order
*/
inline double      Species::stemOrder() const {return stemOrder_;}

/*!\brief Clump diameter
  \return The clump diameter
*/
inline double      Species::clumpDiam() const {return clumpDiam_;}

/*!\brief Clump separation
  \return The clump separation
*/
inline double      Species::clumpSep() const {return clumpSep_;}

/*!\brief Proportion of dead leaves
  \return The proportion of dead leaves (0 to 1)
*/
inline double      Species::propDead() const {return propDead_;}

//mutators

/*!\brief Sets composition of Species within a Stratum
  \param composition


  This method is used during the construction of a Stratum 
  to reset the compositions of its constituent Species
  so that they sum to 1
*/
inline void Species::composition(const double& composition) {
  composition_ = std::max(0.0,composition);
}


// other methods

/*!\brief Crown width
  \return Width of crown
*/
inline double Species::width() const {return crown_.width();}

/*!\brief Leaf moisture
  \return Weighted average of live and dead leaf moistures based on proportion of dead leaves
*/
inline double Species::leafMoisture() const {
  return (1 - propDead_)*liveLeafMoisture_ + propDead_*deadLeafMoisture_;
}

/*!\brief Flame duration model
  \return Flame duration in seconds
*/
inline double Species::flameDuration() const{
  return std::max(1.37*leafWidth_*leafThick_*1.0e6 + 1.61*leafMoisture() - 0.027,
	     ffm_settings::computationTimeInterval);
}

/*!\brief Modelled ignition temperature
  \param modelIt
  \return If modelIt is true or if ignitTemp() < 0 then returns modelled ignition 
  temperature using silica free ash content, otherwise return ignitTemp(). Will 
  return -99 if it is not possible to model ignition temperature and ignitTemp() < 0.
*/
inline double Species::ignitionTemp(bool modelIt) const{
  if (silFreeAshCont_ < 0 && ignitTemp_ < 0) return -99; 
  if (ignitTemp_ < 0 || (modelIt && silFreeAshCont_ > 0))  return 354 - 13.9*log(silFreeAshCont_*100) - 2.91*pow(log(silFreeAshCont_*100),2);
  return ignitTemp_;
}

/*!\brief Ignition delay time model
  \param temp Temperature at ignition point
  \return Ignition delay time 
*/
inline double Species::ignitionDelayTime(const double& temp) const{
  //=(100*D2)/((IF(D4="Round",4,2))/D5)
  double m = 100*leafMoisture()*leafThick_*1000/(leafForm_ == ROUND_LEAF ? 4 : 2);
  //=((100168.23*(D40^-2.11)*D$19)+6018087.86*(D40^-2.39))*D64
  return 100168.23*pow(temp,-2.11)*m + 6018087.86*pow(temp,-2.39);
}

/*!\brief Determines whether species should be regarded as a grass.
  \return true if and only if Species should be regarded as grass

  In practise this is used only in the Near-surface layer.
*/
inline bool Species::isGrass() const {
  return propDead_ >= 0.5 && leafThick_ < 0.00035;
}

/*!\brief Leaf flame length model
  \return Leaf flame length (m)
*/
inline double Species::leafFlameLength() const {

  double area = 0.5*leafWidth_*leafLength_; 
  double sqRootArea = pow(area,0.5);
  double cubeRootArea = pow(area,1/3.0);
  if(leafMoisture() < (17.5*cubeRootArea - 52.5*sqRootArea - 0.0027)/0.277)
    return 1.75*cubeRootArea - 0.0277*leafMoisture() - 0.00027;
  else
    return 5.25*sqRootArea;
}

/*!\brief Leaf density model
  \return Leaves per clump
*/
inline double Species::leavesPerClump() const {
  return 0.88*pow(clumpDiam_*stemOrder_/leafSep_,1.18);
}

//merged leaf flame length model (Zylstra thesis Eq 5.76)
//we use the average of the number of clumps that will be 
//traversed by the ignited segment, Eq 5.63 of Zylstra's thesis 

  ///\brief Flame length model. 
  ///
  ///Does merging of leaf flame lengths but 
  ///does not do lateral merging of plant flames.
inline double Species::flameLength(const double& lengthIgnitedSeg) const {
  if (ffm_numerics::almostZero(lengthIgnitedSeg))
    return 0;
  double numLeaves = leavesPerClump()*lengthIgnitedSeg/(clumpDiam_ + clumpSep_); //cf Eq 5.63
  return std::max(lengthIgnitedSeg,
		  pow(pow(leafFlameLength()*pow(numLeaves,0.4) + lengthIgnitedSeg,4) + 
		      pow(lengthIgnitedSeg,4),0.25));
}

/*!\brief Leaf area index model
  \return Leaf area index of Species
*/
inline double Species::leafAreaIndex() const {
  return leafWidth_*leafLength_/2.0                     //one side leaf area
    * leavesPerClump()                        //leaves per clump
    * crown_.volumeOfRev()                             //volume of crown
    / (4.0/3.0*PI*pow((clumpDiam_ + clumpSep_)*0.5,3))    //volume of clump
    / (PI*pow(0.5*crown_.width(),2));                      //area covered on ground
}


/*!\brief Equality operator
  \param anotherSpecies
  \return true if and only if all attributes are equal with the possible 
  exception of name
*/
inline bool Species::operator==(const Species& anotherSpecies) const {
  return  
    composition_ == anotherSpecies.composition() &&
    crown_ == anotherSpecies.crown() &&
    ffm_numerics::almostEq(liveLeafMoisture_, anotherSpecies.liveLeafMoisture()) &&
    ffm_numerics::almostEq(silFreeAshCont_, anotherSpecies.silFreeAshCont()) &&
    ffm_numerics::almostEq(ignitTemp_, anotherSpecies.ignitTemp()) &&
    ffm_numerics::almostEq(leafForm_, anotherSpecies.leafForm()) &&
    ffm_numerics::almostEq(leafThick_, anotherSpecies.leafThick()) &&
    ffm_numerics::almostEq(leafWidth_, anotherSpecies.leafWidth()) &&
    ffm_numerics::almostEq(leafLength_, anotherSpecies.leafLength()) &&
    ffm_numerics::almostEq(leafSep_, anotherSpecies.leafSep()) &&
    ffm_numerics::almostEq(stemOrder_, anotherSpecies.stemOrder()) &&
    ffm_numerics::almostEq(clumpSep_, anotherSpecies.clumpSep()) &&
    //ffm_numerics::almostEq(clumpDiam_, anotherSpecies.clumpDiam()) &&
    ffm_numerics::almostEq(propDead_, anotherSpecies.propDead());
}

/*!\brief Almost equality operator
  \param anotherSpecies
  \return true if and only if all attributes are equal with the possible 
  exception of name, crown and clump diameter

  When a Stratum fire is under consideration, the fire is considered to 
  burn through a Species object which has a large parallelogram-shaped 
  crown, and a modified clump diameter. This method allows the association
  of this Species object with the original plant Species object.
*/

inline bool Species::sameSpecies(const Species& anotherSpecies) const {
  return  
    //composition_ == anotherSpecies.composition() &&
    //crown_ == anotherSpecies.crown() &&
    ffm_numerics::almostEq(liveLeafMoisture_, anotherSpecies.liveLeafMoisture()) &&
    ffm_numerics::almostEq(silFreeAshCont_, anotherSpecies.silFreeAshCont()) &&
    ffm_numerics::almostEq(ignitTemp_, anotherSpecies.ignitTemp()) &&
    ffm_numerics::almostEq(leafForm_, anotherSpecies.leafForm()) &&
    ffm_numerics::almostEq(leafThick_, anotherSpecies.leafThick()) &&
    ffm_numerics::almostEq(leafWidth_, anotherSpecies.leafWidth()) &&
    ffm_numerics::almostEq(leafLength_, anotherSpecies.leafLength()) &&
    ffm_numerics::almostEq(leafSep_, anotherSpecies.leafSep()) &&
    ffm_numerics::almostEq(stemOrder_, anotherSpecies.stemOrder()) &&
    ffm_numerics::almostEq(clumpSep_, anotherSpecies.clumpSep()) &&
    ffm_numerics::almostEq(propDead_, anotherSpecies.propDead());
}

/*!\brief Printing
  \return A formatted string describing the Species object
*/
inline std::string Species::printToString() const {
  char s[20];
  std::string tmp;
  std::string str;
  str = "Composition: " + printComposition() + "\n";
  str += "Name: " + name_ + "\n";
  str += "Crown (coordinates in m):" + printCrown() + "\n";
  str += "Live leaf moisture: " + printLiveLeafMoisture() + "\n";
  str += "Dead leaf moisture: " + printDeadLeafMoisture() + "\n";
  str += "Proportion dead: " + printPropDead() + "\n";
  str += "Silica free ash content: " + printSilFreeAshCont() + "\n";
  str += "Ignition temperature (C): " + printIgnitTemp() + "\n";
  str += "Leaf form: " + printLeafForm() + "\n";
  str += "Leaf thickness (m): " + printLeafThick() + "\n";
  str += "Leaf width (m): " + printLeafWidth() + "\n";
  str += "Leaf length (m): " + printLeafLength() + "\n";
  str += "Leaf separation (m): " + printLeafSep() + "\n";
  str += "Stem order: " + printStemOrder() + "\n";
  str += "Clump separation (m): " + printClumpSep() + "\n";
  str += "Clump diameter (m): " + printClumpDiam() + "\n";
  return str;
}

/*!\brief Returns formatted string describing attribute
 */
inline std::string Species::printComposition() const{
  char s[10];
  sprintf(s,"%.3f",composition_);
  return std::string(s);
}

/*!\brief Returns formatted string describing attribute
 */
inline std::string Species::printCrown() const {
  return crown_.printToString();
}

/*!\brief Returns formatted string describing attribute
 */
inline std::string Species::printLiveLeafMoisture() const{
  char s[10];
  sprintf(s,"%.3f",liveLeafMoisture_);
  return std::string(s);
}

/*!\brief Returns formatted string describing attribute
 */
inline std::string Species::printDeadLeafMoisture() const{
  char s[10];
  sprintf(s,"%.3f",deadLeafMoisture_);
  return std::string(s);
}

/*!\brief Returns formatted string describing attribute
 */
inline std::string Species::printPropDead() const{
  char s[10];
  sprintf(s,"%.3f",propDead_);
  return std::string(s);
}

/*!\brief Returns formatted string describing attribute
 */
inline std::string Species::printSilFreeAshCont() const{
  char s[10];
  sprintf(s,"%.3f",silFreeAshCont_);
  return std::string(s);
}

/*!\brief Returns formatted string describing attribute
 */
inline std::string Species::printIgnitTemp() const{
  char s[10];
  sprintf(s,"%.1f",ignitTemp_);
  return std::string(s);
}

/*!\brief Prints result of ignition temperature model
  \return A formatted string describing the return value
  of the method ignitionTemp()
 */
inline std::string Species::printIgnitionTemp() const{
  char s[10];
  sprintf(s,"%.1f",ignitionTemp());
  return std::string(s);
}

/*!\brief Returns formatted string describing attribute
 */
inline std::string Species::printLeafForm() const{
  return Species::ROUND_LEAF ? "round" : "flat";
}

/*!\brief Returns formatted string describing attribute
 */
inline std::string Species::printLeafThick() const{
  char s[10];
  sprintf(s,"%.5f",leafThick_);
  return std::string(s);
}

/*!\brief Returns formatted string describing attribute
 */
inline std::string Species::printLeafWidth() const{
  char s[10];
  sprintf(s,"%.5f",leafWidth_);
  return std::string(s);
}

/*!\brief Returns formatted string describing attribute
 */
inline std::string Species::printLeafLength() const{
  char s[10];
  sprintf(s,"%.5f",leafLength_);
  return std::string(s);
}

/*!\brief Returns formatted string describing attribute
 */
inline std::string Species::printLeafSep() const{
  char s[10];
  sprintf(s,"%.5f",leafSep_);
  return std::string(s);
}

/*!\brief Returns formatted string describing attribute
 */
inline std::string Species::printStemOrder() const{
  char s[10];
  sprintf(s,"%.3f",stemOrder_);
  return std::string(s);
}

/*!\brief Returns formatted string describing attribute
 */
inline std::string Species::printClumpSep() const{
  char s[10];
  sprintf(s,"%.5f",clumpSep_);  
  return std::string(s);
}

/*!\brief Returns formatted string describing attribute
 */
inline std::string Species::printClumpDiam() const{
  char s[10];
  sprintf(s,"%.5f",clumpDiam_);
  return std::string(s);
}


#endif
	   
			
			
			
