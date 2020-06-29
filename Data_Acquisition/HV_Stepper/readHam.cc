#include <string>
#include <iostream>
#include <vector>

#include "PMT.h"
#include "CSVParser.h"
#include "VoltageSteps.h"
#include "cxxopts.hpp"

int main (int argc, char* argv[]) {

  std::vector<PMT> data;

  // set up command line parser
  cxxopts::Options options("readHam", "Reader of Hammatsu files");
  options.add_options()
    ("i,input", "Input File name", cxxopts::value<std::string>()->default_value("PMTdata.csv"))
    ("o,output","Output File name", cxxopts::value<std::string>()->default_value("HVScan-new.txt"))
    ("r,recreate","recreate", cxxopts::value<bool>()->default_value("false"))
    ("g,flatGain","flat gain", cxxopts::value<bool>()->default_value("false"))
    ;
  auto result = options.parse(argc, argv);

  /*
  if (argc == 1) {
    ShowOpts();
    return -1;
  }*/
  // get the options we parse
  std::string inputfile =   result["i"].as<std::string>();

  std::string outputfile =  result["o"].as<std::string>();

  bool flatGain = result["g"].as<bool>();
  bool recreate=  result["r"].as<bool>();

  CSVParser parser(data);
  parser.readFile(inputfile);

  VoltageSteps stepper(data);
  //TODO: add checks in createHVScanFile 
  stepper.createHVScanFile(outputfile, flatGain, recreate);
  return 0;
}