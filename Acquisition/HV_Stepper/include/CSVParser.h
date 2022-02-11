#ifndef _CSVPARSER_H_
#define _CSVPARSER_H_

#include "PMT.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

class CSVParser {

public:

  CSVParser(std::vector<PMT> &ext_pmts) : raw_pmts(ext_pmts) {}
  bool readFile(std::string filename);

private:

  std::vector<PMT> &raw_pmts;

};
#endif