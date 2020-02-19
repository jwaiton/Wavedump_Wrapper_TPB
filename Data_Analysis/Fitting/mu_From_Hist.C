#include <iomanip>
#include <iostream>

#include "TFile.h"
#include "TH1F.h"
#include "TLine.h"


#include "Poisson.C"
#include "GetValley.C"


using namespace std;

float Prob_zero_from_TH1F(TH1F * hQ,
 			  int low  = -100,
			  int high = 100){
  
  
  int binLow  = hQ->GetXaxis()->FindBin(low);
  int binHigh = hQ->GetXaxis()->FindBin(high);
  
  float prob = hQ->Integral(binLow,binHigh)/hQ->Integral();
  
  return prob;
}

float mu_From_Histo(TString rootFileName = "Run_30_PMT_133_Loc_3_Test_S"){

  float high = GetValley(rootFileName);

  cout << endl;
  cout << " charge at valley is " << setprecision(3) << high << " mVns " << endl;  
  
  TString hName = "hQ_Fixed_" + rootFileName;
  
  rootFileName = rootFileName + ".root";
  rootFileName = "./" + rootFileName;
  
  TFile * file = TFile::Open(rootFileName);
  
  TH1F * hQ_Fixed = (TH1F*)file->Get(hName);

  float P0 =  Prob_zero_from_TH1F(hQ_Fixed,-500,high);
  
  cout << endl;
  cout << " Poisson P(O) is " << setprecision(4) << P0*100 << " %" <<endl;  
  
  hQ_Fixed->Draw();
  gPad->SetLogy();

  TLine * l = new TLine(high,0,high,1000);
  l->SetLineColor(kRed);
  
  l->Draw("same");

  float mu =  Mu_from_prob_zero(P0);

  cout << endl;
  cout << " Poisson mean is " << setprecision(3) << mu  << " photoelectrons at first dynode" << endl;  
  cout << endl;
  
  return mu;
}
