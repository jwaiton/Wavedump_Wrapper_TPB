#include "Fit_PeakAndValley.C"
#include "TFile.h"
#include "TString.h"

using namespace std;

Result* Fit_PeakAndValley(TH1F*  fhisto);

Result * GetPeakToValley(TString rootFileName = "Run_30_PMT_133_Loc_3_Test_S"){

  TString hName = "hQ_Fixed_" + rootFileName;
      
  rootFileName = rootFileName + ".root";

  //!!!!!
  // set to relevant local path
  TString RawRootDataDIR = "./";
  
  rootFileName = RawRootDataDIR + rootFileName;
  
  TFile * rootFile = new TFile(rootFileName);
  
  Result * result = Fit_PeakAndValley((TH1F*)rootFile->Get(hName));
  
  cout << endl;
  cout << " peak           = " << result->peak.value         << " (" << result->peak.error         << ") " << endl;
  cout << " valley         = " << result->valley.value       << " (" << result->valley.error       << ") " << endl;
  cout << " peak to valley = " << result->peakToValley.value << " (" << result->peakToValley.error << ") " << endl;
  cout << " peak counts    = " << result->peakCounts.value   << " (" << result->peakCounts.error   << ") " << endl;
  cout << " valley counts  = " << result->valleyCounts.value << " (" << result->valleyCounts.error << ") " << endl;

  return result;
}

