#ifndef SEG_H
#define SEG_H

#include<vector>

#include "pt.h"

/*!\brief Represents directed line segments. 

  These begin
  at a start point and terminate at an end point.
*/
class Seg {

  friend class Poly;

public:

  //constructors

  Seg();
  Seg(const Pt& start, const Pt& end);
  Seg(const Pt& start, const double& angle, const double& length);

  //accessors

  Pt start() const;
  Pt end() const;

  //other methods

  double length() const;
  Pt vect() const;
  bool intersects(const Seg& otherSegment, Pt& p) const;
  bool intersects(const std::vector<Seg>& segs) const;
  bool intersects(std::vector<Seg>::const_iterator beg,
		  std::vector<Seg>::const_iterator end) const;
  bool contains(const Pt&) const;

  //printing methods

  std::string printToString() const;

private:

  Pt start_, end_;

};

#include "seg_inline.h"

#endif //SEG_H
