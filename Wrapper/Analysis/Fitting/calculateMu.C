#include "TMath.h"
#include "TH1F.h"

float calculateMu(TH1F * hQ_Fixed ){
  
  int binLow  = hQ_Fixed->GetXaxis()->FindBin(-100);
  int binHigh = hQ_Fixed->GetXaxis()->FindBin(100);
  
  float mu = hQ_Fixed->Integral(binLow,binHigh)/hQ_Fixed->Integral();
  
  mu = -1*TMath::Log(mu);
  
  return mu;
}

float calculateProb(float n, float mu){

  float prob = TMath::Exp(-1*mu)*TMath::Power(mu,n)/TMath::Factorial(n);

  return prob;
  
}
