#ifndef RAY_H
#define RAY_H

#include <cmath>
#include <vector>

#include "pt.h"

class Seg;
class Line;
class Poly;

/*!\brief A Ray is a directed semi-infinite line. 

  A Ray has a definite starting Pt and a direction 
*/
class Ray {

 public:

  //constructors
  Ray();
  Ray(const Pt& start, const Pt& direction);
  Ray(const Pt& start, const double& angle);
  Ray(const Seg& segment);

  //accessors

  Pt start() const;
  Pt direction() const;

  //other methods

  double angle() const;
  bool contains(const Pt& p) const;
  bool intersects(const Seg& s, Pt& p) const;
  bool intersects(const Ray& r, Pt& p) const;
  bool intersects(const Line& l, Pt& p) const;
  bool intersects(const Poly& pol, Pt& p) const;
  std::vector<Pt> boundaryIntersections(const Poly& pol) const;
  std::vector<Seg> intersection(const Poly& pol) const;
  double intersectionLength(const Poly& pol) const;

private:
  Pt start_, direction_;

};

#include "ray_inline.h"

#endif //RAY_H
