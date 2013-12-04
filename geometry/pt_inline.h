#ifndef PT_INLINE_H
#define PT_INLINE_H

#include "ffm_numerics.h"

//constructors

/*!\brief Default constructor 

  Produces the point (-99,-99).
*/
inline Pt::Pt():x_(-99),y_(-99){}

/*!\brief Standard constructor
  \param x
  \param y


  Constructs the point (x,y).
*/
inline Pt::Pt(const double& x, const double& y): x_(x), y_(y){}

//accessors

/*!\brief x-coordinate
  \return The x-coordinate of the Pt
*/
inline double Pt::x() const {return x_;}

/*!\brief y-coordinate
  \return The y-coordinate of the Pt
*/
inline double Pt::y() const {return y_;}

//operators

/*!\brief Assignment operator
  \param rv The Pt that is to be added to *this
  \return A reference to the sum of *this and rv

  Adds rv to *this and also returns a reference to the sum so that 
  the expression can be used on the right hand side of assignment operations
*/
inline  Pt& Pt::operator+=(const Pt& rv){
    x_ += rv.x_;
    y_ += rv.y_;
    return *this;
  }

/*!\brief Assignment operator
  \param rv The Pt that is to be subtracted from *this
  \return A reference to the difference of *this and rv

  Subtracts rv from *this and also returns a reference to the difference so that 
  the expression can be used on the right hand side of assignment operations
*/
inline  Pt& Pt::operator-=(const Pt& rv){
    x_ -= rv.x_;
    y_ -= rv.y_;
    return *this;
  }

/*!\brief Vector addition.
  \param p
  \param q
  \return Vector sum of point1 and point2
*/
inline Pt operator+(const Pt& p, const Pt& q){ //vector addition
  return Pt(p.x_ + q.x_, p.y_ + q.y_);
}


/*!\brief Vector subtraction
  \param p
  \param q
  \return Vector difference of p and p, ie p - q
*/
inline Pt operator-(const Pt& p, const Pt& q){ //vector subtraction
  return Pt(p.x_ - q.x_, p.y_ - q.y_);
}


/*!\brief Inner product
  \param v
  \param w
  \return Scalar product of v and w
*/
inline double operator*(const Pt&v, const Pt& w){ //inner product
  return v.x_*w.x_ + v.y_*w.y_;
}


/*!\brief Scalar multiplication
  \param scal
  \param vec
  \return Scalar multiple scal*vec
*/
inline Pt operator*(const double& scal, const Pt& vec){ //scalar multiplication
  return Pt(scal*vec.x_, scal*vec.y_);
}

/*!\brief Scalar multiplication
  \param vec
  \param scal
  \return Scalar multiple scal*vec
*/
inline Pt operator*(const Pt& vec, const double& scal){ //scalar multiplication
  return Pt(scal*vec.x_, scal*vec.y_);
}

/*!\brief Test for equality. 
  \param rv
  \return true if and only if *this is equal to rv

  Uses ffm_numerics::almostEq(const double&, const double&) for comparison.
*/
inline bool Pt::operator==(const Pt& rv) const {
  return ffm_numerics::almostZero((*this - rv).norm());
}

/*!\brief Test for inequality. 
  \param rv 
  \return true if and only if *this is not equal to rv.

  Uses ffm_numerics::almostEq(const double&, const double&).
*/
inline bool Pt::operator!=(const Pt& rv) const {
  return !((*this)==rv);
}

/*!\brief Vector length
  \return The norm (length) of the vector.
*/
inline double Pt::norm() const{
  return sqrt(x_*x_ + y_*y_);
}

/*!\brief A perpendicular
  \return The vector rotated (anti-clockwise) by pi/2.
*/
inline Pt Pt::perp() const {
  //returns a vector (ie Pt) rotated by pi/2
  return Pt(-y_,x_);
  }

//printing

/*!\brief Printing 
  \return A formatted string describing the point.
*/
inline std::string Pt::printToString() const {
  char s[30];
  sprintf(s,"(%6.3f : %6.3f)", x_, y_);
  return std::string(s);
}


#endif //PT_INLINE_H
