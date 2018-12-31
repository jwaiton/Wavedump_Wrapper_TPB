#include<iostream>

#include <string>
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "PMTAnalyser.h"
#include "fnParser.h"

using namespace std;


int main(Int_t argc, Char_t *argv[]){
  
  //string filePath = "../../RawRootData/Run_11_PMT_90_Loc_0_Test_D.root";

  TFile *file = nullptr;
  TTree * tree = nullptr;
  
  PMTAnalyser * analysePMT = nullptr;
  
  for( int iFile = 1 ; iFile < argc ; iFile++){
     
     file = new TFile(argv[iFile],"READ");
     
     if (!file || !file->IsOpen()) {
       file = new TFile();
   
       cerr << " File Error " << endl; 
       return -1;
     }

     //-------
     cout << endl;
     cout << " Run      " << run(argv[iFile])   << endl;
     cout << " PMT      " << pmtID(argv[iFile]) << endl;
     cout << " Location " << location(argv[iFile]) << endl;

//      cout << GetFileName(argv[iFile]) << endl;
//      cout << GetTreeName(argv[iFile]) << endl;
     
     file->GetObject((TString)GetTreeName(argv[iFile]),tree); 
     
     analysePMT = new PMTAnalyser(tree);
     
     analysePMT->Loop();
  
  }

  
  return 1;
}
