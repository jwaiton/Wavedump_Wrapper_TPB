#include "TH1F.h"
#include "TCanvas.h"

void Cable_Geometry(){
  
  static const int nCables   = 4;
  static const int nMeasures = 5;
  
  float d[nCables][nMeasures] = {{1.00, 1.03, 1.00, 1.00, 0.99},
				 {1.00, 1.04, 1.00, 1.01, 1.06},
				 {1.00, 1.10, 1.01, 1.05, 1.03},
				 {0.99, 1.07, 1.00, 1.03, 0.99}};
  
  float D[nCables][nMeasures] = {{3.65, 3.69, 3.75, 3.71, 3.62},
				 {3.62, 3.63, 3.64, 3.66, 3.68},
				 {3.73, 3.67, 3.69, 3.64, 3.69},
				 {3.67, 3.71, 3.71, 3.58, 3.71}};
  
  TH1F * h_d[nCables];
  TH1F * h_D[nCables];
  
  TH1F * h_d_all;
  TH1F * h_D_all;
  
  char str_name[128];
  char str_title[128];
  
  sprintf(str_name,"h_d_all");
  sprintf(str_title,"h_d_all;Inner conductor diameter (mm);Counts");
  h_d_all = new TH1F(str_name,str_title,21,0.920,1.130);

  sprintf(str_name,"h_D_all");
  sprintf(str_title,"h_D_all;Dielectric diameter (mm);Counts");
  h_D_all = new TH1F(str_name,str_title,21,3.595,3.805);
  
  for( int iCable = 0 ; iCable < nCables ; iCable++ ){
    
    sprintf(str_name,"h_d_%d",iCable);
    sprintf(str_title,"h_d_%d;Inner conductor diameter (mm);Counts",iCable);
    h_d[iCable] = new TH1F(str_name,str_title,21,0.920,1.130);
    
    sprintf(str_name,"h_D_%d",iCable);
    sprintf(str_title,"h_D_%d;Dielectric diameter (mm);Counts",iCable);
    h_D[iCable] = new TH1F(str_name,str_title,21,3.595,3.805);
    
    for (int iMeasure = 0; iMeasure < nMeasures ; iMeasure++){
      h_d[iCable]->Fill(d[iCable][iMeasure]);
      h_D[iCable]->Fill(D[iCable][iMeasure]);

      h_d_all->Fill(d[iCable][iMeasure]);
      h_D_all->Fill(D[iCable][iMeasure]);
    }
    
  }  
  
  TCanvas * canvas = new TCanvas("canvas","canvas");
  
  canvas->Divide(2,2);

  Int_t colors[] = {kRed-7,kOrange+2,kGreen-5,kMagenta+1};
  
  // inner diameter
  canvas->cd(1);
  
  for (int iCable = 0; iCable < nCables ; iCable++){
    h_d[iCable]->SetLineColor(colors[iCable]);
    h_d[iCable]->SetMaximum(3.5);
    h_d[iCable]->Draw("same");
  }
  
  // outer diameter
  canvas->cd(2);
  
  for (int iCable = 0; iCable < nCables ; iCable++){
    h_D[iCable]->SetLineColor(colors[iCable]);
    h_D[iCable]->SetMaximum(3.5);
    h_D[iCable]->Draw("same");
  }

  // inner diameter
  canvas->cd(3);
  
  for (int iCable = 0; iCable < nCables ; iCable++){
    h_d_all->Draw();
  }

  // outer diameter
  canvas->cd(4);
  
  for (int iCable = 0; iCable < nCables ; iCable++){
    h_D_all->Draw();
  }
  
}
