#include "TFile.h"
#include "TH1F.h"
#include "calculateMu.C"

float runCalculateMu(TString rootFileName = "Run_30_PMT_133_Loc_3_Test_S"){
  
  TString hName = "hQ_Fixed_" + rootFileName;

  rootFileName = rootFileName + ".root";

  TFile *file = TFile::Open(rootFileName);
  
  TH1F* hQ_Fixed = (TH1F*)file->Get(hName);

  float mu = calculateMu(hQ_Fixed);

  return mu;
}
