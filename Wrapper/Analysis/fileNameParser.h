#ifndef _fileNameParse_h
#define _fileNameParse_h 1
int parse(std::string f, std::string s1, std::string s2){
 
  int pos1 = f.find(s1,0);
  int pos2 = f.find(s2,pos1);
  std::string str = f.substr(pos1,pos2 - pos1);
  int first = str.find("_");
  str = str.substr(first+1);
  str = str.substr(0, str.size()-1);
  return std::stoi(str);
  
}

string GetFileName(string filePath){

  int pos1 = filePath.find("Run_",0);
  string fileName = filePath.substr(pos1,filePath.size() - pos1);  
  return fileName;
}

string GetFileID(string fileName){
  return fileName.substr(0,fileName.size() - 5);
}

string GetTreeName(string filePath){
  
  string treeName = "Events_" + GetFileID(GetFileName(filePath));
  
  return treeName; 
}

int pmtID(string filename){
  return parse(filename,"PMT_","Loc");
}

int run(string filename){
   return parse(filename,"Run_","PMT");
}

int location(string filename){
  return parse(filename,"Loc_","Test");
}

#endif

/* void stringmess(){

  string runname = "Run_4_PMT_90_Loc_0_Test_N.root";
  cout <<pmtID(runname)  <<  " " << run(runname) <<  " " << location(runname)<< endl; 
}
*/
