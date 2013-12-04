#ifndef POLY_H
#define POLY_H

#include <string>
#include <vector>
class Pt;
class Seg;

/*!\brief Holds and manipulates data representing plane polygons
 */
class Poly {
public:
  //constructors 

  Poly();
  Poly(const std::vector<Pt>& vertices);

  //accessors
  std::vector<Pt> vertices() const;

  //operators
  bool operator==(const Poly&) const;
  bool operator!=(const Poly&) const;
  Poly& operator+=(const Pt& v);
  Poly operator+(const Pt& v);

  //other methods


  double width() const;
  double top() const;
  double bottom() const;
  double left() const;
  double right() const;
  double centreTop() const;
  double centreBottom() const;
  double rightTop() const;
  double rightBottom() const;
  Pt centroid() const;
  Pt pointInBase(const double& x) const;
  std::vector<Seg> adjoiningSegments(const Pt& point) const;
  double volumeOfRev() const;
  std::string printToString() const;
  bool hasVertex(const Pt& p) const;

private:
  std::vector<Pt> vertices_;

};


#include "poly_inline.h"

#endif
