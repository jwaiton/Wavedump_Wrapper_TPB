#include "TMath.h"
#include "TH1F.h"

#include "TCanvas.h"
#include "TGraph.h"

using namespace TMath;

float Mu_from_prob_zero(float prob_zero){
  return (-1*Log(prob_zero));
}

float Prob_n_from_prob_zero(float prob_zero, float n){
  return prob_zero*Power(Mu_from_prob_zero(prob_zero),n)/Factorial(n);
}

float Prob_zero_from_mu(float mu){
  return Exp(-mu);
}

float Prob_n_from_mu(float mu, int n){
  return Exp(-mu)*Power(mu,n)/Factorial(n);
}
