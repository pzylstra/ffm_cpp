#include <limits.h>
#include <iostream>
#include <fstream>
#include <set>
#include <utility>
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
using std::set;
using std::cout;
using std::endl;

void process(std::string inPath, std::ostream &outputStream, bool paramsFlag) {

  std::pair<Results::OutputLevelType, int> prelimPars = prelimParseInputTextFile(inPath);

  Results::OutputLevelType outputLevel = std::get<0>(prelimPars);
  int numIter = std::get<1>(prelimPars);
  
  bool monteCarlo = (outputLevel == Results::MONTE_CARLO);
  
  if (!monteCarlo) {
    Location loc = parseInputTextFile(inPath, false);

    if (paramsFlag) 
      outputStream << loc.printToString() << endl;
    
    Results res = loc.results();

    outputStream << res.printToString(outputLevel) << endl;
  }
  else {
    for (int i = 0; i < numIter; ) {
      Location loc = parseInputTextFile(inPath, true);

      if (!loc.empty()) {
        if (i == 0) outputStream << printMonteCarloHeader(loc);

        Results res = loc.results();

        outputStream << printMonteCarloInputs(loc);
        outputStream << printMonteCarloResults(res) << endl;

        i++ ;
      }
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

