#include "TFile.h"
#include "TString.h"
#include "TH2.h"
#include "TGraphErrors.h"
#include "TTree.h"
#include "TCanvas.h"

#include "wmStyle.C"
#include "ShippingData.h"

void Results_From_CSV_To_Root(){
  
  static const int nPMTs = 38;
  
  TFile *outFile = new TFile("Results.root","RECREATE");

  TStyle *wmStyle = GetwmStyle();
  wmStyle->SetOptFit(1);
  // gROOT->SetStyle("wmStyle");
  // gROOT->ForceStyle();
  
  TTree *T = new TTree("Results","Results from CSV file");
  Long64_t nlines;

  //PMT Number, Rig Location, Nominal Voltage [V],Temperature, Dark Rate [Hz] Short Run, Scaled HPK Dark Rate [Hz],
  //Mu,	P:V, HPK P:V, Gain [x10^7], Operating Voltage [V], ∆V, Dark Rate [Hz]  Long Run

  TString lineFormat;
  lineFormat = "PMT/I:Loc/I:HV_H/F:Temp/F:Dark_S/F:Dark_H/F:Mu/F:PTV/F:PTV_H/F:Gain/F:HV/F:Dark/F:Dark_E/F:Dark_N/F:Noise/F";

  nlines = T->ReadFile("Results.csv",lineFormat,',');

  int   PMT, Loc;

  float HV_H, Temp, Dark_S, Dark_H, Mu, PTV, PTV_H, Gain, HV, Dark,Dark_E,Dark_N,Noise;
  T->SetBranchAddress("PMT",&PMT);
  T->SetBranchAddress("Loc",&Loc);

  T->SetBranchAddress("HV_H",&HV_H);
  T->SetBranchAddress("Temp",&Temp);
  T->SetBranchAddress("Dark_S",&Dark_S);
  T->SetBranchAddress("Dark_H",&Dark_H);

  T->SetBranchAddress("Mu",&Mu);
  T->SetBranchAddress("PTV",&PTV);
  T->SetBranchAddress("PTV_H",&PTV_H);
  T->SetBranchAddress("Gain",&Gain);

  T->SetBranchAddress("HV",&HV);
  T->SetBranchAddress("Dark",&Dark);
  T->SetBranchAddress("Dark_E",&Dark_E);

  T->SetBranchAddress("Dark_N",&Dark_N);
  T->SetBranchAddress("Noise",&Noise);

  printf(" Results for %lld PMTs \n",nlines);

  Int_t cSize = 500;
  
  TCanvas * c1 =  new TCanvas("c1","c1",0,0,4*cSize,2*cSize);
  c1->Divide(5,2);

  c1->cd(1);
  T->Draw("Temp");
    
  c1->cd(2);
  T->Draw("Dark_S");

  c1->cd(3);
  T->Draw("Dark");

  c1->cd(4);
  T->Draw("Dark_H:Dark","","colz");

  c1->cd(5);
  T->Draw("PTV");

  c1->cd(6);
  T->Draw("Gain");
 
  TH1F   *h1 = new TH1F("h1","x distribution;X LABEL; Counts",100,0,5000);

  c1->cd(7);
  h1->Draw();
  h1->Write();
  
  TH1F *P2V = new TH1F("TW1","PTV comparison;PTV;PTV_H",60,1,4);

  double PTV_arr[nPMTs];
  double PTV_H_arr[nPMTs];

  for (int i = 0 ; i < T->GetEntries() ; i++){
    T->GetEntry(i);

    PTV_arr[i] = PTV;
    PTV_H_arr[i] = PTV_H;

    cout << " PTV = " << PTV << endl;

    P2V->Fill(PTV);
  }
  TCanvas *c5 = new TCanvas("P2V","PTV Comparison");

  c5->cd(1);
  P2V->Draw();
  P2V->Write();

  TGraph *g2 = new TGraph(nPMTs,PTV_H_arr,PTV_arr);

  g2->SetTitle("PTV comparison;x;y");
  g2->Fit("pol1");
  gStyle->SetOptFit();

  c1->cd(9);

  g2->SetMarkerStyle(20);
  g2->Draw("AP");

  T->Write();

  //ShippingData *ship_data = new ShippingData(0);

  double Dark_arr[nPMTs];
  double Dark_H_arr[nPMTs];
  double Dark_E_arr[nPMTs];
  double Dark_H_Max_arr[nPMTs];
  double Dark_H_Min_arr[nPMTs];
  double Dark_H_E_arr[nPMTs];
 
  double tempLow,tempHigh;
  
  for (int i = 0 ; i < T->GetEntries() ; i++){
    T->GetEntry(i);
    
    Dark_arr[i]   = Dark;
    Dark_H_arr[i] = Dark_H;
    Dark_E_arr[i] = Dark_E;
    //ship_data = new ShippingData(PMT,0);
    
    // if( (int)Dark_H != (int)ship_data->GetDR(Temp) ){
    // 	cout << " HPK Dark scaled ( function ) = "
    // 	     << ship_data->GetDR(Temp) << endl;
    // 	cout << " HPK Dark scaled ( CSV file  ) = "
    // 	     << Dark_H  << endl;
    //   }

    // tempLow  = Temp - 1;
    // tempHigh = Temp + 1;
    
    // Dark_H_Min_arr[i] = ship_data->GetDR(tempLow);
    // Dark_H_Max_arr[i] = ship_data->GetDR(tempHigh);

    Dark_H_E_arr[i] = (Dark_H_Max_arr[i]-Dark_H_Min_arr[i])/2.;

    cout << " Dark_H_Min_arr[i] = " << Dark_H_Min_arr[i] << endl;
    cout << " Dark_H_Max_arr[i] = " << Dark_H_Max_arr[i] << endl;
    cout << " Dark_H_arr[i]     = " << Dark_H_arr[i] << endl;
    cout << " Dark_H_E_arr[i]   = " << Dark_H_E_arr[i] << endl;
    
    h1->Fill(Dark);
    //delete ship_data;
    
  }

  TGraph *g1 = new TGraph(nPMTs,Dark_H_arr,Dark_arr);
  
  g1->SetTitle(";x;y");
  g1->Fit("pol1");
  gStyle->SetOptFit();
  c1->cd(8);
  
  g1->SetMarkerStyle(20);
  g1->Draw("AP");

  TCanvas * c2 = new TCanvas("c2");

  c2->cd(1);

  T->Draw("HV_H:HV","","colz");
  
  TCanvas *c4 = new TCanvas("c4","Dark rate comparison with errors");

  c4->SetGrid(); 
 
  auto *TW2 = new TGraphErrors(nPMTs,Dark_H_arr,Dark_arr,Dark_H_E_arr,Dark_E_arr);

  TW2->SetTitle("Dark Rate comparison with errors");
  TW2->GetXaxis()->SetTitle("Dark_H");
  TW2->GetYaxis()->SetTitle("Dark");
  TW2->Fit("pol1");
  gStyle->SetOptFit();
  
  TW2->SetMarkerStyle(10);
  TW2->Draw("AP");

  //TCanvas *c5 = new TCanvas("c5", "PTV comparison with PTV_H");

  //c5->SetGrid();

  //TGraph *TW3 = new TGraph(nPMTs,PTV_H_arr,PTV_arr);

  //TW3->SetTitle("PTV comparison with PTV_H");
  //TW3->GetXaxis()->SetTitle("PTV_H");
  //TW3->GetYaxis()->SetTitle("PTV");

  //TW3->SetMarkerStyle(20);
  //TW3->Draw("AP");

  //TW3->Write();

  
}
