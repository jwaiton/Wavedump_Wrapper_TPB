
float HPK_dark_at_temp(float N25, float temp){
 
  float HPK_temp = 25.;

  HPK_temp += 273.15;
  temp += 273.15;
  
  float k_boltz  = 8.62E-5;
  float alpha    = 1.32;   // average work function from data
    
  float deltaT = HPK_temp - temp;
  
  float deltaN = deltaT*N25/temp*(2 + alpha/(k_boltz*temp));

  std::cout << " deltaN = " << deltaN << std::endl;
  
  float N_temp = N25 - deltaN;
    
  return N_temp;
}

void dark(){

  int nPMTs = 8;
  double pmts[8]           = {15,  16,  82,  107, 131, 148, 166,171};

  double dark_rates_HPK[8] = {2200,2000,2700,3900,2400,2100,2300,2500};
  double dark_rates_v1[8]  = {2098,1387,1906,2752,1885,1162,1299,1435};
  
  TH1D * hDarkH = new TH1D("hDarkH","hDarkH",100,0,200);
  TH1D * hDark1 = new TH1D("hDark1","hDark1",100,0,200);

  double maxi = 0;
  
  for (int i = 0 ; i < nPMTs ; i++){

    dark_rates_HPK[i] = HPK_dark_at_temp(dark_rates_HPK[i],23.5);
    
    hDarkH->Fill(pmts[i],dark_rates_HPK[i]);
    hDark1->Fill(pmts[i],dark_rates_v1[i]);

    maxi = std::max(maxi,dark_rates_HPK[i]);
    maxi = std::max(maxi,dark_rates_v1[i]);
    
  }

  std::cout << " maxi = " << maxi << std::endl;
    
  hDarkH->SetLineColor(kBlack);
  //hDarkH->SetFillColor(kBlack);
  hDarkH->SetMaximum(maxi*1.2);

  TCanvas * c1 = new TCanvas("c1","c1",1000,0,1000,500); 
  c1->Divide(2,1);
  c1->cd(1);
  
  hDarkH->Draw("HIST");
  hDark1->SetLineColor(kBlue);
  hDark1->Draw("HIST SAME");

 
  c1->cd(2);
  TGraph * g1 = new TGraph(nPMTs,dark_rates_HPK,dark_rates_v1); 

  g1->SetMinimum(1000);
  g1->SetMaximum(3000);

  g1->GetXaxis()->SetLimits(1000.,3000.);
  g1->Draw("A*");
  
  TLine * l2 = new TLine(1100,1100,2900,2900);
  l2->SetLineColor(kBlue);
  l2->SetLineStyle(2);
  l2->Draw();
  
  //double noise_rate_10_mV[8] = {166.5,125.0,74.0,186.0,73.5,56.5,113.5,126.5};
    
}
