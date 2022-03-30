#include "TGraph.h"
#include "wmStyle.C"

TStyle *wmStyle = GetwmStyle();

// options:
//    1  PTV
//    2  Mu
//    3  Gain
void PTV(int option = 1){
  
  gROOT->SetStyle("wmStyle");
  gROOT->ForceStyle();
  
  static const int nLocs = 4;

  double PTV_HPK_PMT_107[1] = {4.25};
  double PTV_HPK_PMT_131[1] = {4.36};
  double PTV_HPK_PMT_166[1] = {4.30};
  double PTV_HPK_PMT_171[1] = {4.18};

  double Loc_HPK[1] = {5.5};
  double Loc[nLocs] = {4.,5.,6.,7.};
  
  // 107
  double Mu_PMT_107[nLocs]   = {0.14,0.12,0.13,0.15};
  double PTV_PMT_107[nLocs]  = {3.30,2.37,2.34,3.42};
  double Gain_PMT_107[nLocs] = {0.89,0.81,0.84,0.91};

  // 131
  double Mu_PMT_131[nLocs]   = {0.15,0.13,0.14,0.16};
  double PTV_PMT_131[nLocs]  = {3.65,2.36,2.26,3.72};
  double Gain_PMT_131[nLocs] = {0.87,0.84,0.86,0.92};

  // 166
  double Mu_PMT_166[nLocs]   = {0.14,0.12,0.12,0.15};
  double PTV_PMT_166[nLocs]  = {3.59,2.36,2.26,3.68};
  double Gain_PMT_166[nLocs] = {0.88,0.81,0.82,0.89};

  // 171
  double Mu_PMT_171[nLocs]   = {0.15,0.13,0.14,0.16};
  double PTV_PMT_171[nLocs]  = {3.24,2.08,1.92,3.17};
  double Gain_PMT_171[nLocs] = {0.98,0.92,0.92,0.95};
  
  TGraph * G_PTV_HPK_107 = new TGraph(1,Loc_HPK,PTV_HPK_PMT_107);
  TGraph * G_PTV_HPK_131 = new TGraph(1,Loc_HPK,PTV_HPK_PMT_131);
  TGraph * G_PTV_HPK_166 = new TGraph(1,Loc_HPK,PTV_HPK_PMT_166);
  TGraph * G_PTV_HPK_171 = new TGraph(1,Loc_HPK,PTV_HPK_PMT_171);
  
  TGraph * G_PTV_PMT_107 = new TGraph(nLocs,Loc,PTV_PMT_107);
  TGraph * G_PTV_PMT_131 = new TGraph(nLocs,Loc,PTV_PMT_131);
  TGraph * G_PTV_PMT_166 = new TGraph(nLocs,Loc,PTV_PMT_166);
  TGraph * G_PTV_PMT_171 = new TGraph(nLocs,Loc,PTV_PMT_171);

  TGraph * G_Mu_PMT_107 = new TGraph(nLocs,Loc,Mu_PMT_107);
  TGraph * G_Mu_PMT_131 = new TGraph(nLocs,Loc,Mu_PMT_131);
  TGraph * G_Mu_PMT_166 = new TGraph(nLocs,Loc,Mu_PMT_166);
  TGraph * G_Mu_PMT_171 = new TGraph(nLocs,Loc,Mu_PMT_171);

  TGraph * G_Gain_PMT_107 = new TGraph(nLocs,Loc,Gain_PMT_107);
  TGraph * G_Gain_PMT_131 = new TGraph(nLocs,Loc,Gain_PMT_131);
  TGraph * G_Gain_PMT_166 = new TGraph(nLocs,Loc,Gain_PMT_166);
  TGraph * G_Gain_PMT_171 = new TGraph(nLocs,Loc,Gain_PMT_171);

  G_PTV_PMT_107->SetTitle(";Rig location;P:V");
  G_Mu_PMT_107->SetTitle(";Rig location;Mu");
  G_Gain_PMT_107->SetTitle(";Rig location;Gain /10^{7}");
  
  G_PTV_PMT_107->SetMarkerColor(kRed);
  G_Mu_PMT_107->SetMarkerColor(kRed);
  G_Gain_PMT_107->SetMarkerColor(kRed);
  G_PTV_HPK_107->SetMarkerColor(kRed);
  
  G_PTV_PMT_131->SetMarkerColor(kOrange);
  G_Mu_PMT_131->SetMarkerColor(kOrange);
  G_Gain_PMT_131->SetMarkerColor(kOrange);
  G_PTV_HPK_131->SetMarkerColor(kOrange);
  
  G_PTV_PMT_166->SetMarkerColor(kGreen);
  G_Mu_PMT_166->SetMarkerColor(kGreen);
  G_Gain_PMT_166->SetMarkerColor(kGreen);
  G_PTV_HPK_166->SetMarkerColor(kGreen);
  
  G_PTV_PMT_171->SetMarkerColor(kBlue);
  G_Mu_PMT_171->SetMarkerColor(kBlue);
  G_Gain_PMT_171->SetMarkerColor(kBlue);
  G_PTV_HPK_171->SetMarkerColor(kBlue);

  G_PTV_PMT_107->SetMarkerStyle(kFullCircle);
  G_Mu_PMT_107->SetMarkerStyle(kFullCircle);
  G_Gain_PMT_107->SetMarkerStyle(kFullCircle);
  G_PTV_HPK_107->SetMarkerStyle(kFullSquare);

  G_PTV_PMT_131->SetMarkerStyle(kFullCircle);
  G_Mu_PMT_131->SetMarkerStyle(kFullCircle);
  G_Gain_PMT_131->SetMarkerStyle(kFullCircle);
  G_PTV_HPK_131->SetMarkerStyle(kFullSquare);

  G_PTV_PMT_166->SetMarkerStyle(kFullCircle);
  G_Mu_PMT_166->SetMarkerStyle(kFullCircle);
  G_Gain_PMT_166->SetMarkerStyle(kFullCircle);
  G_PTV_HPK_166->SetMarkerStyle(kFullSquare);

  G_PTV_PMT_171->SetMarkerStyle(kFullCircle);
  G_Mu_PMT_171->SetMarkerStyle(kFullCircle);
  G_Gain_PMT_171->SetMarkerStyle(kFullCircle);
  G_PTV_HPK_171->SetMarkerStyle(kFullSquare);
  
  G_PTV_PMT_107->SetMaximum(4.0);
  G_PTV_PMT_107->SetMinimum(1.5);

  G_Mu_PMT_107->SetMaximum(0.17);
  G_Mu_PMT_107->SetMinimum(0.11);

  G_Gain_PMT_107->SetMaximum(1.0);
  G_Gain_PMT_107->SetMinimum(0.8);
  
  
  if(option == 1){
    G_PTV_PMT_107->Draw("AP");
    G_PTV_PMT_131->Draw("P same");
    G_PTV_PMT_166->Draw("P same");
    G_PTV_PMT_171->Draw("P same");
    
    /* G_PTV_HPK_107->Draw("P same"); */
    /* G_PTV_HPK_131->Draw("P same"); */
    /* G_PTV_HPK_166->Draw("P same"); */
    /* G_PTV_HPK_171->Draw("P same"); */
  }
  else if(option==2){
    G_Mu_PMT_107->Draw("AP");
    G_Mu_PMT_131->Draw("P same");
    G_Mu_PMT_166->Draw("P same");
    G_Mu_PMT_171->Draw("P same");
    
  }
  else if(option==3){
    G_Gain_PMT_107->Draw("AP");
    G_Gain_PMT_131->Draw("P same");
    G_Gain_PMT_166->Draw("P same");
    G_Gain_PMT_171->Draw("P same");
    
  }
}
