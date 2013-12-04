#ifndef POLY_INLINE_H
#define POLY_INLINE_H

#include "pt.h"

//constructors

/*!\brief Default constructor

  Constructs an empty polygon
*/
inline Poly::Poly() : vertices_(){}


//accessors

/*!\brief The vertices of the Poly

  \return A vector of points comprising the vertices of the Poly. The vertices will be oriented in the
  counter-clockwise direction and will not contain redundant vertices (for example three collinear points).
*/
inline std::vector<Pt> Poly::vertices() const {
  return vertices_;
}


#endif //POLY_INLINE_H
