#include "TFile.h"
#include "TString.h"
#include "TH2.h"
#include "TGraph.h"
#include "TTree.h"
#include "TCanvas.h"

#include "wmStyle.C"
#include "ShippingData.h"
#include "dark_rate_at_temp.h"

void CSVToRoot(){
  
  TFile *outFile = new TFile("Results.root","RECREATE");

  TStyle *wmStyle = GetwmStyle();
  wmStyle->SetOptFit(1);
  gROOT->SetStyle("wmStyle");
  gROOT->ForceStyle();
  
  TTree *Results = new TTree("Results","Results from CSV file");
  Long64_t nlines;

  //PMT Number, Rig Location, Nominal Voltage [V],Temperature, Dark Rate [Hz] Short Run, Scaled HPK Dark Rate [Hz],
  //Mu,	P:V, HPK P:V, Gain [x10^7], Operating Voltage [V], ∆V, Dark Rate [Hz]  Long Run
  TString lineFormat = "PMT/I:Loc/I:HV_H/F:Temp/F:Dark_S/F:Dark_H/F:Mu/F:PTV/F:PTV_H/F:Gain/F:HV/F:dV/F:Dark/F";

  nlines = Results->ReadFile("Results.csv",lineFormat,',');

  int   PMT, Loc;
  float HV_H, Temp, Dark_S, Dark_H, Mu, PTV, PTV_H, Gain, HV, dV, Dark;
  Results->SetBranchAddress("PMT",&PMT);
  Results->SetBranchAddress("Loc",&Loc);

  Results->SetBranchAddress("HV_H",&HV_H);
  Results->SetBranchAddress("Temp",&Temp);
  Results->SetBranchAddress("Dark_S",&Dark_S);
  Results->SetBranchAddress("Dark_H",&Dark_H);

  Results->SetBranchAddress("Mu",&Mu);
  Results->SetBranchAddress("PTV",&PTV);
  Results->SetBranchAddress("PTV_H",&PTV_H);
  Results->SetBranchAddress("Gain",&Gain);

  Results->SetBranchAddress("HV",&HV);
  Results->SetBranchAddress("dV",&dV);
  Results->SetBranchAddress("Dark",&Dark);
  
  printf(" Results for %lld PMTs \n",nlines);

  Int_t cSize = 500;
  
  TCanvas * c1 =  new TCanvas("c1","c1",0,0,4*cSize,2*cSize);
  c1->Divide(5,2);

  c1->cd(1);
  Results->Draw("Temp");
    
  c1->cd(2);
  Results->Draw("Dark_S");

  c1->cd(3);
  Results->Draw("Dark");

  c1->cd(4);
  Results->Draw("Dark_H:Dark","","colz");

  c1->cd(5);
  Results->Draw("PTV");

  c1->cd(6);
  Results->Draw("Gain");
 
  TH1F   *h1 = new TH1F("h1","x distribution;X LABEL; Counts",100,0,5000);

  double Dark_arr[30];
  double Dark_H_arr[30];


  //Results->cd();
  
  c1->cd(7);
  h1->Draw();
  h1->Write();
  
  TGraph *g1 = new TGraph(30,Dark_H_arr,Dark_arr);

  g1->SetTitle(";x;y");
  g1->Fit("pol1");
  gStyle->SetOptFit();
  c1->cd(8);

  g1->SetMarkerStyle(20);
  g1->Draw("AP");

  TH1F *W1 = new TH1F("TW1","PTV comparison;PTV;PTV_H",30,1,4);

  double PTV_arr[30];
  double PTV_H_arr[30];

  for (int i = 0 ; i < Results->GetEntries() ; i++){
    Results->GetEntry(i);

    PTV_arr[i] = PTV;
    PTV_H_arr[i] = PTV_H;

    cout << " PTV = " << PTV << endl;

    W1->Fill(PTV);
  }

  c1->cd(9);
  W1->Draw();
  W1->Write();

  TGraph *g2 = new TGraph(30,PTV_H_arr,PTV_arr);

  g2->SetTitle("PTV2;PTV_H;PTV");
  g2->Fit("pol1");
  gStyle->SetOptFit();
  c1->cd(10);

  g2->SetMarkerStyle(20);
  g2->Draw("AP");

  Results->Write();

    ShippingData *ship_data = new ShippingData(0);
  
  for (int i = 0 ; i < Results->GetEntries() ; i++){
    Results->GetEntry(i);
    
    Dark_arr[i] = Dark;
    Dark_H_arr[i] = Dark_H;

    ship_data = new ShippingData(PMT,0);
    
    if( (int)Dark_H != (int)ship_data->GetDR(Temp) ){
	cout << " HPK Dark scaled ( function ) = "
	     << ship_data->GetDR(Temp) << endl;
	cout << " HPK Dark scaled ( CSV file  ) = "
	     << Dark_H  << endl;
      }
      
    h1->Fill(Dark);
    delete ship_data;
    
  }
}
