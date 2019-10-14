#ifndef FileNameParser_h
#define FileNameParser_h 

#include <string>
#include <cstdlib> 
#include <iostream>

using namespace std;

class FileNameParser {
public:
  
  FileNameParser();
  // extract from file path or TTree name
  FileNameParser(string); 
  // extract from full path of raw file
  FileNameParser(string,int); 
  ~FileNameParser();
  
  void   Init(int userOption = -1);
  int    parseInt(string f, string s1, int);
  int    parseInt(string f, string s1, string s2);
  char   parseChar(string f, string s1);
  char   parseChar(string f, string s1, string s2);

  string GetFileName(string filePath);
  string GetFileID();
  string GetFileID(string);

  string GetTreeName(string filePath);
  string GetTreeName();
  string Get_hQ_Fixed_Name(string filePath);
  string Get_hQ_Fixed_Name();
  int    pmtID(string filename);
  int    run(string filename);
  int    location(string filename);
  char   test(string filename);
  int    hVStep(string filename);

  void   Print_Data();
  
  int    GetPMT();
  int    GetRun();
  int    GetLoc();
  char   GetTest();
  int    GetHVStep(); 
  
 private:

  void   SetFileID(string fileName);
  void   SetFileID();
  
  int  PMT;   
  int  Run;   
  int  Loc;   
  char Test;  
  int  HVStep;
  string FileID;

  bool allSet;

  // < 0  extract from FileID 
  // > -1 extract from path to binary
  int option;

};

#endif

#ifdef FileNameParser_cxx

FileNameParser::FileNameParser(){
  Init();
}


// Option for use with TTree name
FileNameParser::FileNameParser(string treeName){
  
  Init();

  string fileName = GetFileName(treeName);
  
  // change to same format as root filename
  fileName = fileName + ".root";
  
  PMT    = pmtID(fileName);
  Run    = run(fileName);
  Loc    = location(fileName);
  Test   = test(fileName);
  HVStep = hVStep(fileName); // 0 if Test!='G'
  
  allSet = true;

  //SetFileID(fileName);
  SetFileID();

  Print_Data();
  
}

// 
FileNameParser::FileNameParser(string rawFilePath,
			       int userOption){
  Init(userOption);

  PMT  = pmtID(rawFilePath);
  Run  = run(rawFilePath);
  Loc  = location(rawFilePath);
  Test = test(rawFilePath);
  
  allSet = true;

  SetFileID();

  Print_Data();

}

FileNameParser::~FileNameParser(){

}

void FileNameParser::Init(int userOption){

  printf("\n  FileNameParser \n") ;
  
  allSet = false;

  PMT    = 0;   
  Run    = 0;   
  Loc    = -1;   
  Test   = 'A';  
  HVStep = -1;
  
  option = userOption;
  
}
    
#endif
