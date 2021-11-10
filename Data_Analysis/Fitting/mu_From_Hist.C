#include <iomanip>
#include <iostream>

#include "TFile.h"
#include "TH1F.h"
#include "TF1.h"
#include "TLine.h"

#include "Poisson.C"
#include "GetPeakToValley.C"

#include <TLatex.h>

#include "wmStyle.C"

using namespace std;

float Prob_zero_from_TH1F(TH1F * hQ,
 			  int low  = -100,
			  int high = 100){
  
  int binLow  = hQ->GetXaxis()->FindBin(low);
  int binHigh = hQ->GetXaxis()->FindBin(high);
  
  float prob = hQ->Integral(binLow,binHigh)/hQ->Integral();
  
  return prob;
}

float mu_From_Hist(string rootFileName = "hQ_Fixed_Run_24_PMT_162_Loc_9_Test_N.root",string pathToData = "./"){
  
  TStyle *wmStyle = GetwmStyle();
  gStyle->SetOptTitle(0);
  gROOT->SetStyle("wmStyle");
  gROOT->ForceStyle();

  string hName = rootFileName;

  // strip away the .root extension
  if (!hName.empty()) {
    hName.resize(hName.size() - 5);
  }
  
  Result * result = GetPeakToValley(hName,
				    pathToData);
  
  float valley_Q = result->valley.value;
  float peak_Q   = result->peak.value;

  cout << endl;
  cout << " charge at valley is " << setprecision(3) << valley_Q << " mVns " << endl;  
  cout << " charge at peak is   " << setprecision(3) << peak_Q   << " mVns " << endl;  

  rootFileName = pathToData + rootFileName;

  TFile * file = TFile::Open(rootFileName.c_str());
  
  TH1F * hQ_Fixed = (TH1F*)file->Get(hName.c_str());
  
  float P0 =  Prob_zero_from_TH1F(hQ_Fixed,-500,valley_Q);
  
  cout << endl;
  cout << " Poisson P(O) is " << setprecision(4) << P0*100 << " %" <<endl;  

  hQ_Fixed->SetMinimum(1000);
  hQ_Fixed->GetXaxis()->SetRangeUser(-100,900);
  
  hQ_Fixed->Draw();
  gPad->SetLogy();
  
  TLine * lV = new TLine(valley_Q,0,valley_Q,result->valleyCounts.value);
  TLine * lP = new TLine(peak_Q,0,peak_Q,result->peakCounts.value);

  lV->SetLineColor(kBlue);
  lP->SetLineColor(kGreen+2);
  
  lV->Draw("same");
  lP->Draw("same");

  result->fPeak->SetLineColor(kGreen+2);
  result->fPeak->Draw("same");

  result->fValley->SetLineColor(kBlue);
  result->fValley->Draw("same");
  
  TLatex * latex = new TLatex(); 
  latex->SetNDC();
  latex->SetTextSize(0.04);;
  
  char text_ptv[64];
  sprintf(text_ptv," p:v \t\t %.2f",
	  result->peakToValley.value);

  char text_mu[64];
  sprintf(text_mu," mu \t\t %.2f p.e.",
	  result->mu.value);

  char text_valley[64];
  sprintf(text_valley," valley \t %.0f mVns",
	  result->valley.value);

  char text_peak[64];
  sprintf(text_peak," peak \t\t %.0f mVns",
	  result->peak.value);
  
  float gain = result->peak.value/400.;

  char text_gain[64];
  sprintf(text_gain," gain \t\t %.2f #times 10^{7}",
	  gain); 

  latex->DrawLatex(0.6,0.85,text_ptv);
  latex->DrawLatex(0.6,0.8,text_mu);
  latex->DrawLatex(0.6,0.75,text_valley);
  latex->DrawLatex(0.6,0.7,text_peak);
  latex->DrawLatex(0.6,0.65,text_gain);
  
  string plotName = "./Plots/";
  
  plotName += hName;
  plotName += ".pdf";
  
  gPad->SaveAs(plotName.c_str());

  float mu  =  Mu_from_prob_zero(P0);
  //float mu2 =  result->mu.value;

  cout << endl;
  cout << " Poisson mean is " << setprecision(3) << mu   << " photoelectrons at first dynode" << endl;  
  //cout << " Poisson mean is " << setprecision(3) << mu2  << " photoelectrons at first dynode" << endl;  
  cout << endl;
  
  return mu;
}
