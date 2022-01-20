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

  int    PMT;   
  int    Run;   
  int    Loc;   
  char   Test;  
  int    HVStep;
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
  if(Test=='G')
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

  // extract meta data from file path
  if     (option == 1){
    
    PMT  = pmtID(rawFilePath);
    Run  = run(rawFilePath);
    Loc  = location(rawFilePath);
    Test = test(rawFilePath);
    HVStep = hVStep(rawFilePath);
    
  } // meta data from user input
  else if(option == 2){

    char K = 'N';

    while( K!='Y' && K!='y'){

      printf(" \n Enter PMT number: ");
      scanf(" %d",&PMT);  
      
      printf(" PMT = %d \n ",PMT);
      
      printf("\n Okay? (Enter:Y/N) \n ");
      scanf(" %c",&K);
      
    }

    K = 'n';
    
    while( K!='y' && K!='Y'){
      printf(" \n Enter Run number: ");
      scanf(" %d", &Run);  
      printf(" Run = %d \n Okay? (Enter:Y/N)",Run);
      scanf(" %c", &K);  
    }

    K = 'n';
    
    while( K!='y' && K!='Y'){
      printf(" \n Enter Loc number: ");
      scanf(" %d", &Loc);  
      printf(" Loc = %d \n Okay? (Enter:Y/N)",Loc);
      scanf(" %c", &K);  
    }

    K = 'n';
    
    while( K!='y' && K!='Y'){
      printf(" \n Enter Test type character: ");
      printf(" \n e.g. 'N','D','A','G': ");
      scanf(" %c", &Test);  
      printf(" Test = %c \n Okay? (Enter:Y/N)",Test);
      scanf(" %c", &K);  
    }

    K = 'n';
    
    while( K!='y' && K!='Y' && Test=='G'){
      printf(" \n Enter HV step number: ");
      scanf("%d", &HVStep);  
      printf(" HVStep = %d \n Okay? (Enter:Y/N)",HVStep);
      scanf(" %c", &K);  
    }
    
  }
  
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
