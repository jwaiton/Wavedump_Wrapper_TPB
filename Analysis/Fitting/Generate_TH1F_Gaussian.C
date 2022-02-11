#include "TMath.h"
#include "TH1F.h"

#include "TCanvas.h"
#include "TGraph.h"

#include "TRandom.h"

TH1F * Generate_TH1F_Gaussian(double mean = 0.0, double sigma = 10.){
  
  // Canvas to draw on
  TCanvas * canvas =  new TCanvas("Canvas","Canvas");
  
  TRandom * rand = new TRandom();
  
  TH1F * hPed = new TH1F("hPed","hPed;Gain (mVns);Counts",100,-100.,1000.);

  double rnd = 0.0;

  for(int i = 0; i < 10000; i++ ){
    
    rnd = rand->Gaus(mean, sigma);
      
    hPed->Fill(rnd);
  }

  hPed->Draw();
 
  return hPed;

}
