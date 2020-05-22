#ifndef _PMTPARSE_H
#define _PMTPARSE_H

#include "csvparser.h"
#include "PMTData.h"
#include <vector>
#include <string>
#include<iostream>
#include<fstream>
#include<sstream>
#include<ostream>

/*** Helper functions to parse the PMT data file and create PMTData objects ***/


// helper to check line is PMT data
bool dataLine(std::string& line){

  bool ok = true;
  std::string val = std::string(1,line.at(0));
  if (val == "(" || val == "," || val == "S" ||  val ==  "\""){
    ok = false;
  }
  return ok;
}

// read file into lines
void readdata(std::string file, std::vector<std::string>& lines){

  // open the file
  std::ifstream in;
  in.open(file.c_str());
  
  if(in.fail()){
    std::cout << "Failed to find file" <<std::endl;
    return;
  }
  
  // read it;
  std::string line;
  while(getline(in,line)){
    if (dataLine(line)){
      lines.push_back(line);
    } 
  }
}

// get the PMT data
typedef std::vector<PMTData> PMTs;
void fillPMTData(std::string file, PMTs& vec){

  // read the file and get data in lines one per PMT
  std::vector<std::string> lines;
  readdata("PMTdata.csv", lines);

  // make PMT objects
  for (auto str : lines){
    CSVParser parser(str);
    PMTData pmt; pmt.stream(parser);
    vec.push_back(pmt);
  } // lines
}

#endif
