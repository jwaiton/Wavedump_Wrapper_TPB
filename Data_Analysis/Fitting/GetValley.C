#include "Fit_PeakAndValley.C"
#include "TFile.h"
#include "TString.h"

using namespace std;

Result* Fit_PeakAndValley(TH1F*  fhisto);

float GetValley(TString rootFileName = "Run_30_PMT_133_Loc_3_Test_S"){

  TString hName = "hQ_Fixed_" + rootFileName;
      
  rootFileName = rootFileName + ".root";

  //!!!!!
  // set to relevant local path
  TString RawRootDataDIR = "./";
  
  rootFileName = RawRootDataDIR + rootFileName;
  
  TFile * rootFile = new TFile(rootFileName);
  
  Result * results = Fit_PeakAndValley((TH1F*)rootFile->Get(hName));
  return results->valley.value;
  
}

