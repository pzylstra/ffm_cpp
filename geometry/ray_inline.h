#ifndef RAY_INLINE_H
#define RAY_INLINE_H

#include "seg.h"

//constructors

/*!\brief Default constructor
 */
inline Ray::Ray() : start_(Pt()), direction_(Pt()){}

/*!\brief Constructor
  \param start 
  \param direction 
    

  Constructs a Ray with given direction and with start as its initial Pt.
  */
inline Ray::Ray(const Pt& start, const Pt& direction) : start_(start), direction_(direction){}

/*!\brief Constructor
  \param start (Pt)
  \param angle (radians, double)
    
    
  Constructs a Ray with given angle relative to the positive x-axis, and 
  with start as its initial Pt.
*/
inline Ray::Ray(const Pt& start, const double& angle) : start_(start), 
							direction_(Pt(cos(angle),sin(angle))) {}

/*!\brief Constructor
  \param segment

    
  Constructs a Ray which starts at the initial point of segment, and which 
  has the same direction as segment. 
*/
inline Ray::Ray(const Seg& segment) : start_(segment.start()), direction_(segment.vect()) {}

//accessors

/*!\brief Origin
  \return The initial point of the Ray.
*/
inline Pt Ray::start() const {return start_;}

/*!\brief Direction
  \return A vector (Pt) giving the direction of the Ray.
*/
inline Pt Ray::direction() const {return direction_;}

/*!\brief The angle of the Ray
  \return The angle of the Ray measured in radians counter-clockwise from the positive x-axis
*/
inline double Ray::angle() const {
  return atan2(direction_.y(), direction_.x());
}


#endif //RAY_INLINE_H
