#include "TFile.h"
#include "TString.h"
#include "TH2.h"
#include "TGraph.h"
#include "TTree.h"
#include "TCanvas.h"

void CSVToRoot(){
  
  TFile *outFile = new TFile("Dark.root","RECREATE");
  
  TTree *T = new TTree("Dark","Dark counts from CSV file");
  Long64_t nlines;


  //PMT Number, Rig Location, Nominal Voltage [V],Temperature, Dark Rate [Hz] Short Run, Scaled HPK Dark Rate [Hz],
  //Mu,	P:V, HPK P:V, Gain [x10^7], Operating Voltage [V], ∆V, Dark Rate [Hz]  Long Run
  TString lineFormat = "PMT/I:Loc/I:HV_N/F:Temp/F:Dark_S/F:Dark_H/F:Mu/F:PTV/F:PTV_H/F:Gain/F:HV/F:dV/F:Dark/F";

  nlines = T->ReadFile("Results.csv",lineFormat,',');

  int   PMT, Loc;
  float HV_N, Temp, Dark_S, Dark_H, Mu, PTV, PTV_H, Gain, HV, dV, Dark;
  T->SetBranchAddress("PMT",&PMT);
  T->SetBranchAddress("Loc",&Loc);

  T->SetBranchAddress("HV_N",&HV_N);
  T->SetBranchAddress("Temp",&Temp);
  T->SetBranchAddress("Dark_S",&Dark_S);
  T->SetBranchAddress("Dark_H",&Dark_H);

  T->SetBranchAddress("Mu",&Mu);
  T->SetBranchAddress("PTV",&PTV);
  T->SetBranchAddress("PTV_H",&PTV_H);
  T->SetBranchAddress("Gain",&Gain);

  T->SetBranchAddress("HV",&HV);
  T->SetBranchAddress("dV",&dV);
  T->SetBranchAddress("Dark",&Dark);
  
  printf(" Results for %lld PMTs \n",nlines);

  Int_t cSize = 500;
  
  TCanvas * c1 =  new TCanvas("c1","c1",0,0,4*cSize,2*cSize);
  c1->Divide(4,2);

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

  double Dark_arr[30];
  double Dark_H_arr[30];
  
  for (int i = 0 ; i < T->GetEntries() ; i++){
    T->GetEntry(i);

    Dark_arr[i] = Dark;
    Dark_H_arr[i] = Dark_H;
    
    cout << " Dark = " << Dark << endl;
    
    h1->Fill(Dark);
  }

  c1->cd(7);
  h1->Draw();
  h1->Write();
  
  TGraph *g1 = new TGraph(30,Dark_H_arr,Dark_arr);

  g1->SetTitle(";x;y");
  
  c1->cd(8);

  g1->SetMarkerStyle(20);
  g1->Draw("AP");

  T->Write();

}
