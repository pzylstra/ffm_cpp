#ifndef PT_H
#define PT_H

#include <cmath>
#include <string>


/*!\brief Represents both positions and 2-d vectors
  
  The Pt class serves to hold both points (ie physical positions)
  and 2-d vectors, hence the vector addition, 
  subtraction and inner product operators and
  the perp() and norm() functions. Consequently the terms point
  and vector will be used interchangeably when referring to an 
  instance of the Pt class.
*/ 

class Pt {

  friend Pt operator+(const Pt&, const Pt&);     //vector addition
  friend Pt operator-(const Pt&, const Pt&);     //vector subtraction
  friend double operator*(const Pt&, const Pt&); //inner product
  friend Pt operator*(const double&, const Pt&); //scalar multiplication
  friend Pt operator*(const Pt&, const double&); //scalar multiplication

public:
  // constructors

  Pt();
  Pt(const double& x, const double& y);


  //accesssors

  double x() const;
  double y() const;

  //operators
  
  Pt& operator+=(const Pt&);
  Pt& operator-=(const Pt&);
  bool operator==(const Pt& otherPoint) const;
  bool operator!=(const Pt& otherPoint) const;

  // other functions

  double norm() const;
  Pt perp() const;     
  std::string printToString() const;

private:
  double x_, y_;

};
Pt operator+(const Pt& p, const Pt& q);
Pt operator-(const Pt& p, const Pt& q);
double operator*(const Pt& v, const Pt& w);
Pt operator*(const double& scal, const Pt& vec);
Pt operator*(const Pt& vec, const double& scal);


#include "pt_inline.h"

#endif
