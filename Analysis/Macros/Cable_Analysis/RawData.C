/* 
   A program to analyse raw root data
   BinToRoot output (wavedump waveforms) 
   
   gary.smith@ph.ed.ac.uk 
   24th June 2019

   How to run:
   $ root 
   [0] .L RawData.C+;
   [1] Int_t run = 50;
   [2] RawData * rD = new RawData(run);
   [5] Double_t gateWidths[] = {25.,50.,75.,100.,125.};  
   [4] Double_t cableLength = 80.;
   [3] Int_t    testMode = 0; 
   [4] rD->Modes4Gates(gateWidths,cableLength,testMode);

   Input File:
   Hard coded path in header file of form
   ID.Form("Run_%d_PMT_152_Loc_0_HV_4",run);
   
*/

#define RawData_cxx
#include "RawData.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLatex.h>
#include <TColor.h>

#include <cstdlib>
#include <iostream>

using namespace std;

Float_t GetDelay(Int_t run){
  
  if     ( run == 50 )
    return 105;
  else if( run == 51 )
    return 115;
  else if( run == 52 )
    return 80;
  else if( run == 53 )
    return 93;
  else if( run == 54 )
    return 93;
  else if( run == 55 )
    return 82;
  else if( run == 56 )
    return 88;
  else if( run == 57 )
    return 93;
  else if( run == 58 )
    return 98;
  else if( run == 59 )
    return 91;
  else{
    cerr << " Error: No delay value " << endl;
   return 400;
  }
  
}

