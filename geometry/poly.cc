#include <vector>
#include <algorithm>

#include "seg.h"
#include "ray.h"
#include "poly.h"


/*!\brief Standard constructor

  \param verts

  Will construct the default (empty) poly 
  if the vector verts leads to self-intersection.
  Note also that redundant vertices will be removed, so it is possible that 
  a call to Poly::vertices() will not return the same set of vertices as was used
  to construct the Poly
*/
Poly::Poly(const std::vector<Pt>& verts) : vertices_() {

  // this constuctor removes any redundant vertices 
  // in the input and stores the polygon vertices in positive
  // orientation (ie anti-clockwise). If there are 
  // any self-intersections then the constructor
  // produces an empty polygon

  // the orientation is checked using the well-known 
  // formula:
  //
  // area = 0.5*sum(i=0,n-1){x_i*y_(i+1) - x_(i+1)*y_i}
  // where (x_i, y_i) are the vertices and x_n = x_0, y_n = y_0.
  //
  // This formula is valid for any non self-intersecting
  // polygon and returns a positive result iff the 
  // orientation is anti-clockwise. 

  Seg seg;
 std::vector<Seg> segs; //vector of sides as they are formed
                    //used to check for self-intersections
    if(verts.size() < 3){
    //not enough vertices
    return;
  }
  // initially vertices is empty. We loop over the input vertices 
  // (except the last) and check to see if the input
  // vertex is redundant. If it is then we proceed to next iteration.
  // If it is not redundant then we check for self-intersections
  // and if not, add the vertex to vertices, the segment to segs
  // and accumulate area. The last vertex is a slightly
  // separate case and is handled at the end.
    seg.start_ = verts.back();
  auto i = verts.begin();
  double area = 0;
  while (i != verts.end() - 1) {
    //check to see if vertex is redundant
    seg.end_ = *(i + 1);
    if(!seg.contains(*i)){
      //the vertex is not redundant
      //so add it to vertices
      vertices_.push_back(*i);
      //if this is not the first vertex added
      //then process the segment that ends
      //with this vertex
      if(vertices_.size() > 1){
	seg.end_ = *i;
	//check for intersections and backtracking
	if(segs.size() > 1 && seg.intersects(segs.begin(),segs.end()-1)){
	  //self intersecting polygon
	  vertices_.clear();
	  return;
	}
	if(!segs.empty() &&
	   ffm_numerics::almostEq((seg.vect())*(segs.back().vect()),
				  -seg.length()*segs.back().length())){
	  //the new segment backtracks over the previous one
	  vertices_.clear();
	  return;
	}
	//there are no intersections, put seg in segs and add the 
	//contribution to the area calculation
	segs.push_back(seg);
	area += seg.start_.x() * seg.end_.y() - seg.end_.x() * seg.start_.y();
	
      } //end of segment processing
      //update the start point of the next segment 
      seg.start_ = *i; 
    } //end of check for vertex redundancy
    ++i;
  }
  //process the the last input vertex
  i = verts.end() - 1;
  if(vertices_.size() < 2){
    //too few vertices
    vertices_.clear();
    return;
  }
  //check for redundancy
  seg.end_ = vertices_.front();
  if(!seg.contains(*i)) {
    //the vertex is not redundant, add it to vertices
    //and process the segment that ends with this vertex
    vertices_.push_back(*i);
    seg.end_ = *i;

    if(segs.size() > 1 && seg.intersects(segs.begin(),segs.end()-1)){
      //self intersecting polygon
      vertices_.clear();
      return;
    }
    if(!segs.empty() && 
       ffm_numerics::almostEq((seg.vect())*(segs.back().vect()),
			      -seg.length()*segs.back().length())){
      //the new segment backtracks over the previous one
      vertices_.clear();
      return;
    }
    //if there are no intersections, put seg in segs and add the 
    //contribution to the area calculation
    segs.push_back(seg);
  }
  //that's all the vertices done, process the last segment
  seg.start_ = vertices_.back();
  seg.end_ = vertices_.front();
  //check for intersections
  if(segs.size() > 1 && seg.intersects(segs.begin()+1,segs.end()-1)){
    vertices_.clear();
    return;
  }
  //if there are no intersections, add the 
  //contribution to the area calculation
  //no need to put seg in segs because this is the last 
  //segment to be processed
  area += seg.start_.x() * seg.end_.y() - seg.end_.x() * seg.start_.y();
  // check the sign of the area. If it is negative (ie the polygon is 
  // described clockwise) then reverse the order of the vertices
  if (area < 0){
    reverse(vertices_.begin(), vertices_.end());
  }
}


