#ifndef LAYER_H
#define LAYER_H

/*!\brief A Layer object represents horizontal bands of uniform forest composition

  Forest strata may overlap, and a Layer object may contain 0, 1 or more than 1 Stratum. 
  The Layer class is used primarily for the wind field computations.
*/

class Layer{

public:

  //constructors
  Layer();
  Layer(const double& bottom, const double& top, const std::vector<Stratum::LevelType>& levels);

  //other methods
  double bottom() const;
  double top() const;
  std::vector<Stratum::LevelType> levels() const;

private:
  double bottom_;
  double top_;
  std::vector<Stratum::LevelType> levels_;
};

#include "layer_inline.h"

#endif //LAYER_H
