#ifndef TEST_H
#define TEST_H
#include <limits.h>
#include <iostream>
#include "pt.h"
#include "species.h"
#include "stratum.h"
#include "forest.h"
#include "location.h"
#include "ffm_io.h"
#include "layer.h"
#include "ffm_settings.h"

using namespace ffm_settings;
using std::vector;
using std::cout;
using std::endl;

int main() {
  std::string inFileName = "ffm_input.txt";

  std::pair<Results::OutputLevelType, int> waddaYaWant = prelimParseInputTextFile(inFileName);
  Results::OutputLevelType outputLevel = waddaYaWant.first;
  int numIter = waddaYaWant.second;
  
  bool monteCarlo = (outputLevel == Results::MONTE_CARLO);
  

  if (!monteCarlo) {
    Location loc = parseInputTextFile(inFileName, monteCarlo);
    cout << loc.printToString() << endl;
    Results res = loc.results();
    cout << res.printToString(outputLevel) << endl;
  }
  else {
    int counter = 0;
    bool firstTime = true;
    while (counter < numIter) {
      Location loc = parseInputTextFile(inFileName, monteCarlo);
      if (loc.empty()) {
	cout << "continuing" << endl;
	continue;
      }
      Results res = loc.results();
      if (firstTime) {
	cout << printMonteCarloHeader(loc);
	firstTime = false;
      }
      cout << printMonteCarloInputs(loc);
      cout << printMonteCarloResults(res) << endl;
      ++counter;
    }
  }


}

#endif //TEST_H
