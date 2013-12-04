#include <iostream>
#include <algorithm>
#include <iterator>
#include <string.h>

#include "pt.h"
#include "seg.h"
#include "line.h"
#include "ray.h"
#include "poly.h"
#include "ffm_numerics.h"

using std::vector;

// contains non-inline implementation code for the geometric classes 
// (eg Pt, Seg, Ray and Poly). Inline code for these classes is in 
// files *_inline.h which are included in *.h

/*!\brief Test for intersection of two Seg objects
  \param s (Seg)
  \param p (Pt)
  \return true if and only if *this intersects s.

  If true then places the Pt of intersection into p
*/
bool Seg::intersects(const Seg& s, Pt& p) const {

  //returns false if the segments are parallel. Also returns
  //false it they don't intersect. If they intersect then 
  //returns true and places coordinates of intersection into 
  //the second argument

  double thisLength = length();
  double sLength = s.length();

  if (ffm_numerics::almostZero(thisLength)) return s.contains(start_);
  if (ffm_numerics::almostZero(sLength)) return contains(s.start());

  Pt v = (s.vect()).perp()*(1/sLength); //v is normalised and perpendicular to s
  const double dotprod = vect()*v;

  if(ffm_numerics::almostZero(dotprod/thisLength)) 
    // the segments are paralled
    // don't change p and return false
    return false;

  //if we are here then the lines, but not necessarily the segments,
  //intersect somewhere, so calculate the point of intersection P. 
  //The computation is based on the following: 
  //if *this = AB and s = CD then
  //set OP = 0A + l1 * AB = OC + l2 * CD
  //and eliminate l2 by taking the inner product with v

  double l1 = (s.start_*v - start_*v)/dotprod;
  p = start_ + l1*vect();

  //compute l2
  double l2;
  if (fabs(s.vect().x()) >= fabs(s.vect().y()))
    l2 = (p - s.start_).x()/s.vect().x();
  else
    l2 = (p - s.start_).y()/s.vect().y();
    
  //the segments intersect iff l1 and l2 are in [0,1]
  //return 0 <= l1  && l1 <= 1 && 0 <= l2 && l2 <= 1;
  return ffm_numerics::geq(l1,0) && ffm_numerics::leq(l1,1) && 
    ffm_numerics::geq(l2,0) && ffm_numerics::leq(l2,1);
}

/*!\brief Test for intersection a Seg with a vector of Seg objects
  \param segs
  \return true if and only if *this intersects at least one Seg in segs.
*/
bool Seg::intersects(const std::vector<Seg>& segs) const {
  return Seg::intersects(segs.begin(),segs.end());
}

/*!\brief Test for intersection of a Seg with a range of Seg objects
  \param b (vector<Seg>::const_iterator)
  \param e (vector<Seg>::const_iterator)
  \return true if and only if *this intersects with at least one Seg in the range [b,e)
*/
bool Seg::intersects(std::vector<Seg>::const_iterator b,
		     std::vector<Seg>::const_iterator e
		     ) const {

  // brute force check whether member 
  // intersects any segment in 
  // the range [beg,end)

  Pt p; //dummy but needed for the function call

  for (auto i = b; i != e; ++i)
    if (intersects(*i,p)) 
      return true;
  
  return false;
}

/*!\brief Test for a Seg containing a Pt
  \param p (Pt)
  \return true if and only if p lies on *this
*/
bool Seg::contains(const Pt& p) const {
  Pt u = vect();
  Pt v = p - start_;

  //we just want to know if v = l*u for some l in [0,1]

  //case u is zero vector
  if (ffm_numerics::almostZero(u.norm()))
    return ffm_numerics::almostZero(v.norm());

  //if we are here then u is not zero vector

  //if u and v are not parallel return false
  if(!ffm_numerics::almostZero(u*(v.perp())))
    return false;

  //if we are here then u and v are parallel and u is not zero vector, so 
  //v = l*u for some l. Check the vale of l.
  if (fabs(u.x()) >= fabs(u.y()))
    return ffm_numerics::leq(0,v.x()/u.x()) && ffm_numerics::leq(v.x()/u.x(),1); 
  else
    return ffm_numerics::leq(0,v.y()/u.y()) && ffm_numerics::leq(v.y()/u.y(),1); 
}

