#define PMTAnalyser_cxx
#include "PMTAnalyser.h"
#include <TH2.h>
#include <TStyle.h>
#include "TBranch.h"
#include "TLatex.h"
#include "TROOT.h"

void PMTAnalyser::SetStyle(){
  
  TStyle     *binToRootStyle  = new TStyle("binToRootStyle",
					   "My Root Styles");
  
  const Int_t NCont = 255;
  const Int_t NRGBs = 5;
  
  // Color scheme for 2D plotting with a better defined scale 
  Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };          
  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  
  binToRootStyle->SetNumberContours(NCont);
  
  // General
  // OPTIONS - FILL LINE TEXT MARKER
  
  binToRootStyle->SetFillColor(0);
  binToRootStyle->SetTextSize(0.05);
  
  //-----------  Canvas
  
  binToRootStyle->SetCanvasBorderMode(0);
  binToRootStyle->SetCanvasColor(kWhite);
  
  //------------- Pad
  
  binToRootStyle->SetPadBorderMode(0); 
  binToRootStyle->SetPadColor(kWhite);
  
  // Make more room for X and Y titles
  // one pad
  // binToRootStyle->SetPadRightMargin(0.05);  //percentage
  // binToRootStyle->SetPadLeftMargin(0.1);    //percentage
  // binToRootStyle->SetPadBottomMargin(0.12); //percentage
  
  // six sub-pads
  binToRootStyle->SetPadRightMargin(0.16);  //percentage
  binToRootStyle->SetPadLeftMargin(0.2);    //percentage
  binToRootStyle->SetPadBottomMargin(0.14); //percentage
  
  //----------- Histogram
  
  //Histos
  binToRootStyle->SetHistLineWidth(1);
  binToRootStyle->SetMarkerStyle(20);
  
  //  FILL CONTOURS LINE BAR 
  //  Frames
  binToRootStyle->SetFrameBorderMode(0);
  
  //  FILL BORDER LINE
  //  Graphs
  //  LINE ERRORS
  
  //---------  Axis 
  
  binToRootStyle->SetLabelFont(132,"XYZ"); 
  binToRootStyle->SetLabelSize(0.04,"XYZ");
  binToRootStyle->SetLabelOffset(0.01 ,"Y");
  
  //---------  Title
  binToRootStyle->SetOptTitle(1);
  binToRootStyle->SetTitleStyle(0);
  binToRootStyle->SetTitleBorderSize(0);


  binToRootStyle->SetTitleSize(0.03,"t");
  binToRootStyle->SetTitleFont(132,"t"); 

  binToRootStyle->SetTitleFont(132,"XYZ"); 

  binToRootStyle->SetTitleSize(0.05,"XYZ");
  
  binToRootStyle->SetTitleOffset(1.0,"XYZ");
  
  // 6 sub-pads
  binToRootStyle->SetTitleOffset(1.6,"Y");
  
  //----------  Stats
  binToRootStyle->SetOptStat(0);
  binToRootStyle->SetStatStyle(0);

  binToRootStyle->SetOptFit(1);
  
  //----------  Legend
  binToRootStyle->SetLegendBorderSize(0);
  //binToRootStyle->SetLegendFont(132);
  
  gROOT->SetStyle("binToRootStyle");
  gROOT->ForceStyle();
}

// !!! IN PROGRESS
Int_t PMTAnalyser::DarkRate(Float_t threshold = 10)
{
  if (rawRootTree == 0) return -1;
  
  int verbosity = 0;
  
  Long64_t ientry;
  
  Long64_t nentries = rawRootTree->GetEntriesFast();
  Long64_t nDark    = 0 ;
  
  cout << endl;
  cout << " Calculating Dark Rate " << endl;
  cout << " " << nentries << " entries " << endl;

  for (Long64_t jentry=0; jentry < nentries; jentry++) {
    
    ientry = LoadTree(jentry);
    
    if (ientry < 0) break;
    rawRootTree->GetEntry(jentry);   
    
//     for( int iSample = 0 ; iSample < NSamples; iSample++){
      
//     }

    if( peakV_mV > threshold )
      nDark++;

    if(verbosity > 0){
      cout << endl;
      cout << " jentry        = " << jentry        << endl;
    }
    
  }  

  //!!!! Temporary
  Float_t darkRate = (Float_t)nDark/nentries/220.*1.0e9;
  
  if(verbosity > 0){  
    cout << endl;
    cout << " nentries = " << nentries << endl;
    cout << " nDark    = " << nDark    << endl;
    cout << " rate     = " << darkRate << endl;
  }
  
  return darkRate;
}