/*!\brief Is a Pt a vertex of the Poly?
  \return true if and only if point is a vertex of the Poly. 
*/
bool Poly::hasVertex(const Pt& p) const {
  return find(vertices_.begin(), vertices_.end(), p) < vertices_.end();
}
  
/*!\brief The segments of the Poly that adjoin a given Pt.
  \param p
  \return If p is on boundary of the Poly then the method returns segments of the poly
  that contain p (two if p is a vertex, one if not). If p is not 
  on the boundary of the polygon then it returns an empty vector. 
  Orientation and order of segments is positive (ie counter-clockwise)
*/
std::vector<Seg> Poly::adjoiningSegments(const Pt& p) const {
  //if p is on boundary of polygon then returns segments of the poly
  //that contain p (two if p is a vertex, one if not). If p is not 
  //on the boundary of the polygon then returns empty vector. 
  //Orientation and order of segments is positive (ie anti-clockwise)
  std::vector<Seg> segs;
  for(auto i = vertices_.begin(); i != vertices_.end(); ++i){
    Seg nextSeg(*i,i < (vertices_.end() - 1) ? *(i+1) : vertices_.front());
    Seg previousSeg(i > vertices_.begin() ? *(i-1) : vertices_.back(), *i);
    if (p == *i) {
      segs.push_back(previousSeg);
      segs.push_back(nextSeg);
      return segs;
    }
    if (nextSeg.contains(p) && p != nextSeg.end()){
      segs.push_back(nextSeg);
      return segs;
    }
  }
  return segs;
}


/*!\brief Equality of two polygons. 
  \param pol The other Poly that is to be compared with *this
  \return true if and only if their vertices (after removal of redundancies) are equal as sets.
*/
bool Poly::operator==(const Poly& pol) const{
  
  // uses the fact that no members
  // of Poly will have redundant vertices. 
  // Function might be expensive, but it will probably
  // only be used in testing the Poly class.

  if(this == &pol)
    return true;

  auto verts = pol.vertices();

  if(verts.size() != vertices_.size())
    return false;

  if(verts.empty())
    // then vertices is also empty
    return true;

  // look for the first vertice of (*this) in verts
  auto i = find(verts.begin(), verts.end(), vertices_.front());
  
  if (i == verts.end())
    //no such vertex in verts
    return false;
  else {
    // rotate verts and compare the vectors element by element
    rotate(verts.begin(), i, verts.end());
    return verts == vertices_;
  }
}

/*!\brief Inequality of operators
  \param pol The other Poly that is to be compared with *this
  \return true if and only if their vertices (after removal of redundancies) are not equal as sets.
*/
bool Poly::operator!=(const Poly& pol) const {
  return !(*this == pol);
}

/*!\brief Translation
  \param v
  \return A reference to a Poly obtained from *this by translation
  by the vector v
*/
Poly& Poly::operator+=(const Pt& v){
  for(auto i = vertices_.begin(); i != vertices_.end(); ++i)
    *i += v;
  return *this;
}


/*!\brief Translation
  \param v
  \return The Poly obtained by translating *this by the vector v
*/
Poly Poly::operator+(const Pt& v){
  return Poly(*this += v);

}

/*!\brief Width of the Poly
  \return The greatest x-value minus the least x-value.
*/
double Poly::width() const {
  auto cmp = [](Pt p1, Pt p2) {return p1.x() < p2.x();};
  return (*max_element(vertices_.begin(), vertices_.end(), cmp)).x() - 
         (*min_element(vertices_.begin(), vertices_.end(), cmp)).x(); 
}

/*!\brief The top of the Poly
  \return The greatest y value.
*/
double Poly::top() const {
  auto cmp = [](Pt p1, Pt p2) {return p1.y() < p2.y();};
  return (*max_element(vertices_.begin(), vertices_.end(), cmp)).y();
}

/*!\brief The bottom of the Poly
  \return The least y value
*/
double Poly::bottom() const {
  auto cmp = [](Pt p1, Pt p2) {return p1.y() < p2.y();};
  return (*min_element(vertices_.begin(), vertices_.end(), cmp)).y();
}

/*!\brief The left-most point of the Poly
  \return The least x value
*/
double Poly::left() const {
  auto cmp = [](Pt p1, Pt p2) {return p1.x() < p2.x();};
  return (*min_element(vertices_.begin(), vertices_.end(), cmp)).x();
}

/*!\brief The right-most point of the Poly
  \return The greatest x value
*/
double Poly::right() const {
  auto cmp = [](Pt p1, Pt p2) {return p1.x() < p2.x();};
  return (*max_element(vertices_.begin(), vertices_.end(), cmp)).x();
}

