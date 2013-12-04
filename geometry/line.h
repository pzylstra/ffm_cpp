#ifndef LINE_H
#define LINE_H

#include "pt.h"

class Ray;

/*!\brief Holds and manipulates data representing lines in 2-space
 */
class Line{

public:

  //Constructors

  Line();
  Line(const Pt& point, const Pt& direction);
  Line(const Pt& point, const double& angle);

  //accessors

  Pt point() const;
  Pt direction() const;

  //other methods
  bool intersects(const Ray& ray, Pt& intersectionPoint) const;
  bool contains(const Pt& p) const;

  bool originOnLine(const double& angle, const Pt& targetPt, Pt& originPt);

private:

  Pt point_;
  Pt direction_;
};

#include "line_inline.h"

#endif //LINE_H
