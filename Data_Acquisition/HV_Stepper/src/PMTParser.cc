#include "PMTParser.h"

// helper to check line is PMT data
bool dataLine(std::string& line){
  /*
  * Checks first character in line is formatted correctly.
  */
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
    std::cout << "Failed to find file: " << file <<std::endl;
    return;
  }
  
  // read it;
  std::string line;
  while(getline(in,line, '\n')){
    if (dataLine(line)){
      //reads only the last line, for some reason
      std::cout << line << std::endl;
      lines.push_back(line);
    } 
  }
}

// get the PMT data
typedef std::vector<PMTData> PMTs;
void fillPMTData(std::string file, PMTs& vec){

  // read the file and get data in lines one per PMT
  std::vector<std::string> lines;
  readdata(file, lines);

  // make PMT objects
  for (auto str : lines){
    //fill pmts vector with PMT data
    CSVParser parser(str); //parser initialised with string
    PMTData pmt; 
    pmt.stream(parser);
    vec.push_back(pmt);
  } // lines
}
