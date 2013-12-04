#include <iostream>
#include <algorithm>
#include <iterator>
#include <string.h>

#include "pt.h"
#include "seg.h"
#include "line.h"
#include "ray.h"
#include "poly.h"
//#include "geometry.h"
#include "ffm_numerics.h"

using std::vector;

/*!\brief Determines if the Line intersects a Ray
  \param ray
  \param intersectionPoint
  \return true if and only if the Line intersects the Ray. 

  If the Line and Ray intersect the function places the point of intersection into intersectionPoint.
*/
bool Line::intersects(const Ray& ray, Pt& intersectionPoint) const {return ray.intersects(*this,intersectionPoint);}

bool Line::originOnLine(const double& angle, const Pt& targetPt, Pt& originPt) {
  //if there is a pt on *this such that a ray starting at that point with angle = angle would 
  //pass through targetPt, function returns true and sets originPt to such. Otherwise originPt is 
  //left unaltered and function return false
  if (contains(targetPt)){
    originPt = targetPt;
    return true;
  }
  Ray r(targetPt, angle + PI);
  if (!intersects(r, originPt))
    return false;
  return true;
}
