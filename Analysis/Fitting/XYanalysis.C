#include <iostream>

#include "TFile.h"
#include "TH1F.h"
#include "TF1.h"
#include "TLine.h"

#include "efficiency.C"
//#include "Poisson.C"
//#include "Fit_PeakAndValley.C"

#include <TLatex.h>

//#include "wmStyle.C"

// Create structure with important values

typedef struct {

  // ValueWithError from Fit_PeakAndValley.C to contain the uncertainty	
  ValueWithError mu;
  ValueWithError peakToValley;
  ValueWithError efficiency;
  ValueWithError peakTime;
  ValueWithError gain;
} Data;

Data* XYanalysis(TString rootFileName = "hQ_Fixed_Run_24_PMT_162_Loc_9_Test_N.root", string pathToData = "./"){

	// Adjusting the file name for different operations
	
	// Removing .root
	string hName(rootFileName.Data());
  	if (!hName.empty()) {
  		hName.resize(hName.size() - 5);
  	}
 
	// Applying pathToData, which can be adjusted
	TString rootFilePath = pathToData;
	rootFilePath += rootFileName;
	TFile * rootFile = new TFile(rootFilePath);
	
	// Collecting mu and PV
	Data* data = new Data();
	Result * result = Fit_PeakAndValley((TH1F*)rootFile->Get(hName.c_str()));

	// Take efficiency.C to variable, then take important values
	data->efficiency = efficiency(rootFileName);
	// Timing

	//Return data
	data->mu = result->mu;
	data->peakToValley = result->peakToValley;
	data->gain.value = result->peak.value/400.;
	data->gain.error = result->peak.error/400.;
	//
	//Test to prove that its easy to pull these values use, and how to do so
	cout << " P:V ratio = " << data->peakToValley.value << " (" << data->peakToValley.error << ") " << endl;
        cout << " Efficiency = " << data->efficiency.value << " (" << data->efficiency.error << ") " << endl;
	return data;

}

