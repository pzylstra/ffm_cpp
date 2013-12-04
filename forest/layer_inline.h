#ifndef LAYER_INLINE_H
#define LAYER_INLINE_H

//constructors

/*!\brief Default constructor 

  Constructs empty layer.
*/
inline Layer::Layer() : bottom_(0), top_(0), levels_() {}

/*!\brief Standard constructor.
  \param bottom The vertical coordinate of the bottom of the Layer
  \param top The vertical coordinate of the top of the Layer
  \param levels A vector of Stratum::LevelType containing the levels that are 
  found in the layer
*/
inline Layer::Layer(const double& bottom, const double& top, const std::vector<Stratum::LevelType>& levels)
  : bottom_(bottom), top_(top), levels_(levels) {}

//other methods

/*!\brief The bottom of the layer
  \return Height of bottom of the layer (m).
*/
inline double Layer::bottom() const {return bottom_;}

/*!\brief The top of the layer
  \return Height of top of the layer (m).
*/
inline double Layer::top() const {return top_;}

/*!\brief The strata found in the layer
  \return A vector of type Stratum::LevelType containing the levels that are found 
  in the Layer
*/
inline std::vector<Stratum::LevelType> Layer::levels() const {return levels_;}

#endif //LAYER_INLINE_H
