#include <limits.h>
#include <iostream>
#include <fstream>
#include "pt.h"
#include "species.h"
#include "stratum.h"
#include "forest.h"
#include "location.h"
#include "ffm_io.h"
#include "layer.h"
#include "ffm_settings.h"
#include "ffm_util.h"

using namespace ffm_settings;
using std::vector;
using std::cout;
using std::endl;

void process(std::string inPath, std::ostream &outputStream, bool paramsFlag) {

  std::pair<Results::OutputLevelType, int> waddaYaWant = prelimParseInputTextFile(inPath);
  Results::OutputLevelType outputLevel = waddaYaWant.first;
  int numIter = waddaYaWant.second;
  
  bool monteCarlo = (outputLevel == Results::MONTE_CARLO);
  
  if (!monteCarlo) {
    Location loc = parseInputTextFile(inPath, monteCarlo);

    if (paramsFlag) 
      outputStream << loc.printToString() << endl;
    
    Results res = loc.results();

    outputStream << res.printToString(outputLevel) << endl;
  }
  else {
    int counter = 0;
    bool firstTime = true;
    while (counter < numIter) {
      Location loc = parseInputTextFile(inPath, monteCarlo);
      if (loc.empty()) {
	cout << "continuing" << endl;
	continue;
      }
      Results res = loc.results();
      if (firstTime) {
	outputStream << printMonteCarloHeader(loc);
	firstTime = false;
      }
      outputStream << printMonteCarloInputs(loc);
      outputStream << printMonteCarloResults(res) << endl;
      ++counter;
    }
  }


}

int main(int argc, char *argv[]) {
  std::string usage("usage: ffm input_file [output_file] [-p]"); 

  if (argc < 2) {
    cout << usage << endl;
    return 0;
  }

  // collect the inPath and the optional outPath and flag arguments
  bool paramsFlag;
  std::string flag_str("-p");
  std::string inPath;
  std::string outPath;

  for (int i = 1; i < argc; i++) {
    std::string arg( argv[i] );

    if (arg.compare(0, flag_str.size(), flag_str) == 0)
      paramsFlag = true;
    else if (inPath.empty())
      inPath = arg;
    else if (outPath.empty())
      outPath = arg;
  }

  if (inPath.empty()) {
    cout << usage << endl;
    return 0;
  }

  std::ostream* fp = &cout;
  std::ofstream fout;
  if( !outPath.empty()) {
    fout.open(outPath); 
    fp = &fout;
  }

  process(inPath, *fp, paramsFlag);
  if (!outPath.empty()) fout.close();

  return 0;
}

