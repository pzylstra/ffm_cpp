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

// contains non-inline implementation code for the geometric classes 
// (eg Pt, Seg, Ray and Poly). Inline code for these classes is in 
// files *_inline.h which are included in *.h



/*!\brief Is a point on the Ray?
  \param p
  \return true if and only if the Pt p is on the Ray.
*/
bool Ray::contains(const Pt& p) const{
  if (start_ == p) return true; 
  if(ffm_numerics::almostZero(direction_.norm())) return (start_ == p);
  Pt v(p - start_);
  if(!ffm_numerics::almostZero(v.perp()*direction_)) return false;
  if(fabs(v.x()) > 0)
    return (direction_.x() >= 0 && v.x() >= 0) || (direction_.x() <= 0 && v.x() <= 0) ;
  else
    return (direction_.y() >= 0 && v.y() >= 0) || (direction_.y() <= 0 && v.y() <= 0) ;
}


/*!\brief Intersection of Ray and Seg
  \param s 
  \param p 
  \return true if and only if the Ray intersects the Seg s.

  If they intersect then the method places the point of intersection
  into the Pt reference p.
*/
bool Ray::intersects(const Seg& s, Pt& p) const {
  //returns false if the ray and segment are parallel. Also returns
  //false it they don't intersect. If they intersect then 
  //returns true and places coordinates of intersection into 
  //the second argument

  double slength = s.length();
  double dlength = direction_.norm();

  if(ffm_numerics::almostZero(dlength))
    return false; // the ray has no direction
  
  if(ffm_numerics::almostZero(slength)){ 
    //the segment has no length, ie is a point. Does the ray pass through it?
    Pt v(s.start() - start_);
    if(ffm_numerics::almostEq(v*direction_, v.norm()*dlength)){
      p = s.start();
      return true;
    }
    return false;
  }
  
  Pt d = direction_*(1/dlength); //normalised direction vector
  Pt v = (s.vect()).perp()*(1/slength); //v is normalised and perpendicular to s

  const double dotprod = d*v;

  if(ffm_numerics::almostZero(dotprod)) 
    // the ray and segment are parallel
    // don't change p and return false
    return false;

  //if we are here then the lines, but not necessarily the segments,
  //intersect somewhere, so calculate the point of intersection P. 
  double l1 = (s.start()*v - start_*v)/dotprod;
  p = start_ + l1*d;

  //compute l2
  double l2;
  if (fabs(s.vect().x()) >= fabs(s.vect().y()))
    l2 = (p - s.start()).x()/s.vect().x();
  else
    l2 = (p - s.start()).y()/s.vect().y();
    
  //the ray intersects the segment iff l1 >= 0 and l2 is in [0,1]
  return ffm_numerics::geq(l1,0) && ffm_numerics::geq(l2,0) && ffm_numerics::leq(l2,1);
}


/*!\brief Intersection of two rays
  \param r
  \param p
  \return true if and only if the *this intersect the Ray r.

  If they intersect then the method places the point of intersection
  into the Pt reference p.
*/
bool Ray::intersects(const Ray& r, Pt& p) const {
  double thisLength = direction_.norm();
  double thatLength = r.direction_.norm();

  if(ffm_numerics::almostZero(thisLength) || ffm_numerics::almostZero(thatLength))
    return false; 
    
  Pt d = direction_*(1/thisLength); //normalised direction_ 
  Pt v = r.direction_.perp()*(1/thatLength); //normalised and perpendicular to r

  const double dotprod = d*v;

  if(ffm_numerics::almostZero(dotprod)) 
    // the rays are parallel
    // don't change p and return false
    return false;

  //if we are here then the lines, but not necessarily the rays,
  //intersect somewhere, so calculate the point of intersection P. 
  p = start_ + ((r.start_ - start_)*v)/dotprod*d;
  return contains(p) && r.contains(p);
}


/*!\brief Intersection of Ray and Line
  \param l
  \param p
  \return true if and only if the *this intersect the Line l.

  If they intersect then the method places the point of intersection
  into the Pt reference p.
*/
bool Ray::intersects(const Line& l, Pt& p) const {
  if (l.contains(start_)) {
    p = start_;
    return true;
  }

  double thisLength = direction_.norm();
  double thatLength = l.direction().norm();
  if(ffm_numerics::almostZero(thisLength) || ffm_numerics::almostZero(thatLength))
    return false; 
    
  Pt d = direction_*(1/thisLength); //normalised direction_ 
  Pt v = l.direction().perp()*(1/thatLength); //normalised and perpendicular to l

  const double dotprod = d*v;

  if(ffm_numerics::almostZero(dotprod)) 
    // the ray and line are parallel
    // don't change p and return false
    return false;

  //if we are here then the lines intersect somewhere, so 
  //calculate the point of intersection p. 
  double a = ((l.point() - start_)*v)/dotprod;
  p = start_ + ((l.point() - start_)*v)/dotprod*d;
  //does the ray contain the pt?
  return contains(p);

}

/*!\brief Intersection of Ray and Poly
  \param pol
  \param p
  \return true if and only if the Ray intersects pol.

  If the Ray intersects  pol then places the nearest point of intersection
  to the start of the Ray into the Pt reference p.
*/
bool Ray::intersects(const Poly& pol, Pt& p) const {
  //if ray intersects pol, p is the first point in pol that ray sees
  std::vector<Pt> pts = boundaryIntersections(pol);
  if (pts.empty()) return false;
  p =  *min_element(pts.begin(),pts.end(),
		    [this](Pt p1, Pt p2){return (start_ - p1).norm() < (start_ - p2).norm();});
  return true;
}

