#include "TGraph.h"

void PTV(){

  bool drawShip   = true;
  bool drawBoul   = true;
  bool drawRigA   = true;
  bool drawRigB1A = true;
  bool drawRigB1B = false;
  
  double pmt[]      = {15,16,82,107,131,148,166,171};
  double pmt_boul[] = {82,107,131,148,166,171};

  double ptv_ship[] = {4.43,4.43,4.17,4.25,4.36,4.78,4.30,4.18};
  double ptv_boul_mean[] = {2.25,3.57,2.71,2.62,2.94,3.69};

  double ptv_boul_1[] = {2.3,3.9,2.7,2.7,2.7,3.8};

  TLegend *leg = new TLegend(0.7,0.8,0.95,0.95);
  
  
  TGraph * g_ship = new TGraph(8,pmt,ptv_ship);  
  g_ship->SetMinimum(1.5);
  g_ship->SetMaximum(6.5);
  g_ship->SetTitle("");
  g_ship->GetXaxis()->SetTitle("PMT ID");
  g_ship->GetYaxis()->SetTitle("Peak to valley ratio");
  g_ship->SetMarkerStyle(21);

  if(drawShip){
    g_ship->Draw("P L A");
    leg->AddEntry(g_ship,"Ship data","PL");
  }
  
  
  
  TGraph * g_boul = new TGraph(6,pmt_boul,ptv_boul_1);  
  g_boul->SetMarkerStyle(21);
  g_boul->SetMarkerColor(kGreen+2);
  g_boul->SetLineColor(kGreen+2);

  if(drawBoul){
    g_boul->Draw("P L same");
    leg->AddEntry(g_boul,"Boulby run 1","PL");
  }


  double pmt_config1[] = {131,107,166,171};
  double pmt_config2[] = {15,16,82,148};
  
  double ptv_riga_height1_config1[] = {6.28,4.01,3.81,3.48};
  double ptv_riga_height1_config2[] = {3.32,5.82,3.97,5.6};

  //  double pmt[]          = {15,  16,  82,  107, 131, 148, 166, 171};
  double ptv_riga_height1[] = {3.32,5.82,3.97,5.64,6.28,4.01,3.81,3.48};

  //  double pmt[]          = {15,  16,  82,  107, 131, 148, 166, 171};
  double ptv_rigb_run1[]    = {3.50,2.76,1.82,2.45,3.69,4.46,2.03,3.89};
  
  //TGraph * g_riga_height1 = new TGraph(8,pmt,ptv_riga_height1);
  TGraph * g_riga_height1 = new TGraph(4,pmt_config2,ptv_riga_height1_config2);
  
  g_riga_height1->SetMarkerStyle(23);
  g_riga_height1->SetMarkerColor(kBlue);
  g_riga_height1->SetLineColor(kBlue);

  if(drawRigA){
    g_riga_height1->Draw("P L same");
    leg->AddEntry(g_riga_height1,"Rig A LEDs Low","PL");
  }

  double pmt_rigb_run1A[]    = {15,16,82,148};
  double pmt_rigb_run1B[]    = {107,131,166,171};

  double ptv_rigb_run1A[]    = {3.50,2.76,1.82,4.46};
  double ptv_rigb_run1B[]    = {2.45,3.69,2.03,3.89};

  /* TGraph * g_rigb_run1 = new TGraph(8,pmt,ptv_rigb_run1);   */
  /* g_rigb_run1->SetMarkerStyle(25); */
  /* g_rigb_run1->SetMarkerColor(kRed); */
  /* g_rigb_run1->SetLineColor(kRed); */

  /* if(drawRigB){ */
  /*   g_rigb_run1->Draw("P L same"); */
  /*   leg->AddEntry(g_rigb_run1,"Rig B Last week","PL"); */
  /* } */
  
  TGraph * g_rigb_run1A = new TGraph(4,pmt_rigb_run1A,ptv_rigb_run1A);  
  g_rigb_run1A->SetMarkerStyle(25);
  g_rigb_run1A->SetMarkerColor(kRed);
  g_rigb_run1A->SetLineColor(kRed);

  if(drawRigB1A){
    g_rigb_run1A->Draw("P L same");
    leg->AddEntry(g_rigb_run1A,"Rig B Run 1A","PL");
  }
  

  double ptv_riga_height2[] = {2.79,4.01,2.51,2.59};
  double pmt_a2[]           = {15,16,82,148};

  double pmt_b2[]        = {107,131,166,171};
  double ptv_rigb_run2[] = {2.47,3.25,2.05,3.88};

  TGraph * g_riga_height2 = new TGraph(4,pmt_a2,ptv_riga_height2);  
  g_riga_height2->SetMarkerStyle(26);
  g_riga_height2->SetMarkerColor(kRed+2);
  g_riga_height2->SetLineStyle(2);
  g_riga_height2->SetLineWidth(2);
  //g_riga_height2->SetLineColor(kBlue+2);
  g_riga_height2->SetLineColor(kRed+2);

  if(drawRigA){
    g_riga_height2->Draw("P L same");
    leg->AddEntry(g_riga_height2,"Rig A LEDs High","PL");
  }
  
  TGraph * g_rigb_run2 = new TGraph(4,pmt_b2,ptv_rigb_run2);  
  g_rigb_run2->SetMarkerStyle(24);
  g_rigb_run2->SetMarkerColor(kRed);
  g_rigb_run2->SetLineStyle(2);
  g_rigb_run2->SetLineColor(kRed+2);

  if(drawRigB1B){
    g_rigb_run2->Draw("P L same");
    leg->AddEntry(g_rigb_run2,"Rig B This week","PL");
  }

  leg->Draw();

}

