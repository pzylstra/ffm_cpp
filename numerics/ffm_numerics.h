#ifndef INCLUDED_FFM_NUMERICS_H
#define INCLUDED_FFM_NUMERICS_H

#include <cmath>

/*!\brief Holds some useful numerical utilities
 */
namespace ffm_numerics{

  /*!\brief Constant for float comparison

    Floats will be equal if their absolute difference is 
    less than or equal to abs_epsilon
  */
  const double abs_epsilon = 1.0E-9;

  /*!\brief Constanf for float comparison
    
    Large floats will be equal if 
    abs(x - y) <= max(abs(x), abs(y)) * rel_epsilon
  */
  const double rel_epsilon = 1.0E-9;

  bool almostEq(const double&, const double&);
  bool almostZero(const double&);
  bool geq(const double&, const double&);
  bool leq(const double&, const double&);
  bool lt(const double&, const double&);
  bool gt(const double&, const double&);
}

const double PI = 4*atan(1);

#endif
