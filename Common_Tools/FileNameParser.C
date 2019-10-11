#define FileNameParser_cxx
#include "FileNameParser.h"

int FileNameParser::parseInt(string f, string s1,
			     int length){
 
  int pos = f.find(s1,0) + 4; 
    
  string str = f.substr(pos,length);

  return stoi(str);
}

int FileNameParser::parseInt(string f, string s1, string s2){
 
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

  string str = f.substr(pos1,pos2 - pos1);
  
  int first = str.find("_");
  int last  = str.size()-1;

  // start after "_" 
  first++;

  str = str.substr(first);
  // end one before s2 starts
  str = str.substr(0, last);
  
  return stoi(str);
}

char FileNameParser::parseChar(string f, string s1){
 
  int pos = f.find(s1,0) + 9;
  
  string str = f.substr(pos,1);

  return str[0];
}

char FileNameParser::parseChar(string f, string s1, string s2){
 
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

  string str = f.substr(pos1,pos2 - pos1);
  
  int first = str.find("_");
  str = str.substr(first+1);
  str = str.substr(0, str.size()-1);
  
  return str[0];
}

string FileNameParser::GetFileName(string filePath){

  int pos1 = filePath.find("Run_",0);
  string fileName = filePath.substr(pos1,filePath.size() - pos1);  
  return fileName;
}

string FileNameParser::GetFileID(string fileName){
  return fileName.substr(0,fileName.size() - 5);
}

string FileNameParser::GetTreeName(string filePath){
  
  string treeName = "Events_" + GetFileID(GetFileName(filePath));
  
  return treeName; 
}


string FileNameParser::Get_hQ_Fixed_Name(string filePath){
  
  string hName = "hQ_Fixed_" + GetFileID(GetFileName(filePath));
  
  return hName; 
}

int FileNameParser::pmtID(string filename){

  if(option < 0)
    return parseInt(filename,"PMT_","Loc");
  else
    return parseInt(filename,"/PMT",4);
}

int FileNameParser::run(string filename){
 
  if(option < 0)
    return parseInt(filename,"Run_","PMT");
  else
    return parseInt(filename,"/RUN",6);
}

int FileNameParser::location(string filename){
  
  if(option < 0)
    return parseInt(filename,"Loc_","Test");
  else
    return parseInt(filename,"wave_","dat");
}

char FileNameParser::test(string filename){ 
  
  char ctr; 
  
  if(option < 0)
    ctr = parseChar(filename,"Test_","root");
  else 
    ctr = parseChar(filename,"/PMT");
  
  if( ctr > '0' && ctr < '6' ) 
    return 'G';
  else
    return ctr;
} 

int FileNameParser::hVStep(string filename){ 

  if(test(filename)=='G')
    return parseInt(filename,"Test_","root");
  else 
    return 0;
} 

int    FileNameParser::GetPMT(){
  if(allSet)
    return PMT;
  else
    cerr << "Error: data members not set " << endl;
  return -1;
}

int    FileNameParser::GetRun(){
  if(allSet)
    return Run;
  else
    cerr << "Error: data members not set " << endl;
  return -1;
}

int    FileNameParser::GetLoc(){
  if(allSet)
    return Loc;
  else
    cerr << "Error: data members not set " << endl;
  return -1;
}

char   FileNameParser::GetTest(){
  if(allSet)
    return Test;
  else
    cerr << "Error: data members not set " << endl;
  return -1;
}

int    FileNameParser::GetHVStep(){
  if(allSet)
    return HVStep;
  else
    cerr << "Error: data members not set " << endl;
  return -1;
}

