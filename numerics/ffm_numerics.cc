#include "ffm_numerics.h"
#include <cmath>

/*!\brief Useful numerical utilites*/
namespace ffm_numerics{

  /*!\brief Equality for floats
    \param a
    \param b
    \return true if and only if a may be considered to be 
    equal to b
  */
  bool almostEq(const double& a, const double& b){
    
    double absdiff = fabs(a-b);
    if (absdiff <= abs_epsilon)
      return true;

    double A = fabs(a), B = fabs(b);
    return absdiff <= (A < B ? B : A) * rel_epsilon;
  }

  /*!\brief Float comparison with zero
    \param a
    \return true if and only if a may be considered equal to zero
*/
  bool almostZero(const double& a){
    return fabs(a) <= abs_epsilon;
  }

  /*!\brief Float comparison
    \param a
    \param b
    \return true if and only if a may be considered to be greater than
    or equal to b
  */
  bool geq(const double& a, const double& b){
    return almostEq(a,b) || (a > b);
  }

  /*!\brief Float comparison
    \param a
    \param b
    \return true if and only if a may be considered to be less than
    or equal to b
  */
  bool leq(const double& a, const double& b){
    return almostEq(a,b) || (a < b);
  }
		 
  /*!\brief Float comparison
    \param a
    \param b
    \return true if and only if a may be considered to be less than b
  */
  bool lt(const double& a, const double& b){
    return !geq(a,b);
  }

  /*!\brief Float comparison
    \param a
    \param b
    \return true if and only if a may be considered to be greater than b
  */
  bool gt(const double& a, const double& b){
    return !leq(a,b);
  }

  /*!\brief Clamp small values to zero
    \param a
    \return 0.0 if almostZero(a) is true, otherwise returns a unchanged
  */
  double clampToZero(const double& a) {
    return almostZero(a) ? 0.0 : a;
  }
}
