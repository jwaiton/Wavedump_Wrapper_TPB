#include "TFile.h"
#include "TString.h"
#include "TH2.h"
#include "TGraph.h"
#include "TTree.h"
#include "TCanvas.h"

void CSVToRoot(){
  
  TFile *outFile = new TFile("Results.root","RECREATE");
  
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

  double Dark_arr[38];
  double Dark_H_arr[38];
  
  for (int i = 0 ; i < T->GetEntries() ; i++){
    T->GetEntry(i);

    Dark_arr[i] = Dark;
    Dark_H_arr[i] = Dark_H;
    
    cout << " Dark   = " << Dark   << endl;
    cout << " Dark_E = " << Dark_E << endl;
    cout << " Dark_N = " << Dark_N << endl;
    cout << " Noise  = " << Noise  << endl;
    
    h1->Fill(Dark);
  }

  c1->cd(7);
  h1->Draw();
  h1->Write();
  
  TGraph *g1 = new TGraph(38,Dark_H_arr,Dark_arr);

  g1->SetTitle(";x;y");
  g1->Fit("pol1");
  gStyle->SetOptFit();
  c1->cd(8);

  g1->SetMarkerStyle(20);
  g1->Draw("AP");

  T->Write();

  TH1F *W1 = new TH1F("TW1","PTV comparison;PTV;PTV_H",30,1,4);

  double PTV_arr[38];
  double PTV_H_arr[38];

  for (int i = 0 ; i < T->GetEntries() ; i++){
    T->GetEntry(i);

    PTV_arr[i] = PTV;
    PTV_H_arr[i] = PTV_H;

    cout << " PTV = " << PTV << endl;

    W1->Fill(PTV);
  }

  c1->cd(9);
  W1->Draw();
  W1->Write();

  TGraph *g2 = new TGraph(38,PTV_H_arr,PTV_arr);

  g2->SetTitle("PTV comparison;x;y");
  g2->Fit("pol1");
  gStyle->SetOptFit();
  c1->cd(10);

  g2->SetMarkerStyle(20);
  g2->Draw("AP");

  T->Write();

  TCanvas * c2 = new TCanvas("c2");

  c2->cd(1);

  T->Draw("HV_H:HV","","colz");
  
  TCanvas * c3 = new TCanvas("c3");

  c3->cd(1);

  T->Draw("Dark:Dark_H");
  
}
