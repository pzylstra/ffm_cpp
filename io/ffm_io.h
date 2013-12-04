#ifndef FFM_IO_H
#define FFM_IO_H

#include "location.h"

std::vector<std::string> processLine(const std::string& line);

double stringToDouble(const std::string& str);

std::pair<Results::OutputLevelType, int> prelimParseInputTextFile(std::string inFileName);

Location parseInputTextFile(const std::string& inFileName, const bool& monteCarlo);

std::string printMonteCarloHeader(const Location& loc);

std::string printMonteCarloInputs(const Location& loc);

std::string printMonteCarloResults(const Results& res);

#endif //FFM_IO_H
