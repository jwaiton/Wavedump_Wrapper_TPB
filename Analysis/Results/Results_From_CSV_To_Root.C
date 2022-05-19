#include "TSystem.h"

#include "TFile.h"
#include "TString.h"
#include "TH2.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TTree.h"
#include "TCanvas.h"

#include "wmStyle.C"
#include "ShippingData.h"
#include "ShippingData.C"

void Results_From_CSV_To_Root(){
  
  static const int nPMTs = 85; // 87 total - 2 with invalid data
  // static const int nPMTs = 38;
  
  
  TStyle *wmStyle = GetwmStyle();
  wmStyle->SetOptFit(1);
  gROOT->SetStyle("wmStyle");
  gROOT->ForceStyle();
  
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

  Int_t cSize = 400;

  bool quickPlot = kTRUE;

  // plot data from the tree directly
  if(quickPlot){
    
    TCanvas * c1 =  new TCanvas("c1","c1",0,0,3*cSize,2*cSize);
    c1->Divide(3,2);
    
    c1->cd(1);
    T->Draw("Temp");
    
    c1->cd(2);
    T->Draw("Dark_S");
    
    T->Draw("Dark_H","","SAME");
    ((TH1F*)(gPad->GetListOfPrimitives()->At(1)))->SetLineColor(4);
    
    c1->cd(3);
    T->Draw("Dark");
    T->Draw("Dark_H","","SAME");
    ((TH1F*)(gPad->GetListOfPrimitives()->At(1)))->SetLineColor(4);
    
    c1->cd(4);
    
    T->Draw("PTV_H","","");
    ((TH1F*)(gPad->GetListOfPrimitives()->At(0)))->SetLineColor(4);
    
    T->Draw("PTV","","SAME");
    
    c1->cd(5);
    T->Draw("HV");
    T->Draw("HV_H","","SAME");
    ((TH1F*)(gPad->GetListOfPrimitives()->At(1)))->SetLineColor(4);
    
    c1->cd(6);
    T->Draw("Gain");
  }

  //----------
  // Dark Counts

  TH1F   *hDark = new TH1F("hDark",";Dark Rate (Hz);Counts",20,0,7500);
  TH1F   *hDarkH = new TH1F("hDarkH",";Dark Rate (Hz);Counts",20,0,7500);

  TGraph   *gDark;
  
  //----------
  // Gain

  TH1F   *hHV = new TH1F("hHV",";HV (V);Counts",20,1150,2200);
  TH1F   *hHVH = new TH1F("hHVH",";HV (V);Counts",20,1150,2200);
  
  //----------
  // Peak to Valley
  
  TH1F   *hPTV = new TH1F("hPTV",";PTV (V);Counts",20,2.0,4.5);
  TH1F   *hPTVH = new TH1F("hPTVH",";PTV (V);Counts",20,1.5,5.5);

  TH1F *P2V = new TH1F("TW1","PTV comparison;PTV;PTV_H",30,1.6,3);

  double PTV_arr[nPMTs];
  double PTV_H_arr[nPMTs];

  TGraph *g2 = new TGraph(nPMTs,PTV_H_arr,PTV_arr);

  g2->SetTitle("PTV comparison;x;y");
  g2->Fit("pol1");
  gStyle->SetOptFit();

  //c1->cd(8);

  g2->SetMarkerStyle(7);
  //g2->Draw("AP");

  //g2->Write();

  ShippingData *ship_data = new ShippingData(0);

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
    
    //float dark_rate = gSystem->Exec("shipping_data -d 1");
    //gSystem->Exec("ls");
    //cout << " dark_rate = " << dark_rate << endl;

    ship_data = new ShippingData(PMT,0);
    
    if( abs((int)Dark_H-(int)ship_data->GetDR(Temp)) > 100 ){
      cout << " PMT " << PMT << endl;
      cout << " HPK Dark rate, function  "
	   << ship_data->GetDR(Temp) << endl;
      cout << " HPK Dark rate, file  ) = "
	   << Dark_H  << endl;
    }
    tempLow  = Temp - 1;
    tempHigh = Temp + 1;
    
    Dark_H_Min_arr[i] = ship_data->GetDR(tempLow);
    Dark_H_Max_arr[i] = ship_data->GetDR(tempHigh);

    Dark_H_E_arr[i] = (Dark_H_Max_arr[i]-Dark_H_Min_arr[i])/2.;

    /* cout << " Dark_H_Min_arr[i] = " << Dark_H_Min_arr[i] << endl; */
    /* cout << " Dark_H_Max_arr[i] = " << Dark_H_Max_arr[i] << endl; */
    /* cout << " Dark_H_arr[i]     = " << Dark_H_arr[i] << endl; */
    /* cout << " Dark_H_E_arr[i]   = " << Dark_H_E_arr[i] << endl; */
    
    hDark->Fill(Dark);
    hDarkH->Fill(Dark_H);

    hHV->Fill(HV);
    hHVH->Fill(HV_H);

    hPTV->Fill(PTV);
    hPTVH->Fill(PTV_H);

    delete ship_data;

  }// end of : for (int i = 0 ; i

  TFile *outFile = new TFile("Results.root","RECREATE");

  TLatex * latex = new TLatex(); 
  latex->SetNDC();
  latex->SetTextSize(0.04);;
  
  //------
  // Dark Counts

  TCanvas * cDark =  new TCanvas("cDark","cDark",0,0,cSize,cSize);
  hDark->SetLineColor(kBlue);
  hDark->SetFillColor(kBlue);
  hDark->SetFillStyle(3);
  hDark->Draw();

  hDarkH->SetLineColor(kRed);
  hDarkH->SetFillColor(kRed);
  hDarkH->SetFillStyle(3);
  hDarkH->Draw("SAME");
  
  char textDarkEntries[64];
  char textDarkMean[64];
  char textDarkRMS[64];
  char textDarkHMean[64];
  char textDarkHRMS[64];

  sprintf(textDarkEntries,"Entries \t\t %.0f ",
	  roundf(hDark->GetEntries()));

  sprintf(textDarkMean,
	  "#color[4]{EDI Mean \t %.0f}",
	  hDark->GetMean());
  sprintf(textDarkRMS,
	  "#color[4]{EDI RMS \t %.0f}",
	  hDark->GetRMS());
  
  sprintf(textDarkHMean,
	  "#color[2]{HPK Mean \t %.0f}",
	  hDarkH->GetMean());
  sprintf(textDarkHRMS,
	  "#color[2]{HPK RMS \t %.0f}",
	  hDarkH->GetRMS());

  latex->DrawLatex(0.6,0.85,textDarkEntries);
  latex->DrawLatex(0.6,0.80,textDarkMean);
  latex->DrawLatex(0.6,0.75,textDarkHMean);
  latex->DrawLatex(0.6,0.70,textDarkRMS);
  latex->DrawLatex(0.6,0.65,textDarkHRMS);

  hDark->Write();
  hDarkH->Write();
  
  //----------
  // HV
  TCanvas * cHV =  new TCanvas("cHV","cHV",cSize,0,cSize,cSize);

  hHV->SetLineColor(kBlue);
  hHV->SetFillColor(kBlue);
  hHV->SetFillStyle(3);

  hHVH->SetLineColor(kRed);
  hHVH->SetFillColor(kRed);
  hHVH->SetFillStyle(3);
  hHVH->Draw("SAME");
  hHV->Draw("SAME");
  
  char textHVEntries[64];
  char textHVMean[64];
  char textHVRMS[64];
  char textHVHMean[64];
  char textHVHRMS[64];

  sprintf(textHVEntries,"Entries \t\t %.0f ",
	  roundf(hHV->GetEntries()));

  sprintf(textHVMean,
	  "#color[4]{EDI Mean \t %.0f}",
	  hHV->GetMean());
  sprintf(textHVRMS,
	  "#color[4]{EDI RMS \t %.0f}",
	  hHV->GetRMS());
  
  sprintf(textHVHMean,
	  "#color[2]{HPK Mean \t %.0f}",
	  hHVH->GetMean());
  sprintf(textHVHRMS,
	  "#color[2]{HPK RMS \t %.0f}",
	  hHVH->GetRMS());

  latex->DrawLatex(0.6,0.85,textHVEntries);
  latex->DrawLatex(0.6,0.80,textHVMean);
  latex->DrawLatex(0.6,0.75,textHVHMean);
  latex->DrawLatex(0.6,0.70,textHVRMS);
  latex->DrawLatex(0.6,0.65,textHVHRMS);

  hHVH->Write();
  hHV->Write();

  //----------
  // Peak to Valley

  TCanvas * cPTV =  new TCanvas("cPTV","cPTV",2*cSize,0,cSize,cSize);
  hPTV->SetLineColor(kBlue);
  hPTV->SetFillColor(kBlue);
  hPTV->SetFillStyle(3);

  hPTVH->SetLineColor(kRed);
  hPTVH->SetFillColor(kRed);
  hPTVH->SetFillStyle(3);
  hPTVH->Draw("SAME");

  hPTV->Draw("SAME");

  char textPTVEntries[64];
  char textPTVMean[64];
  char textPTVRMS[64];
  char textPTVHMean[64];
  char textPTVHRMS[64];

  sprintf(textPTVEntries,"Entries \t\t %.0f ",
	  roundf(hPTV->GetEntries()));

  sprintf(textPTVMean,
	  "#color[4]{EDI Mean \t %.1f}",
	  hPTV->GetMean());
  sprintf(textPTVRMS,
	  "#color[4]{EDI RMS \t %.1f}",
	  hPTV->GetRMS());
  
  sprintf(textPTVHMean,
	  "#color[2]{HPK Mean \t %.1f}",
	  hPTVH->GetMean());
  sprintf(textPTVHRMS,
	  "#color[2]{HPK RMS \t %.1f}",
	  hPTVH->GetRMS());

  latex->DrawLatex(0.6,0.85,textPTVEntries);
  latex->DrawLatex(0.6,0.80,textPTVMean);
  latex->DrawLatex(0.6,0.75,textPTVHMean);
  latex->DrawLatex(0.6,0.70,textPTVRMS);
  latex->DrawLatex(0.6,0.65,textPTVHRMS);


  
  hPTV->Write();
  hPTVH->Write();  
  //----


  /* TCanvas *c5 = new TCanvas("P2V","PTV Comparison"); */

  /* c5->cd(1); */
  /* P2V->Draw(); */
  /* P2V->Write(); */
  
  TGraph *g1 = new TGraph(nPMTs,Dark_H_arr,Dark_arr);
  
  g1->SetTitle("Dark Rate comparison;x;y");
  
  g1->Fit("pol1");
  gStyle->SetOptFit();
  //c1->cd(9);
  
  g1->SetMarkerStyle(7);
  //g1->Draw("AP");

  /* TCanvas * c2 = new TCanvas("c2"); */

  /* c2->cd(1); */

  /* T->Draw("HV_H:HV","","colz"); */
  
  /* TCanvas *c4 = new TCanvas("c4","Dark rate comparison with errors"); */

  /* c4->SetGrid();  */
 
  /* auto *TW2 = new TGraphErrors(nPMTs,Dark_H_arr,Dark_arr,Dark_H_E_arr,Dark_E_arr); */

  /* TF1 * fDark = new TF1("fDark","pol1(0)",0,5000); */
  
  /* fDark->SetParameter(0,0); */
  /* fDark->SetParameter(0,1.); */
  /* // fDark->FixParameter(0,0); */
  
  /* TW2->SetTitle("Dark Rate comparison with errors"); */
  /* TW2->GetXaxis()->SetTitle("Dark_H"); */
  /* TW2->GetYaxis()->SetTitle("Dark"); */
  /* TW2->Fit("fDark"); */
  /* gStyle->SetOptFit(); */
  
  /* TW2->SetMarkerStyle(7); */
  /* TW2->Draw("AP"); */

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
