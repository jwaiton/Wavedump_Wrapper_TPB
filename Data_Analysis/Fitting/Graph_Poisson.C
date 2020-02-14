#include "TMath.h"
#include "TH1F.h"

#include "TCanvas.h"
#include "TGraph.h"

#include "Poisson.C"

void Graph_Prob_n(int nPhot = 2){
  
  // Canvas to draw on
  TCanvas * canvas =  new TCanvas("Graph","Graph");

  // dummy histogram for controlling canvas
  TH1F * hr;
  
  // extend to include more data points?
  static const int nPoints = 100;
  
  // automate array fill within for loop? 
  //double mu[nPoints] = {0.10,0.15,0.20,1.0,5.0};
  double mu[nPoints];
  
  double prob[nPoints];
  
  // populate probabilities
  for(int i = 0; i < nPoints; i++ ){

    mu[i] = (float)i/nPoints*10.; 
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