// !!! IN PROGRESS
Int_t PMTAnalyser::Make_hFixed_Filtered(){
  
  if (rawRootTree == 0) return -1;

  double waveformDuration = NSamples * nsPerSample;

  TH1D * hWave = new TH1D("hWave","hWaveform;Time /ns;ADC counts",
			NSamples, 0., waveformDuration);
  
  Long64_t ientry;
  Long64_t nentries = rawRootTree->GetEntriesFast();

  //!!! Test
  nentries = 300000;
  
  double aoff = 8700;
  
  for (Long64_t jentry = 0; jentry < nentries; jentry++) {
    
    ientry = LoadTree(jentry);
    if (ientry < 0) break;
    rawRootTree->GetEntry(jentry);   
    
    if( jentry%100000==0 )
      cout << " entry = " << jentry << endl; 
    
    for( int iSample = 0 ; iSample < NSamples; iSample++){
      
      hWave->SetBinContent(iSample+1,
			   (double)(aoff - waveform[iSample]));
      
    } // end: for( int iSample = ...


    // Fill unfiltered histogram

    // Check this entry's waveform
    if( IsCleanFFTWaveform(hWave) ){
      
      // Fill filtered histograms
      
    }
  } //end of: for (Long64_t jentry = 0; jentry

  // Draw plots and append to root file
  
  return 1;

}


Bool_t PMTAnalyser::IsCleanFFTWaveform(TH1D * hWave){
  
  Bool_t waveformIsClean = kFALSE;

  TH1F* hWaveFFT  = new TH1F("hWaveFFT","hWaveFFT",
			    NSamples, 0, NSamples);
  
  hWave->FFT(hWaveFFT ,"MAG");
  
  // delete zero frequency data
  hWaveFFT->SetBinContent(1,0.) ;
  
  if(hWaveFFT->GetMaximumBin() == 2)
    waveformIsClean = kTRUE;
  
  hWaveFFT->Delete();
    
  return waveformIsClean;
}


 // 
 // Member Function to test filtering noisy waveforms 
 //
 // Author 
 //  Steve Quillin
 //
 // Translated  22 02 19 
 //  gary.smith@ed.ac.uk
//
//
// Description 
//
//  Smallest non-zero frequency component in bin #2 
//  at a frequency of  1/110 samples * 2ns/sample ~ 4.55 MHz
//  bins 2 - 54 hold magnitudes at multiples of this frequency
//  bin 55  Nyquist frequency component at 250 MHz = 1/2*2ns  
//
//  Note that the magnitude of the zero frequency FFT bin is 
//  a measure of the trace baseline value (multiplied by the 
//  number of samples)
//  Propose using only data traces where the peak FFT component 
//  falls in the lowest non-zero bin (bin #2)

