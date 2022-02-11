#include <iomanip>
#include <iostream>

#include "TFile.h"
#include "TH1F.h"
#include "TF1.h"
#include "TLine.h"

#include "Poisson.C"
#include "Fit_PeakAndValley.C"

#include <TLatex.h>

#include "wmStyle.C"

using namespace std;

// Testing if this works


ValueWithError efficiency(TString rootFileName = "hQ_Fixed_Run_13_PMT_162_Loc_9_Test_N.root", 
 			  int low  = 100,
			  int high = 1000){
 
  // This is the method from GetPeakToValley.C
  string hName(rootFileName.Data());	
  TString rootFilePath = "./";
  rootFilePath += rootFileName;
  
  TFile *rootFile = new TFile(rootFilePath);
  
  // strip away the .root extension and apply ';1' to the end.
  if (!hName.empty()) {
  	hName.resize(hName.size() - 5);
	hName.append(";1");
  }
  //rootFile->ls();
  // Apply hName to histogram 
  TH1F * hQ = (TH1F*)rootFile->Get(hName.c_str());
  // Collect lowest and highest bins (defined in function arguments)
  int binLow  = hQ->GetXaxis()->FindBin(low);
  int binHigh = hQ->GetXaxis()->FindBin(high);
  // Calculate the probability of bin range against the entire integral.
  double prob = hQ->Integral(binLow,binHigh)/hQ->Integral();
  // Calculate poisson error
  // Taking N as the number of data samples taken (how to do so)
  float N = hQ->GetEntries();
  double error = sqrt(prob*(1-prob)/N);

  // return probability with error
  ValueWithError pro;
  pro.value = prob;
  pro.error = error;
  return pro;
}
