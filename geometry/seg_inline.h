#ifndef SEG_INLINE_H
#define SEG_INLINE_H

#include "pt.h"

/*!\brief Default constructor*/
inline Seg::Seg() : start_(Pt()), end_(Pt()){}

/*!\brief Constructor
  \param s (Pt)
  \param e (Pt)

  Constructs a Seg object starting at s and ending at e
*/
inline Seg::Seg(const Pt& s, const Pt& e) : start_(s), end_(e) {}

/*!\brief Constructor
  \param s (Pt)
  \param a (double, radians)
  \param l (double, m)

  Constructs a Seg of length l starting at s, with angle a relative to positive x-axis
*/
inline Seg::Seg(const Pt& s, const double& a, const double& l) : start_(s), end_(s + Pt(l*cos(a),l*sin(a))) {}

/*!\brief Initial point of the Seg
  \return The initial Pt of the Seg
*/
inline Pt Seg::start() const {return start_;}

/*!\brief Terminal point of the Seg
  \return The terminal Pt of the Seg
*/
inline Pt Seg::end() const {return end_;}

/*!\brief Length
  \return The length of the Seg (m)
*/
inline double Seg::length() const {
  return (end_ - start_).norm();
}

/*!\brief Make a vector (a Pt) from the Seg
  \return The vector end() - start()
*/
inline Pt Seg::vect() const {
  return end_ - start_;
}

/*!\brief Printing
  \return A formatted string describing the Seg
*/
inline std::string Seg::printToString() const {
  return start_.printToString() + " -> " + end_.printToString();
}


#endif //SEG_INLINE_H
