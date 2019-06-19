#include "TFile.h"
#include "TH1F.h"
#include "calculateMu.C"

float runCalculateMu(TString rootFileName = "Run_43_PMT_82_Loc_0_Test_N"){

  TString hName = "hQ_Fixed_" + rootFileName;

  rootFileName = rootFileName + ".root";

  rootFileName = "/home/ubuntu/WATCHMAN/RootData/" + rootFileName;
  
  TFile *file = TFile::Open(rootFileName);
  
  TH1F* hQ_Fixed = (TH1F*)file->Get(hName);

  float mu = calculateMu(hQ_Fixed);

  return mu;
}
