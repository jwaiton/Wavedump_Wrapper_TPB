#ifndef FileNameParser_h
#define FileNameParser_h 

#include <string>
#include <cstdlib> 
#include <iostream>

class FileNameParser {
public:
  
  FileNameParser();
  FileNameParser(std::string);
  ~FileNameParser();
  
  int    parseInt(std::string f, std::string s1, std::string s2);
  char   parseChar(std::string f, std::string s1, std::string s2);
  std::string GetFileName(std::string filePath);
  std::string GetFileID(std::string fileName);
  std::string GetTreeName(std::string filePath);
  int    pmtID(std::string filename);
  int    run(std::string filename);
  int    location(std::string filename);
  char   test(std::string filename);
  int    hVStep(std::string filename);
  
  // private:
  int  PMT;   
  int  Run;   
  int  Loc;   
  char Test;  
  int  HVStep;
  std::string FileID;
};

#endif

#ifdef FileNameParser_cxx

FileNameParser::FileNameParser(){
  PMT    = 0;   
  Run    = 0;   
  Loc    = -1;   
  Test   = 'A';  
  HVStep = -1;
}

// Option for use with TTree name
FileNameParser::FileNameParser(std::string treeName){
  
  std::string fileName = GetFileName(treeName);
  
  // change to same format as root filename
  fileName = fileName + ".root";
  
  FileID = GetFileID(fileName);

  std::cout << " FileID = " << FileID << std::endl;
  
  PMT    = pmtID(fileName);
  Run    = run(fileName);
  Loc    = location(fileName);
  Test   = test(fileName);
  HVStep = hVStep(fileName); // 0 if Test!='G'
  
  
  std::cout << std::endl;
  std::cout << " Run    = " << Run    << std::endl;
  std::cout << " PMT    = " << PMT    << std::endl;
  std::cout << " Loc    = " << Loc    << std::endl;
  std::cout << " Test   = " << Test   << std::endl;
  
  if(Test=='G')
    std::cout << " HVStep = " << HVStep << std::endl;
  
}

FileNameParser::~FileNameParser(){

}
    
#endif
