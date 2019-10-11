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
  string GetFileID(string fileName);
  string GetTreeName(string filePath);
  string Get_hQ_Fixed_Name(string filePath);
  int    pmtID(string filename);
  int    run(string filename);
  int    location(string filename);
  char   test(string filename);
  int    hVStep(string filename);

  int    GetPMT();
  int    GetRun();
  int    GetLoc();
  char   GetTest();
  int    GetHVStep(); 
  
 private:
  
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
  
  FileID = GetFileID(fileName);

  cout << endl;
  cout << " FileID = " << FileID << endl;
  
  PMT    = pmtID(fileName);
  Run    = run(fileName);
  Loc    = location(fileName);
  Test   = test(fileName);
  HVStep = hVStep(fileName); // 0 if Test!='G'
  
  allSet = true;

  cout << endl;
  cout << " Run    = " << Run    << endl;
  cout << " PMT    = " << PMT    << endl;
  cout << " Loc    = " << Loc    << endl;
  cout << " Test   = " << Test   << endl;
  
  if(Test=='G')
    cout << " HVStep = " << HVStep << endl;

}

FileNameParser::FileNameParser(string rawFilePath,
			       int userOption){
  Init(userOption);


  cout << " PMT  = " << pmtID(rawFilePath)    << endl;
  cout << " Run  = " << run(rawFilePath)      << endl;
  cout << " Loc  = " << location(rawFilePath) << endl;
  cout << " Test = " << test(rawFilePath)     << endl; 
  
  PMT  = pmtID(rawFilePath);
  Run  = run(rawFilePath);
  Loc  = location(rawFilePath);
  Test = test(rawFilePath);
  
  allSet = true;

}

FileNameParser::~FileNameParser(){

}

void FileNameParser::Init(int userOption){

  allSet = false;

  PMT    = 0;   
  Run    = 0;   
  Loc    = -1;   
  Test   = 'A';  
  HVStep = -1;
  
  option = userOption;
  
}
    
#endif
