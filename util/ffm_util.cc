#include <algorithm>
#include <string>
#include <vector>
#include <chrono>
#include <random>

#include "ffm_util.h"
#include "ffm_numerics.h"

namespace ffm_util {
  
  std::random_device RDEV{};
  std::default_random_engine GENERATOR{ RDEV() };

  /*!\brief Trims leading and trailing white space
    \param str
    \param whitespace = " \t"
    \return A string derived from str with all leading and trailing 
    white space removed

    Note that the second parameter is optional and defaults to " \t"
  */
  std::string trim(const std::string& str,
		   const std::string& whitespace) {
    // trims leading and trailing whitespace
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos) return ""; // no content
    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;
    return str.substr(strBegin, strRange);
  }

  /*!\brief Removes all white space
    \param str
    \return A string derived from str by removing all white space
  */
  std::string reduce(const std::string& str) {
    //removes all whitespace
    std::string ret(str);
    ret.erase(remove_if(ret.begin(), ret.end(), isspace), ret.end());
    return ret;
  }

  /*!\brief Splits a string using a char as the separator and removes all
    leading and trailing white space from the individual elements
    \param str
    \param ch
    \return A vector of type string derived from str by splitting str at 
    all occurrences of ch. Leading and trailing white space is removed from 
    the elements of the vector returned
  */
  std::vector<std::string> split(const std::string& str, const char& ch)  {
    // splits string at all occurences of ch
    // removes leading and trailing whitespace
    // returns vector of strings
    std::vector<std::string> ret;
    size_t startPos = 0, pos;
    while (startPos < str.size()){
      pos = str.find(ch, startPos);
      if (pos > startPos) ret.push_back(trim(str.substr(startPos, pos - startPos)));
      startPos = pos < std::string::npos ? pos + 1 : std::string::npos;
    }
    return ret;
  }

  /*!\brief Number of non-zero elements
    \param data
    \return A count of the number of non-zero elements in data
  */
  int nonNullCount(const std::vector<double>& data) {
    if (data.empty()) return 0;
    return count_if(data.begin(), data.end(), 
		    [](const double& x){return !ffm_numerics::almostZero(x);});
  }

  /*!\brief Mean of a vector of doubles
    \param data
    \param ignoreZeros = true
    \return The mean of the elements of data. If ignoreZeros == true
    then the zero elements of data are ignored in the calculation

    Note that the second parameter is optional and defaults to true
   */
  double mean(const std::vector<double>& data, const bool& ignoreZeros) {
    if (data.empty()) return 0;
    return accumulate(data.begin(), data.end(), 0.0) / 
      (ignoreZeros ? nonNullCount(data) : data.size()); 
  }

  /*!\brief Standard deviation of a vector of doubles
    \param data
    \param ignoreZeros = true
    \return The standard deviation of the elements of data. If ignoreZeros == true
    then the zero elements of data are ignored in the calculation

    Note that the second parameter is optional and defaults to true
   */
  double stdDev(const std::vector<double>& data, const bool& ignoreZeros) {
    int num = ignoreZeros ? nonNullCount(data) : data.size();
    if (num < 2) return 0.0;
    double m = mean(data,ignoreZeros);
    double sum = accumulate(data.begin(), data.end(), 0.0, 
			    [ignoreZeros, m](const double& sum, const double& elem){
			      return sum + (ffm_numerics::almostZero(elem) && ignoreZeros ? 
					    0 : (elem - m)*(elem - m));});
    return pow(sum / (num - 1), 0.5);
  }

  /*!\brief The maximum value
    \param data
    \return The maximum value of the elements of data
  */
  double maxVal(const std::vector<double>& data) {
    auto i = max_element(data.begin(), data.end());
    return *i;
  }

  /*!\brief Maximum value capped by mean + 1 standard deviation
    \param data
    \param ignoreZeros = true
    \return The minimum of the maximum value of the data and the mean + 1 standard deviation.
    If ignoreZeros == true then the zero elements of data are ignored in the computation.
 
    Note that the second parameter is optional and defaults to true
 */
  double cappedMax(const std::vector<double>& data, const bool& ignoreZeros) {
    return std::min(maxVal(data), mean(data,ignoreZeros) + stdDev(data,ignoreZeros));
  }

  /*!\brief Random number from specified normal distribution
    \param mean
    \param stdDev
    \return A random value from a Gaussian distribution with given 
    mean and standard deviation
  */
  double randomNormal(const double& mean, const double& stdDev) {
    if (stdDev <= 0) return mean;
    std::normal_distribution<double> distribution(mean,stdDev);
    return distribution(GENERATOR);
  }

  /*!\brief Random number from specified uniform distribution
    \param mean
    \param range
    \return A random value from the uniform distribution on [mean - 0.5*range, mean + 0.5*range]
  */
  double randomUniform(const double& mean, const double& range) {
    if (range <= 0) return mean;
    std::uniform_real_distribution<double> distribution(mean - 0.5*range, mean + 0.5*range);
    return distribution(GENERATOR);
  }

  /*
   * Random number from U[0, 1].
   */
  double randomUniform() {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(GENERATOR);
  }

  /*!\brief Random number from specified normal distribution
    \param str A comma separated pair representing mean and standard deviation
    of the desired distribution. If only one value is present then that is assumed
    to be the mean and the standard deviation is taken to be zero.
    \return A random value from the specified Gaussian distribution
  */
  double randomNormal(const std::string& str) {
    //expects str to be comma separated pair representing mean and stdDev
    //if only one value then assumes stdDev is zero
    std::vector<std::string> strVec = ffm_util::split(str, ',');
    if (strVec.size() == 1) return atof(strVec.at(0).c_str());
    return ffm_util::randomNormal(atof(strVec.at(0).c_str()), atof(strVec.at(1).c_str()));
  }

  /*!\brief Random number from specified uniform distribution
    \param str A comma separated pair representing mean and range
    of the desired distribution. If only one value is present then that is assumed
    to be the mean and the range is taken to be zero.
    \return A random value from the specified uniform distribution
  */
  double randomUniform(const std::string& str) {
    //expects str to be comma separated pair representing mean and range
    //if only one value then assumes range is zero
    std::vector<std::string> strVec = ffm_util::split(str, ',');
    if (strVec.size() == 1) return atof(strVec.at(0).c_str());
    return ffm_util::randomUniform(atof(strVec.at(0).c_str()), atof(strVec.at(1).c_str()));
  }

}