Double_t * RawData::Modes4Gates(Double_t gateWidths[5],
				Int_t cableLength,
				Int_t testMode)
{
  if (fChain == 0) return nullptr;  
  
  // investigate 5 charge integration intervals
  static const Int_t nGates = 5;
  static Double_t modeValues[nGates];
  
  // plotting tools
  TString canvasName = "";
  canvasName.Form("canvas_run_%d",run);
  TCanvas * canvas =  new TCanvas(canvasName,canvasName);
  TLatex * latex = new TLatex(); 
  latex->SetNDC();
  latex->SetTextSize(0.03);;
  TString text = "";
  Int_t colors[5] = {kRed,kOrange,kGreen+2,kCyan+1,kMagenta+1};
  
  // arrays of histograms
  // Gain, and (limited) Range for find mode
  TH1F * hG[nGates], * hR[nGates];
  
  static const Int_t   nBinsG = 200;
  static const Float_t minG   = -0.5, maxG = 5.;
  TString hName  = "", hLabel = "";
  for (Int_t iGate = 0 ; iGate < nGates ; iGate++ ){
    hName.Form("hG_%0.f",gateWidths[iGate]);
    
    // value for 'run' is set in header 
    hLabel.Form("Run %d (%d m cable);Gain /10^{7};Counts",
		run,cableLength);
    hG[iGate] = new TH1F(hName,hLabel,nBinsG,minG,maxG);
    
    hName.Form("hR_%0.f",gateWidths[iGate]);
    hR[iGate] = new TH1F(hName,hLabel,nBinsG,minG,maxG);
  }
  
  // To Do: Replace hard coded delay with a method
  Float_t delay = GetDelay(run);
  Float_t delay2 = delay + 200; // for randoms study
  //Float_t baselineWidth = delay;
  Float_t baselineWidth = 50.;
  
  // variables for integrating signal 
  // and subtracting baseline
  Int_t   baselineVT = 0;
  Int_t   signalVT[nGates] = {0};
  Int_t   randomVT[nGates] = {0};
  Float_t signalQ[nGates]  = {0.};
  
  Long64_t nentries = fChain->GetEntriesFast();
  
  Int_t verbosity = testMode;
  
  // reduce run time when testing
  if     (testMode >= 3 )
    nentries = 2;
  else if(testMode == 2 )
    nentries = 10;
  else if(testMode == 1 )
    nentries = 100000;
  
  // LOOP over ENTRIES
  for (Long64_t jentry = 0; jentry < nentries; jentry++) {
    
    // connect to tree in root file
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    fChain->GetEntry(jentry);   
    
    // re-set counting variables
    baselineVT = 0;
    for ( Int_t iGate = 0 ; iGate < nGates ; iGate++){
      signalVT[iGate] = 0;
      randomVT[iGate] = 0;
    }

    // LOOP over SAMPLES
    for ( Int_t iSample = 0 ; iSample < 1024 ; iSample++ ){
      
      if( verbosity > 3 )
	cout << " waveform[" << iSample << "] = " 
	     <<   waveform[iSample] << endl;

      // accumulate baseline 
      if     ( iSample >= (delay - baselineWidth) && 
	       iSample < delay ){
	baselineVT += waveform[iSample];
	
	if( verbosity > 2 ){
	  cout << endl;
	  cout << " baselineVT, waveform[" << iSample << "] = " 
	       <<   baselineVT << ", " << waveform[iSample] << endl;
	}
      }
      
      for ( Int_t iGate = 0 ; iGate < nGates ; iGate++){
	
	if( iSample >= delay && iSample < (delay + gateWidths[iGate]) ){
	  signalVT[iGate] += waveform[iSample];
	  
	  if(verbosity > 2 && iGate==2){
	    cout << endl;
	    cout << " signalVT[" << iGate << "], waveform[" << iSample << "] = " 
		 <<   signalVT[iGate] << ", "  << waveform[iSample] << endl;
	  }
	} // end of: if( iSample >= delay &
	
	if( iSample >= delay2 && iSample < (delay2 + gateWidths[iGate]) ){
	  randomVT[iGate] += waveform[iSample];
	  
	  if(verbosity > 2 && iGate==2){
	    cout << endl;
	    cout << " randomVT[" << iGate << "], waveform[" << iSample << "] = " 
		 <<   randomVT[iGate] << ", "  << waveform[iSample] << endl;
	  }
	} // end of: if( iSample >= delay &
      } // end of : for ( Int_t iGate = 0 ; 
      
    }// SAMPLES
    
    for ( Int_t iGate = 0 ; iGate < nGates ; iGate++){
      

      // scale baseline to signal
      signalQ[iGate] =  (Float_t)gateWidths[iGate]/baselineWidth*(Float_t)baselineVT; 
      // pulses are negative
      
      signalQ[iGate] -= (Float_t)signalVT[iGate] ;
      
      // Set to integrate outside of signal pulse region
      //signalQ[iGate] -= (Float_t)randomVT[iGate] ;
      
      signalQ[iGate] *= 1.0e3 / 4096.; 

      // convert to gain (400 mVns = 10^7)
      signalQ[iGate] /= 400.; 
    
      if( verbosity > 3 ){
	cout << endl;
	cout << " signalQ["  << iGate << "] = " << signalQ[iGate]  << endl;
      }
      
      hG[iGate]->Fill(signalQ[iGate]);
      hR[iGate]->Fill(signalQ[iGate]);
      
    }
  } // ENTRIES
  
  gPad->SetLogy();
  gStyle->SetOptStat(0);

  for ( Int_t iGate = 0 ; iGate < nGates ; iGate++){
    
    hG[iGate]->SetLineColor(colors[iGate]);
    hG[iGate]->SetLineWidth(2);
    
    hG[iGate]->Draw("same");
    
    // range for signal peak ( exclude pedestal )
    Int_t lowBin  = (Int_t)( ( 0.8 - minG ) * nBinsG/( maxG - minG ) );
    Int_t highBin = (Int_t)( ( 2.0 - minG ) * nBinsG/( maxG - minG ) );
    
    hR[iGate]->GetXaxis()->SetRange(lowBin,highBin);
    
    Float_t maxBin = hR[iGate]->GetMaximumBin();

    modeValues[iGate] = hR[iGate]->GetXaxis()->GetBinCenter(maxBin);
    
    text.Form("#color[%d]{%.0f ns gate: sig. mode = %.2f}",
	      colors[iGate],gateWidths[iGate],modeValues[iGate]);

    
    hR[iGate]->Draw();
    latex->DrawLatex(0.6,1-((Float_t)(iGate+4)/20),text);
    
    canvasName.Form("./Plots/hQ_%dns_Run_%d.png",(Int_t)gateWidths[iGate],run);
    canvas->SaveAs(canvasName);
    

  }
  
  for ( Int_t iGate = 0 ; iGate < nGates ; iGate++){
    
    if(iGate==0)
      hG[iGate]->Draw();
    else
      hG[iGate]->Draw("same");
    
    text.Form("#color[%d]{%.0f ns gate: sig. mode = %.2f}",
	      colors[iGate],gateWidths[iGate],modeValues[iGate]);
    latex->DrawLatex(0.6,1-((Float_t)(iGate+4)/20),text);
    
  }
  
  canvasName.Form("./Plots/hQ_Run_%d.png",run);
  canvas->SaveAs(canvasName);
  
  canvas->Close();
  
  for ( Int_t iGate = 0 ; iGate < nGates ; iGate++){
    hG[iGate]->Delete();
    hR[iGate]->Delete();
  }

  return modeValues;  
}
