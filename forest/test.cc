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

void process(std::string inPath, std::ostream &outputStream) {

  std::pair<Results::OutputLevelType, int> waddaYaWant = prelimParseInputTextFile(inPath);
  Results::OutputLevelType outputLevel = waddaYaWant.first;
  int numIter = waddaYaWant.second;
  
  bool monteCarlo = (outputLevel == Results::MONTE_CARLO);
  
  if (!monteCarlo) {
    Location loc = parseInputTextFile(inPath, monteCarlo);
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
  if (argc < 2) {
    cout << "usage: ffm input_file [output_file]" << endl;
    return 0;
  }

  std::string inPath = std::string(argv[1]);

  // optional output path - if not provided output is sent to console
  bool isFileOutput = false;
  std::ostream* fp = &cout;
  std::ofstream fout;
  std::string outPath = argc >= 3 ? std::string(argv[2]) : "";
  if( !outPath.empty()) {
    fout.open(outPath); 
    fp = &fout;
    isFileOutput = true;
  }

  process(inPath, *fp);
  if (!outPath.empty()) fout.close();
}

