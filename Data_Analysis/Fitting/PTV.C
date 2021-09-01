#include "TGraph.h"

void PTV(){

  double pmt[]      = {15,16,82,107,131,148,166,171};
  double pmt_boul[] = {82,107,131,148,166,171};

  double ptv_ship[] = {4.43,4.43,4.17,4.25,4.36,4.78,4.30,4.18};
  double ptv_boul_mean[] = {2.25,3.57,2.71,2.62,2.94,3.69};

  double ptv_boul_1[] = {2.3,3.9,2.7,2.7,2.7,3.8};

  TGraph * g_ship = new TGraph(8,pmt,ptv_ship);  
  g_ship->SetMinimum(1.5);
  g_ship->SetMaximum(6.5);
  g_ship->SetTitle("");
  g_ship->GetXaxis()->SetTitle("PMT ID");
  g_ship->GetYaxis()->SetTitle("Peak to valley ratio");
  g_ship->SetMarkerStyle(21);
  g_ship->Draw("P L A");

  TGraph * g_boul = new TGraph(6,pmt_boul,ptv_boul_1);  
  g_boul->SetMarkerStyle(21);
  g_boul->SetMarkerColor(kGreen+2);
  g_boul->SetLineColor(kGreen+2);
  g_boul->Draw("P L same");

  double ptv_riga_1[] = {3.32,5.82,3.97,5.64,6.28,4.01,3.81,3.48};
  
  double ptv_rigb_1[] = {3.50,2.76,1.82,2.45,3.69,4.46,2.03,3.89};
  
  TGraph * g_riga_1 = new TGraph(8,pmt,ptv_riga_1);  
  g_riga_1->SetMarkerStyle(23);
  g_riga_1->SetMarkerColor(kBlue);
  g_riga_1->SetLineColor(kBlue);
  g_riga_1->Draw("P L same");

  TGraph * g_rigb_1 = new TGraph(8,pmt,ptv_rigb_1);  
  g_rigb_1->SetMarkerStyle(25);
  g_rigb_1->SetMarkerColor(kRed);
  g_rigb_1->SetLineColor(kRed);
  g_rigb_1->Draw("P L same");


  double ptv_riga_2[] = {2.79,4.01,2.51,2.59};
  double pmt_a2[]     = {15,16,82,148};

  double pmt_b2[]     = {107,131,166,171};
  double ptv_rigb_2[] = {2.47,3.25,2.05,3.88};

  TGraph * g_riga_2 = new TGraph(4,pmt_a2,ptv_riga_2);  
  g_riga_2->SetMarkerStyle(26);
  g_riga_2->SetMarkerColor(kBlue);
  g_riga_2->SetLineStyle(2);
  g_riga_2->SetLineColor(kBlue+2);
  g_riga_2->Draw("P L same");

  TGraph * g_rigb_2 = new TGraph(4,pmt_b2,ptv_rigb_2);  
  g_rigb_2->SetMarkerStyle(24);
  g_rigb_2->SetMarkerColor(kRed);
  g_rigb_2->SetLineStyle(2);
  g_rigb_2->SetLineColor(kRed+2);
  g_rigb_2->Draw("P L same");


  TLegend *leg = new TLegend(0.85,0.2,0.95,0.8);
  leg->AddEntry(g_ship,"Ship data","PL");
  leg->AddEntry(g_boul,"Boulby run 1","PL");
  leg->AddEntry(g_riga_1,"Rig A LEDs Low","PL");
  leg->AddEntry(g_rigb_1,"Rig B Last week","PL");
  leg->AddEntry(g_riga_2,"Rig A LEDs High","PL");
  leg->AddEntry(g_rigb_2,"Rig B This week","PL");
  leg->Draw();
}