/*!\brief Points of intersection of Ray and Poly boundary
  \param pol
  \return A Pt vector comprising the intersections of the Ray with the boundary of pol.
*/
std::vector<Pt> Ray::boundaryIntersections(const Poly& pol) const {
  std::vector<Pt> ret;
  //works okay because of way Poly's are constructed
  //maybe look at later?
  auto verts = pol.vertices();
  if(verts.empty()) return ret;
  Pt p;
  auto i = verts.begin();
  Pt s(*(i++));
  while (i < verts.end()){
    if (intersects(Seg(s,*i), p))
      if ( ret.empty() || !(p == ret.back()) )
	ret.push_back(p);
    s = *(i++);
  }
  if(intersects(Seg(s,verts.front()),p)) 
    if ( ret.empty() || !(p == ret.back() || p == ret.front()))
      ret.push_back(p);
  return ret;
}



/*!\brief Segments of intersection of a Ray and a Poly
  \param pol
  \return A vector of segments comprising the intersections of the Ray with the pol.
*/
std::vector<Seg> Ray::intersection(const Poly& pol) const {
  //returns a vector<Seg> which is the intersection of the Ray with the Poly. 
  //note that the boundaries of the polygon are included in it
  std::vector<Seg> retValue;
  std::vector<Pt> bdryIntsct = boundaryIntersections(pol);
  
  if (bdryIntsct.empty()) return retValue;

  //sort intersection points from furthest to closest
  sort(bdryIntsct.begin(), bdryIntsct.end(), 
       [this](Pt p1, Pt p2){return (p1 - start()).norm() > (p2 - start()).norm();});

  //a lambda to compute whether a vector v crosses the boundary at a vertex
  auto crossesAtVertex = [](Seg s1, Seg s2, Pt v) -> bool {
    if (ffm_numerics::almostZero(v*s1.vect().perp()) || ffm_numerics::almostZero(v*s2.vect().perp())){
      if (ffm_numerics::geq(s1.vect().perp()*s2.vect(), 0)) return true;
      else return false;
    }
    if (ffm_numerics::lt(0,(v*s1.vect().perp())*(v*s2.vect().perp()))) return true;
    else return false;
  };
  //loop through intersection points starting from the furthermost, 
  //so initially the ray is outside the polygon
  bool inside = false;
  Pt startPt, endPt;
  bool crosses;
  for (const Pt& p : bdryIntsct){
    //check to see if ray crosses
    if (!pol.hasVertex(p))
      //if p is not a vertex then the ray definitely crosses
      crosses = true;
    else {
      //if p is a vertex then we check to see if it crosses
      std::vector<Seg> segs(pol.adjoiningSegments(p));
      crosses = crossesAtVertex(segs.front(), segs.back(), direction_);
    }

    if (crosses){
      if (!inside)
	endPt = p;
      else {
	startPt = p;
	retValue.push_back(Seg(startPt, endPt));
      }
      inside = !inside;
    }
  }

  //now deal with the start point of the Ray
  if (inside && start_ != endPt){
    startPt = start_;
    retValue.push_back(Seg(startPt, endPt));
  }

  //reverse the order of the vector and return it
  reverse(retValue.begin(), retValue.end());
  return retValue;
}


/*!\brief Length of intersection of Ray with Poly
  \param pol
  \return The distance between the nearest intersection of the Ray and pol to the furthest 
  such intersection, neglecting degenerate intersections at vertices
*/
double Ray::intersectionLength(const Poly& pol) const {
  std::vector<Pt> bdryIntsct(boundaryIntersections(pol));
  if (bdryIntsct.empty()) return 0;
  //sort intersection points from furthest to closest
  sort(bdryIntsct.begin(), bdryIntsct.end(), 
       [this](Pt p1, Pt p2){return (p1 - start()).norm() > (p2 - start()).norm();});

  //a lambda to compute whether a vector v crosses the boundary at a vertex
  auto crossesAtVertex = [](Seg s1, Seg s2, Pt v) -> bool {
    if (ffm_numerics::almostZero(v*s1.vect().perp()) || ffm_numerics::almostZero(v*s2.vect().perp())){
      if (ffm_numerics::geq(s1.vect().perp()*s2.vect(), 0)) return true;
      else return false;
    }
    if (ffm_numerics::lt(0,(v*s1.vect().perp())*(v*s2.vect().perp()))) return true;
    else return false;
  };

  bool foundEnd = false;
  bool inside = false;
  bool crosses = false;
  Pt   startPt = start_, endPt = start_, tmpPt = start_;

  for (const Pt& p : bdryIntsct){
    //check to see if ray crosses
    if (!pol.hasVertex(p))
      //if p is not a vertex then the ray definitely crosses
      crosses = true;
    else {
      //if p is a vertex then we check to see if it crosses
      std::vector<Seg> segs(pol.adjoiningSegments(p));
      crosses = crossesAtVertex(segs.front(), segs.back(), direction_);
    }
    if (crosses){
      inside = !inside;
      if (!foundEnd){
	startPt = endPt = p;
	foundEnd = true;
      }
      if (!inside) startPt = p;
      tmpPt = p;
    }
  }
  //now deal with the start point of the Ray
  if (inside && start_ != tmpPt){
    startPt = start_;
  }
  return (startPt - endPt).norm();
}


