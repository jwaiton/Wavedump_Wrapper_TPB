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
  string GetDir();
  string GetFileID();
  string GetFileID(string);

  string GetTreeName(string filePath, int option);
  string GetTreeName(string filePath);
  string GetTreeName();
  string Get_hQ_Fixed_Name(string filePath);
  string Get_hQ_Fixed_Name();

  int    HasExtension(string name);

  int    pmtID(string name);
  int    run(string name);
  int    location(string name);
  char   test(string name);
  int    hVStep(string name);

  void   Print_Data();
  
  int    GetPMT();
  int    GetRun();
  int    GetLoc();
  char   GetTest();
  int    GetHVStep(); 
  
 private:

  void   SetFileID(string name);
  void   SetFileID();
  void   SetDir(string filePath);

  int  PMT;   
  int  Run;   
  int  Loc;   
  char Test;  
  int  HVStep;
  string FileID;
  string Dir;

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


// Option for use with string containing
// the FileID somewhere within it
FileNameParser::FileNameParser(string str_with_ID){
  
  Init();

  // strip path info leaving file name
  string name = GetFileName(str_with_ID);

  if( HasExtension(name) < 0 )
    name += ".root";
  
  PMT    = pmtID(name);
  Run    = run(name);
  Loc    = location(name);
  Test   = test(name);
  HVStep = hVStep(name); // 0 if Test!='G'
  
  allSet = true;

  SetFileID();
  
  SetDir(str_with_ID);
  
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
  
  SetDir(rawFilePath);
 
  Print_Data();

}

FileNameParser::~FileNameParser(){

}

void FileNameParser::Init(int userOption){

  printf("\n  ----------------------------- \n") ;
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
