#include "TMath.h"
#include "TH1F.h"

#include "TCanvas.h"
#include "TGraph.h"

float Mu_from_prob_zero(float prob_zero){
  return (-1*TMath::Log(prob_zero));
}

float Prob_n_from_prob_zero(float prob_zero, float n){
  return prob_zero*TMath::Power(Mu_from_prob_zero(prob_zero),n)/TMath::Factorial(n);
}

float Prob_zero_from_mu(float mu){
  return TMath::Exp(-mu);
}

float Prob_n_from_mu(float mu, int n){
  return TMath::Exp(-mu)*TMath::Power(mu,n)/TMath::Factorial(n);
}

// float Prob_zero_from_TH1F(TH1F * hQ, int thresh_low -100, int thres_high = 100){
  
//   int binLow  = hQ->GetXaxis()->FindBin(thresh_low);
//   int binHigh = hQ->GetXaxis()->FindBin(thres_high);
  
//   float prob = hQ->Integral(binLow,binHigh)/hQ_Fixed->Integral();
  
//   return prob;
// }


void Graph_Prob_n(int nPhot = 2){
  
  // Canvas to draw on
  TCanvas * canvas =  new TCanvas("Graph","Graph");

  // dummy histogram for controlling canvas
  TH1F    *hr;
  
  // extend to include more data points?
  static const int nPoints = 5;
  
  // automate array fill within for loop? 
  double mu[nPoints] = {0.10,0.15,0.20,1.0,5.0};
  
  double prob[nPoints];
  
  // populate probabilities
  for(int i = 0; i < nPoints; i++ ){
    prob[i] = Prob_n_from_mu(mu[i],nPhot);
  }
  
  // histogram settings
  
  // ranges
  float yMax = 1.0;
  hr = canvas->DrawFrame(0.0,0.0,10.0,yMax);
  
  hr->SetTitle(" Poisson Statistics");
  hr->GetXaxis()->SetTitle(" mu " );
  
  // A C way of doing things
  char yAxis[164];
  sprintf(yAxis,"P(%d)",nPhot);
  hr->GetYaxis()->SetTitle(yAxis);
  hr->Draw();

  TGraph * gr = new TGraph(nPoints,mu,prob);

  gr->SetLineColor(kBlue);  
  gr->SetMarkerColor(kBlue);  
  gr->SetMarkerSize(.5);
  gr->SetMarkerStyle(21);     
  
  // for draw options:
  //   https://root.cern.ch/doc/master/classTGraphPainter.html
  gr->Draw("PL");

}
