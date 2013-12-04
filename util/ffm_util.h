#ifndef FFM_UTIL_H
#define FFM_UTIL_H

#include <string>
#include <vector>

/*!\brief Various useful utilities*/
namespace ffm_util {

  //string utility functions ********************

  std::string trim(const std::string& str,
		   const std::string& whitespace = " \t");
  // trims leading and trailing whitespace

  std::string reduce(const std::string& str);
  //removes all whitespace

  std::vector<std::string> split(const std::string& str, const char& ch);
  // splits string at all occurences of ch
  // removes leading and trailing whitespace
  // returns vector of strings

  //statistics utility functions ******************

  int nonNullCount(const std::vector<double>& data);
  double mean(const std::vector<double>& data, const bool& ignoreZeros = true);
  double stdDev(const std::vector<double>& data, const bool& ignoreZeros = true);
  double maxVal(const std::vector<double>& data);
  double cappedMax(const std::vector<double>& data, const bool& ignoreZeros = true);

  //random number generation
  double randomNormal(const double& mean, const double& stdDev);

  //expects str to be comma separated pair representing mean and stdDev
  //if only one value then assumes stdDev is zero
  double randomNormal(const std::string& str);


  double randomUniform(const double& mean, const double& range);

  //expects str to be comma separated pair representing mean and range
  //if only one value then assumes range is zero
  double randomUniform(const std::string& str);
}

#endif //FFM_UTIL_H
