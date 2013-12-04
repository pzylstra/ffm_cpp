#ifndef LINE_INLINE_H
#define LINE_INLINE_H

//constructors

/*!\brief Default constructor.
 */
inline Line::Line() : point_(), direction_(){}

/*!\brief Constructor 
  \param point
  \param direction

  
  Constructs a line through the Pt point parallel
  to the vector direction.
*/
inline Line::Line(const Pt& point, const Pt& direction) : point_(point), direction_(direction){}

/*!\brief Constructor
  \param point
  \param angle (in radians)


  Constructs the line through the Pt point with given 
  angle (measured in the counter-clockwise direction 
  relative to the positive x-axis
*/
inline Line::Line(const Pt& point, const double& angle) : point_(point),
						  direction_(Pt(cos(angle),sin(angle))) {}
//accessors

/*!\brief A point on the line.
  \return A point on the line.
*/
inline Pt Line::point() const {return point_;}

/*!\brief The direction of the line
  \return A vector (Pt) parallel to the line.
*/
inline Pt Line::direction() const {return direction_;}

//other methods

/*!\brief Does the Line contain a Pt?
  \param point
  \return true if and only if the Pt point is on the Line.
*/
inline bool Line::contains(const Pt& point) const {
  if (point == point_) return true;
  if (ffm_numerics::almostZero(direction_.norm())) return false;
  return ffm_numerics::almostZero(((point - point_).perp())*direction_);
}




#endif //LINE_INLINE_H
