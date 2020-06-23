#include "CSVParser.h"

bool CSVParser::readFile(std::string filename) {
  std::ifstream fin;

  fin.open(filename.c_str());

  if (fin.fail()) {
    std::cout << "Could Not find file: " << filename << std::endl;
    return false;
  }
  std::string line;

  while(getline(fin, line)) {
    std::vector<std::string> row;
    std::stringstream ss(line);
    std::string col;
    while(getline(ss, col, ',')) {
        row.emplace_back(col);
      }//inner loop
    PMT pmt(row);
    raw_pmts.emplace_back(pmt);
  }//outerloop
  return true;
}