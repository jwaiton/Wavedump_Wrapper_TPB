#include "TFile.h"
#include "TTree.h"
#include "PMTAnalyser.h"

int main(Int_t argc, Char_t *argv[]){
  
  TString testName = "Run_11_PMT_90_Loc_0_Test_D";
  
  TString fileName = "../../RawRootData/" + testName;
  fileName += ".root";
  
  TString treeName = "Events_" + testName;

  TFile *file = new TFile(fileName,"READ");
  
  if (!file || !file->IsOpen()) {
    file = new TFile();
    return -1;
  }
  
  TTree * tree;
  file->GetObject(treeName,tree); 
    
  PMTAnalyser * analysePMT = new PMTAnalyser(tree);
  
  analysePMT->Loop();

  
}