TCanvas * PMTAnalyser::Make_FFT_Canvas()
{
  
  TCanvas * canvas = new TCanvas();

  if (rawRootTree == 0) return nullptr;
  
  int verbosity = 1;

  double waveformDuration = NSamples * nsPerSample;
  
  TH1D* hWave = new TH1D("hWave","hWaveform;Time /ns;ADC counts",
			NSamples, 0., waveformDuration);
  
  TH1F* hWaveFFT      = nullptr;

  TH1F* hWaveFFT_Acc  = new TH1F("hWaveFFT_Acc",
				 "hWaveFFT_Acc",
				 NSamples, 0, NSamples);
  
  TH1F* hWaveFFT_MaxBin  = new TH1F("hWaveFFT_MaxBin",
				    "hWaveFFT_MaxBin",
				    NSamples, 0, NSamples);
  
  TH1F* hWaveFFT_MaxBin_Acc  = new TH1F("hWaveFFT_MaxBin_Acc",
					"hWaveFFT_MaxBin_Acc",
					NSamples, 0, NSamples);
  
  TH1D* hMaxADC = new TH1D("hMaxADC","Waveform ADC Maximum;maxADC;counts",
			   1000, 200.,1200.); 
  
  TH1D* hMaxADC_Filtered = new TH1D("hMaxADC_Filtered",
				    "hMaxADC_Filtered;maxADC;counts",
				    1000, 200.,1200.);
  
  TH1D* hMaxADC_Filtered_Acc = new TH1D("hMaxADC_Filtered_Acc",
					"hMaxADC_Filtered_Acc;maxADC;counts",
					1000, 200.,1200.);
  
  Long64_t ientry;
  Long64_t nentries = rawRootTree->GetEntriesFast();
  
  double aoff   = 8700.; // arbitrary offset / baseline
  double fftoff = 0;     // fft offset / baseline

  cout << endl;
  cout << " Performing FFT " << endl;
  
  //!!!!!!!
  // testing
  //  nentries = 200000;
  
  for (Long64_t jentry = 0; jentry < nentries; jentry++) {
    
    ientry = LoadTree(jentry);
    if (ientry < 0) break;
    rawRootTree->GetEntry(jentry);   
    
    if( jentry%100000==0 )
      cout << " entry = " << jentry << endl; 

    // make waveform histogram    
    for( int iSample = 0 ; iSample < NSamples; iSample++){
      hWave->SetBinContent(iSample+1,
			  (double)(aoff - waveform[iSample]));
      
      if( verbosity > 1){
	cout << endl;
	cout << " waveform[" << iSample << "] = " 
	     <<   waveform[iSample] << endl;
      }
	
    } // end: for( int iSample = ...
    // waveform histogram made    
    
    hMaxADC->Fill( hWave->GetMaximum() );
    
    //------
    // FFT of this entry's waveform only
    hWaveFFT = new TH1F("hWaveFFT","hWaveFFT;",
			NSamples, 0, NSamples);

    hWave->FFT(hWaveFFT ,"MAG");

    hWaveFFT->SetBinContent(1,0.);

    // Store maximum FFT bin 
    hWaveFFT_MaxBin->Fill(hWaveFFT->GetMaximumBin());
    
    if(hWaveFFT->GetMaximumBin() == 2)
      hMaxADC_Filtered->Fill( hWave->GetMaximum());
    
    hWaveFFT->Delete();
    //--------
    
    //-----------
    // Accumulate FFTs of waveforms for all entries 
    hWave->FFT(hWaveFFT_Acc ,"MAG");
    
    hWaveFFT_Acc->SetBinContent(1,0.);
    
    // Store maximum FFT bin of accumulated
    hWaveFFT_MaxBin_Acc->Fill(hWaveFFT_Acc->GetMaximumBin());
    
    // Check the accumulated waveform FFTs
    if(hWaveFFT_Acc->GetMaximumBin()==2)
      hMaxADC_Filtered_Acc->Fill( hWave->GetMaximum());
    
    fftoff = hWaveFFT_Acc->GetBinContent(1)/(double)NSamples; 
    
    if(verbosity > 1){
      cout << endl;
      cout << " jentry = " << jentry << endl;
      cout << " fftoff = " << fftoff << endl;
    }
    
  } //end of: for (Long64_t jentry = 0; jentry
  
  if(verbosity > 0){  
    cout << endl;
    cout << " nentries = " << nentries << endl;
  }

   canvas->Divide(2,2);

   canvas->cd(1);
   gPad->SetLogy(1);

   hMaxADC->Draw();
   
   hMaxADC_Filtered->SetLineColor(kBlue);
   hMaxADC_Filtered->Draw("same");
   
   hMaxADC_Filtered_Acc->SetLineColor(kRed);
   hMaxADC_Filtered_Acc->Draw("same");
   
   TLatex * latex = new TLatex();
   TString entriesStr = " %d entries ";
  
   latex->SetNDC();

   latex->SetTextSize(0.025);
   latex->SetTextAlign(12);  //align at top
   
   entriesStr.Form("hMaxADC: %d entries ",
		   (int)hMaxADC->GetEntries());
   
   latex->DrawLatex(0.5,0.8,entriesStr);
   
   latex->SetTextColor(kBlue);
   
   entriesStr.Form("FFT Filtered: %d entries ",
		   (int)hMaxADC_Filtered->GetEntries());
   latex->DrawLatex(0.5,0.75,entriesStr);
   
   latex->SetTextColor(kRed);
   entriesStr.Form("Filter Accum.: %d entries ",
		   (int)hMaxADC_Filtered_Acc->GetEntries());
   latex->DrawLatex(0.5,0.7,entriesStr);
  
   canvas->cd(2);  
   
   hWaveFFT_MaxBin->SetLineColor(kBlue);  
   hWaveFFT_MaxBin->Draw();
   
   hWaveFFT_MaxBin->SetLineColor(kRed);  
   hWaveFFT_MaxBin_Acc->Draw("same");
   
   canvas->cd(3);  
   
   hWaveFFT_Acc->Draw();

   canvas->cd(4);  

   return canvas;
}
