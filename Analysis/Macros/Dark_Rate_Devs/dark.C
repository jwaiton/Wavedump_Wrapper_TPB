#include "iostream"

#include "TROOT.h"

#include "TStyle.h"
#include "TMath.h"

#include "TCanvas.h"
#include "TLegend.h"
#include "TH1.h"
#include "TGraphErrors.h"

#include "wmStyle.C"

// Common tools
#include "dark_rate_at_temp.h"
#include "ShippingData.h"
#include "ShippingData.C"

using namespace std;

// To do: change to use shipping data
float N25(int pmt = 15){
  
  double pmts[8] = {15,  16,  82,  107, 131, 148, 166, 171};
  double DRs_HPK[8] = {2200,2000,2700,3900,2400,2100,2300,2500};
  
  int i = 0;
  for (; i < 8 ; i++ ){
    if ( pmt == pmts[i])
      break;
  }
  
  return DRs_HPK[i];
}

void analyse_dark(){

  gDirectory->DeleteAll();

  while(gPad!=nullptr){
    gPad->Close();
  }

  ShippingData * sD = new ShippingData(15);
  
  TStyle * wmStyle = GetwmStyle();
  gROOT->SetStyle("wmStyle");
  gROOT->ForceStyle();

  TCanvas * c1;

  c1 = new TCanvas("c1","c1",1000,0,400,300);

  int nPMTs = 8;
  double pmts[8] = {15,  16,  82,  107, 131, 148, 166,171};
  
  double DRs_HPK_L[8];
  double DRs_HPK_H[8];
  double DRs_HPK_err[8];
  double DRs_HPK[8] = {};
  
  double DRs_v1[8] = {2098,1387,1906,2752,1885,1162,1299,1435};
  double DRs_v1_err[8] = {100,100,100,100,100,100,100,100};
  
  TH1D * hDarkH = new TH1D("hDarkH","hDarkH;PMT ;Dark Rate (Hz)",100,0,200);
  TH1D * hDark1 = new TH1D("hDark1","hDark1;PMT ;Dark Rate (Hz)",100,0,200);
  double maxi = 0;
  
  for (int i = 0 ; i < nPMTs ; i++){

    sD->SetNewPMT(pmts[i]);
        
    DRs_HPK_L[i] = dark_rate_at_temp(sD->GetDR(),22.5);
    DRs_HPK[i]   = dark_rate_at_temp(sD->GetDR(),23.0);
    DRs_HPK_H[i] = dark_rate_at_temp(sD->GetDR(),23.5);
   
    DRs_HPK_err[i]= TMath::Max(abs(DRs_HPK_L[i]-DRs_HPK[i]),
				      abs(DRs_HPK_H[i]-DRs_HPK[i]));
    
    hDarkH->Fill(pmts[i],DRs_HPK[i]);
    hDark1->Fill(pmts[i],DRs_v1[i]);
  }

  for (int i = 0 ; i < nPMTs ; i++){
    hDark1->SetBinError(hDark1->FindBin(pmts[i]),DRs_v1_err[i]);
    hDarkH->SetBinError(hDarkH->FindBin(pmts[i]),DRs_HPK_err[i]);

    
    /* cout << " BinError (pmt " << pmts[i] << ") = " */
    /* 	      << DRs_v1_err[i] << endl; */
    /* cout << " BinError (pmt " << pmts[i] << ") = " */
    /* 	      << hDark1->GetBinError(hDark1->FindBin(pmts[i])) << endl; */
    /* cout << " HPK Error (pmt " << pmts[i] << ") = " */
    /* 	      << hDarkH->GetBinError(hDarkH->FindBin(pmts[i])) << endl; */
    
    maxi = TMath::Max(maxi,DRs_HPK[i]);
    maxi = TMath::Max(maxi,DRs_v1[i]);
    
  }
  
  //cout << " maxi = " << maxi << endl;
    
  hDarkH->SetLineColor(kBlack);
  //hDarkH->SetFillColor(kBlack);
  hDarkH->SetMaximum(maxi*1.2);
  
  hDarkH->Draw("HIST E");
  hDark1->SetLineColor(kBlue);
  hDark1->SetMarkerColor(kBlue);
  hDark1->Draw("HIST E1 SAME");

  TCanvas * c2 = new TCanvas("c2","c2",1000,350,400,300); 
  
  TGraphErrors * g1 = new TGraphErrors(nPMTs,
				       DRs_HPK,    DRs_v1,
				       DRs_HPK_err,DRs_v1_err); 

  
  TLatex *latex[7];
  char N_PMT[4];

  for (int i = 0 ; i < nPMTs ; i++){
    sprintf(N_PMT,"%.0f",pmts[i]);
    latex[i] = new TLatex(g1->GetX()[i], g1->GetY()[i],N_PMT);
    latex[i]->SetTextSize(0.05);// latex->SetTextColor(kRed);
    g1->GetListOfFunctions()->Add(latex[i]); 

  }
  
  g1->SetTitle(";HPK Dark Rate temp-scaled (Hz);Lab Dark Rate (Hz)");
  
  g1->SetMinimum(1000);
  g1->SetMaximum(3000);

  g1->GetXaxis()->SetLimits(1000.,3000.);
  g1->Draw("A*");
  
  TLine * l2 = new TLine(1100,1100,2900,2900);
  l2->SetLineColor(kBlue);
  l2->SetLineStyle(2);
  l2->Draw();
  
  //double noise_rate_10_mV[8] = {166.5,125.0,74.0,186.0,73.5,56.5,113.5,126.5};

  TCanvas * c3 = new TCanvas("c3","c3",1000,700,400,300);

  TH1D * hTD[8] = {NULL};

  char name[128];
  char title[128];
  
  for (int i = 0 ; i < 8 ; i++){

    sprintf(name,"hTD_%d",i);
    sprintf(title,"hTD_%d;Temp (C) ;Dark Rate(Hz)",i);
    
    hTD[i] = new TH1D(name,title,31,-0.5,30.5);
      
  }   
  //  double pmts[8] = {15,  16,  82,  107, 131, 148, 166,171};

  
  for (int j = 0 ; j < 31 ; j++){
    for (int i = 0 ; i < 8 ; i++){
    
      hTD[i]->Fill(double(j),dark_rate_at_temp(N25(pmts[i]),double(j)));
      
      //cout << " PMT " << pmts[i] << " Rate (" << j << ") = " << dark_rate_at_temp(N25(pmts[i]),double(j)) << endl;
    }
  }
  
  TLegend * leg;
  leg = new TLegend(0.2,0.5,0.4,0.9);

  int colours[] = {1,2,3,4,5,6,7,8,9,41,42,43};
  
  for (int i = 7 ; i >= 0 ; i--){

    hTD[i]->SetMarkerColor(colours[i]);
    hTD[i]->SetLineColor(colours[i]);
    hTD[i]->SetLineWidth(2);

    sprintf(N_PMT,"%.0f",pmts[i]);
    leg->AddEntry(hTD[i],N_PMT,"LP");
    hTD[i]->Draw("hist PC same");

  }
  leg->Draw();

  TCanvas * c4 = new TCanvas("c4","c4",1450,0,400,300);
  double lambda_R_arr[8];
 
  for (int i = 0 ; i < 8 ; i++)
    lambda_R_arr[i] = A_G(N25(pmts[i]))/A_0();

  TGraph * g = new TGraph(8,pmts,lambda_R_arr);

  g->SetTitle(";PMT; #lambda");

  g->Draw("AP");
  
}    

void dark(){
  analyse_dark();
}
