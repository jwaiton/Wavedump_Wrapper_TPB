#define FileNameParser_cxx
#include "FileNameParser.h"

int FileNameParser::parseInt(std::string f, std::string s1, std::string s2){
 
  int pos1 = f.find(s1,0);
  int pos2 = f.find(s2,pos1);

  // "Test" not found for case "HV"
  if     (pos1 == -1){
    s1 = "HV";
    pos1 = f.find(s1,0);
  }
  else if(pos2 == -1){
    s2 = "HV";
    pos2 = f.find(s2,pos1);
  }

  std::string str = f.substr(pos1,pos2 - pos1);
  
  int first = str.find("_");
  str = str.substr(first+1);
  str = str.substr(0, str.size()-1);
  
  return std::stoi(str);
}

char FileNameParser::parseChar(std::string f, std::string s1, std::string s2){
 
  int pos1 = f.find(s1,0);
  int pos2 = f.find(s2,pos1);

  // "Test" not found for case "HV"
  if     (pos1 == -1){
    s1 = "HV";
    pos1 = f.find(s1,0);
  }
  else if(pos2 == -1){
    s2 = "HV";
    pos2 = f.find(s2,pos1);
  }

  std::string str = f.substr(pos1,pos2 - pos1);
  
  int first = str.find("_");
  str = str.substr(first+1);
  str = str.substr(0, str.size()-1);
  
  return str[0];
}

std::string FileNameParser::GetFileName(std::string filePath){

  int pos1 = filePath.find("Run_",0);
  std::string fileName = filePath.substr(pos1,filePath.size() - pos1);  
  return fileName;
}

std::string FileNameParser::GetFileID(std::string fileName){
  return fileName.substr(0,fileName.size() - 5);
}

std::string FileNameParser::GetTreeName(std::string filePath){
  
  std::string treeName = "Events_" + GetFileID(GetFileName(filePath));
  
  return treeName; 
}

int FileNameParser::pmtID(std::string filename){
  return parseInt(filename,"PMT_","Loc");
}

int FileNameParser::run(std::string filename){
  return parseInt(filename,"Run_","PMT");
}

int FileNameParser::location(std::string filename){
  return parseInt(filename,"Loc_","Test");
}

char FileNameParser::test(std::string filename){ 
  
  char ctr = parseChar(filename,"Test_","root");

  if( ctr > '0' && ctr < '6' ) 
    return 'G';
  else
    return ctr;
} 

int FileNameParser::hVStep(std::string filename){ 

  if(test(filename)=='G')
    return parseInt(filename,"Test_","root");
  else 
    return 0;
} 
