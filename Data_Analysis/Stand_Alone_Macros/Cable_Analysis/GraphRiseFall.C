#include "TGraphErrors.h"


void GraphRiseFall(){
  
  static const Int_t nRuns = 10;
  
  Double_t cableLengths[] = {80,64,48,32,28,24,20,16,8,2};
  
  Double_t riseVal[] = {4.17,4.06,3.96,3.84,3.83,3.79,3.72,3.62,3.52,3.52};
  Double_t riseDev[] = {0.62,0.72,0.72,0.73,0.88,0.78,0.86,0.67,0.93,0.84};
  Double_t riseErr[] = {0.03,0.03,0.03,0.03,0.04,0.03,0.04,0.03,0.04,0.04};

  Double_t fallVal[] = {20.71,17.90,15.74,14.57,12.97,11.87,11.39,10.52,9.45,9.05};
  Double_t fallDev[] = {2.87, 3.13, 3.15, 3.17, 3.51, 3.56, 3.46, 3.02, 3.49,3.55}; 
  Double_t fallErr[] = {0.13, 0.14, 0.14, 0.14, 0.16, 0.16, 0.16, 0.14, 0.16,0.16};

  for (int iLength = 0; iLength < 10 ; iLength++){
    riseErr[iLength] = 2*riseErr[iLength];
    fallErr[iLength] = 2*fallErr[iLength];
  }
  
  TGraph       * gRise    = new TGraph(nRuns,cableLengths,riseVal);
  TGraphErrors * gRiseDev = new TGraphErrors(nRuns,cableLengths,riseVal,0,riseDev);
  TGraphErrors * gRiseErr = new TGraphErrors(nRuns,cableLengths,riseVal,0,riseErr);
  
  TGraph       * gFall    = new TGraph(nRuns,cableLengths,fallVal);
  TGraphErrors * gFallDev = new TGraphErrors(nRuns,cableLengths,fallVal,0,fallDev);
  TGraphErrors * gFallErr = new TGraphErrors(nRuns,cableLengths,fallVal,0,fallErr);
 
  TCanvas * canvas =  new TCanvas("Graphs","Graphs");
  TH1F *hr;
  hr = canvas->DrawFrame(0.,0.0,90.,5.0);
  //hr->SetTitle("");

  gStyle->SetTitleFont(40,"");
  gStyle->SetTitleFontSize(0.035);
  
  hr->GetXaxis()->SetTitle("Cable length (m)");
  hr->GetYaxis()->SetTitle("Mean rise time (ns) ");
  hr->Draw();

  gRiseDev->SetMinimum(0);
  
  
  gRiseDev->Draw("same");
  canvas->SaveAs("./Rise.pdf");
  
  hr = canvas->DrawFrame(0.,0.0,90.,25.0);
  hr->GetXaxis()->SetTitle("cable length (m)");
  hr->GetYaxis()->SetTitle("Mean fall time (ns) ");
  hr->Draw();

  gFall->Draw();
  gFallDev->Draw();
  gFallErr->Draw("same");
  
  canvas->SaveAs("./Fall.pdf");
  
  // Draw Rise and Fall on one
  canvas->Clear();

  hr = canvas->DrawFrame(0.,0.0,90.,25.0);
  hr->GetXaxis()->SetTitle("cable length (m)");
  hr->GetYaxis()->SetTitle("mean rise and fall times (ns)");
  
  gFall->SetLineColor(kBlack);
  gFall->SetLineStyle(0);
  gFall->SetMarkerColor(kBlack);
  gFall->SetMarkerStyle(2);
  gFall->Draw("same L P");
  
  gFallDev->SetLineColor(kBlack);
  gFallDev->Draw("same ");

  gFallErr->SetLineColor(kBlack);
  gFallErr->SetMarkerColor(kBlack);
  gFallErr->SetMarkerStyle(1);
  gFallErr->Draw("same ");

  gRise->SetLineColor(kGray+2);
  gRise->SetLineStyle(2);
  gRise->SetMarkerColor(kGray+2);
  gRise->SetMarkerStyle(2);
  gRise->Draw("same L P");
  
  gRiseDev->SetLineColor(kGray+2);
  gRiseDev->Draw("same ");

  //gRiseErr->SetLineColor(kGray+2);
  //gRiseErr->SetMarkerColor(kGray+2);
  gRiseErr->SetMarkerStyle(2);
  gRiseErr->Draw("same ");
  
  canvas->SaveAs("./Rise_And_Fall.pdf");
}

