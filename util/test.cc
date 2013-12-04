#include <iostream>
#include "ffm_util.h"

int main() {
  std::vector<double> vec{1, 4, 0, 2, 0};
  double result = stdDev(vec,true);
  std::cout << result << std::endl;
}
