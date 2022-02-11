#include "Fit_PeakAndValley.C"
#include "TFile.h"
#include "TString.h"

using namespace std;

Result* Fit_PeakAndValley(TH1F*  fhisto);

Result * GetPeakToValley(TString rootFileName = "Run_30_PMT_133_Loc_3_Test_S",string pathToData = "./"){
  
  //TString hName = "hQ_Fixed_" + rootFileName;
  TString hName = rootFileName;
  
  
  rootFileName = rootFileName + ".root";

  TString rootFilePath = pathToData;
  rootFilePath += rootFileName;
  
  TFile * rootFile = new TFile(rootFilePath);
  
  Result * result = Fit_PeakAndValley((TH1F*)rootFile->Get(hName));
  
  cout << endl;
  cout << " peak           = " << result->peak.value         << " (" << result->peak.error         << ") " << endl;
  cout << " valley         = " << result->valley.value       << " (" << result->valley.error       << ") " << endl;
  cout << " peak to valley = " << result->peakToValley.value << " (" << result->peakToValley.error << ") " << endl;
  cout << " peak counts    = " << result->peakCounts.value   << " (" << result->peakCounts.error   << ") " << endl;
  cout << " valley counts  = " << result->valleyCounts.value << " (" << result->valleyCounts.error << ") " << endl;

  return result;
}