/*!\brief The highest point of the Poly along the central vertical axis
  \return The greatest y value with x == 0.5*(left() + right())
*/
double Poly::centreTop() const {
// greatest y value with x == 0.5*(left() + right())
  double centre = 0.5*(left() + right());
  Ray r(Pt(centre, bottom() - 1), Pt(0,1));
  std::vector<Pt> pts = r.boundaryIntersections(*this);
  auto cmp = [](Pt p1, Pt p2) {return p1.y() < p2.y();};
  return (*max_element(pts.begin(), pts.end(), cmp)).y();
}

/*!\brief The lowest point of the Poly along the central vertical axis
  \return The least y value with x == 0.5*(left() + right())
*/
double Poly::centreBottom() const {
// least y value with x == 0.5*(left() + right())
  double centre = 0.5*(left() + right());
  Ray r(Pt(centre, bottom() - 1), Pt(0,1));
  std::vector<Pt> pts = r.boundaryIntersections(*this);
  auto cmp = [](Pt p1, Pt p2) {return p2.y() < p1.y();};
  return (*max_element(pts.begin(), pts.end(), cmp)).y();
}

/*!\brief the highest point at the right-most edge of the Poly
  \return The greatest y value with x == right()
*/
double Poly::rightTop() const {
  auto cmp = [](Pt p1, Pt p2) {return p1.x() < p2.x() || 
			       (ffm_numerics::almostEq(p1.x(), p2.x()) && p1.y() < p2.y());};
  return (*max_element(vertices_.begin(), vertices_.end(), cmp)).y();
}
  
/*!\brief the lowest point at the right-most edge of the Poly
  \return The least y value with x == right()
*/
double Poly::rightBottom() const {
  auto cmp = [](Pt p1, Pt p2) {return p1.x() < p2.x() || 
			       (ffm_numerics::almostEq(p1.x(), p2.x()) && p1.y() > p2.y());};
  return (*max_element(vertices_.begin(), vertices_.end(), cmp)).y();
}
  
/*!\brief The centroid
  \return The arithmetic average of the vertices
*/
Pt Poly::centroid() const {
  if (vertices_.empty()) return Pt();
  Pt p(0,0);
  int n = 0;
  for (const Pt& v : vertices_) {
    p += v;
    ++n;
  }
  return p * (1.0/n);
}

/*!\brief The point in the base of the Poly with given x-value
  \param x
  \return The lowest (ie least y value) point in the Poly that has x-coordinate equal to x.

  If x is greater than right() then the function returns the lowest y-value with x == right(). Similarly if 
  x is less than left() then the function returns the least y-value with x == left().
*/
Pt Poly::pointInBase(const double& x) const {
  //returns left-most or right-most base points if x is 
  //not within the x-bounds of the polygon
  double xx = std::max(std::min(x, right()), left());
  Ray r(Pt(xx, bottom()-1), Pt(0,1));
 std::vector<Pt> intsct(r.boundaryIntersections(*this));
  return *min_element(intsct.begin(),intsct.end(),[](Pt p1, Pt p2){return p1.y() < p2.y();});
}


/*!\brief Volume of revolution (see detailed description). 
  \return Volume of revolution around the vertical axis which 
  passes through the centroid. 

  Works correctly only for polygons
  that are symmetric about this axis
*/
double Poly::volumeOfRev() const {

  Pt u,first;
  double sum(0), r1, r2;
  double axis = centroid().x();

  auto i = vertices_.begin();
  u = first = *(i++);
  while(i != vertices_.end()){
    if(ffm_numerics::geq(u.x(), axis) && ffm_numerics::geq((*i).x(), axis)){
      r1 = u.x() - axis;
      r2 = (*i).x() - axis;
      sum += (pow(r1,2) + r1*r2 + pow(r2,2))*((*i).y() - u.y());
    }
    u = *(i++);
  }

  //the last segment
  if(ffm_numerics::geq(u.x(), axis) && ffm_numerics::geq(first.x(), axis)){
    r1 = u.x() - axis;
    r2 = first.x() - axis;
    sum += (pow(r1,2) + r1*r2 + pow(r2,2))*(first.y() - u.y());
  }
  return sum*PI/3;
}

//printing

/*!\brief Printing
  \return A formatted string describing the Poly
*/
std::string Poly::printToString() const {
  std::string str;
  if(!vertices_.empty()){
    for(const Pt& v : vertices_){
      str = str + (v.printToString());
      str += " ";
    }
  }
  return str;
}


