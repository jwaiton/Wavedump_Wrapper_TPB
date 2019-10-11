#include "./RawData.C"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TLegend.h"

#include "TPaveText.h"

void AnalyseCableData(Int_t testMode = 2){
  
  static const Int_t nRuns = 10, nGates = 5;
  
  RawData * r[nRuns]; 
  TGraph * gModes[nRuns];
  TGraph * gDummy = nullptr;
  
  Double_t   gateWidths[] = {25.,50.,75.,100.,125.};
  //Double_t   gateWidths[] = {50.,100.,200.,350.,500.};
  Double_t   cableLengths[] = {80,64,48,32,28,24,20,16,8,2};
  //Double_t   cableLengths[] = {24,20,16,8,2};
  Double_t * modeValues = new Double_t[nGates];

  
  Double_t mode_50ns_80m = 0;
  
  for (Int_t iRun = 0 ; iRun < nRuns ; iRun++) {
    
    r[iRun] = new RawData(iRun+50);
    
    modeValues =  r[iRun]->Modes4Gates(gateWidths,
				       (Int_t)cableLengths[iRun],
 				       testMode);
    if(iRun==0)
      mode_50ns_80m = modeValues[1];
    
    cout << endl;
    for ( Int_t iGate = 0 ; iGate < nGates ; iGate++ ) {      
      modeValues[iGate] = modeValues[iGate] / mode_50ns_80m;
      
      cout << " modeValues[" << iGate 
	   << "] = " << modeValues[iGate] << endl;
      
    }

    gModes[iRun] = new TGraph(nGates,gateWidths,modeValues);
    
  }
  
  TCanvas * canvas =  new TCanvas("Graphs","Graphs");
  TH1F *hr;
  gPad->SetLogy(0);
  hr = canvas->DrawFrame(15.,0.6,
			 gateWidths[4]+(gateWidths[4]-gateWidths[3]),
			 1.6);
  hr->SetTitle("Gain relative to 80 m cable with 50 ns gate ");

  gStyle->SetTitleFont(40,"");
  gStyle->SetTitleFontSize(0.035);
  
  hr->GetXaxis()->SetTitle("Gate Width (ns)");
  hr->GetYaxis()->SetTitle("Relative mode of charge spectrum ");
  hr->Draw();
  
  TLegend *leg = new TLegend(0.78,0.2,0.89,0.8);
  TString legTitle = "";
  leg->SetTextSize(0.025);
  leg->SetHeader("Cable Lengths","C");
  Int_t colors[5] = {kRed,kOrange,kGreen+2,kCyan+1,kMagenta+1};
  leg->SetMargin(0.4); 
  for (Int_t iRun = 0 ; iRun < nRuns ; iRun++) {
    
    legTitle.Form("%.0f m",cableLengths[nRuns-iRun-1]);
    leg->AddEntry(gModes[nRuns-iRun-1],legTitle,"P L C");  
    
    gModes[iRun]->SetLineColor(colors[iRun%5]);  
    gModes[iRun]->SetMarkerColor(colors[iRun%5]);  
    gModes[iRun]->SetMarkerSize(.5);
    gModes[iRun]->SetMarkerStyle(21);  

    if(iRun > 4){
      gModes[iRun]->SetLineStyle(2);  
      gModes[iRun]->SetMarkerStyle(20);  
    }
    gModes[iRun]->Draw("PC same");        
    leg->Draw("same");
  }

  canvas->SaveAs("./Plots/Gain_vs_GateWidth.pdf");
  
}
