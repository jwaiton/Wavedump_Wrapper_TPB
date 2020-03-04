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
  string name = filePath.substr(pos1,filePath.size() - pos1);  
  return name;
}

void FileNameParser::SetDir(string filePath){

  if(option > 0){
    int pos1 = filePath.find("wave_",0);
    Dir = filePath.substr(0,pos1);  
  }
  else{
    int pos1 = filePath.find("Events_",0);
    
    if(pos1==-1)
      pos1 = filePath.find("Run_",0);
    
    Dir = filePath.substr(0,pos1);  
  }
  
}

string FileNameParser::GetDir(){
  return Dir;
}

int FileNameParser::HasExtension(string name){
  return name.find(".",0); 
}

// Set using file name directly
void FileNameParser::SetFileID(string name){
  
  if(option < 0)
    FileID = name.substr(0,name.size() - 5);
  else{
    char buff[128];
    sprintf(buff,"Run_%d_PMT_%d_Loc_%d_Test_%c",Run,PMT,Loc,Test);
    FileID = buff;
  }
}

// Set using data members
// which must have been set already
void FileNameParser::SetFileID(){
  
  if(allSet){
    char buff[128];
    sprintf(buff,"Run_%d_PMT_%d_Loc_%d_Test_%c",Run,PMT,Loc,Test);
    FileID = buff;
  }
  else{
    cerr << "Error: data members not set " << endl;
  }

}

// Get directly from file name
string FileNameParser::GetFileID(string name){
  return name.substr(0,name.size() - 5);;
}

// Get data member which must 
//  have already been set
string FileNameParser::GetFileID(){
  return FileID;
}

void FileNameParser::Print_Data(){
  
  printf(" \n   FileID = %s", FileID.c_str());
  printf(" \n   Run    = %d", Run);
  printf(" \n   PMT    = %d", PMT);
  printf(" \n   Loc    = %d", Loc);
  printf(" \n   Test   = %c", Test);

  if(Test=='G')
    printf(" \n   HVStep   = %d", HVStep);

  printf(" \n   Dir    = %s", Dir.c_str());  

  printf(" \n ");
}

string FileNameParser::GetTreeName(string filePath, int option = 0){
  
  string treeName = "Events_" + GetFileID(GetFileName(filePath));
  
  return treeName; 
}

string FileNameParser::GetTreeName(string filePath){
  
  string treeName = "Cooked_" + GetFileID(GetFileName(filePath));
  
  return treeName; 
}

string FileNameParser::GetTreeName(){
  
  string treeName = "Events_" + GetFileID();
  
  return treeName; 
}


string FileNameParser::Get_hQ_Fixed_Name(string filePath){
  
  string hName = "hQ_Fixed_" + GetFileID(GetFileName(filePath));
  
  return hName; 
}

string FileNameParser::Get_hQ_Fixed_Name(){
  
  string hName = "hQ_Fixed_" + GetFileID();
  
  return hName; 
}

int FileNameParser::pmtID(string name){

  if(option < 0)
    return parseInt(name,"PMT_","Loc");
  else
    return parseInt(name,"/PMT0",4);
}

int FileNameParser::run(string name){
 
  if(option < 0)
    return parseInt(name,"Run_","PMT0");
  else
    return parseInt(name,"/RUN",6);
}

int FileNameParser::location(string name){
  
  if(option < 0)
    return parseInt(name,"Loc_","Test");
  else
    return parseInt(name,"wave_","dat");
}

char FileNameParser::test(string name){ 
  
  char ctr; 
  
  if(option < 0)
    ctr = parseChar(name,"Test_","root");
  else 
    ctr = parseChar(name,"/PMT0");
  
  if( ctr > '0' && ctr < '6' ) 
    return 'G';
  else
    return ctr;
} 

int FileNameParser::hVStep(string name){ 

  if(test(name)=='G')
    return parseInt(name,"Test_","root");
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
  return 'E';
}

int    FileNameParser::GetHVStep(){
  if(allSet)
    return HVStep;
  else
    cerr << "Error: data members not set " << endl;
  return -1;
}

