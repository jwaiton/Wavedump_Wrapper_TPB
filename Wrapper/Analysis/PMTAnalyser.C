#define PMTAnalyser_cxx
#include "PMTAnalyser.h"
#include <TH2.h>
#include <TF1.h>
#include <TStyle.h>
#include "TBranch.h"
#include "TLatex.h"
#include "TROOT.h"

#include <stdlib.h>

#include "../BinToRoot/wmStyle.C"

Int_t PMTAnalyser::GetNEntriesTest(Int_t verbosity,
				   Int_t nentries){
  switch ( verbosity ){
  case(3):
    return 2;
  case(2):
    return 10;
  case(1):
    return 10000;
  case(0):
    return 1000000;
  default:
    return nentries; 
  }
  
}

void PMTAnalyser::MakeCalibratedTree(){
  
  if (rawRootTree == 0) return;
  
  int verbosity = 2;

  Long64_t ientry;
  Long64_t nentries = rawRootTree->GetEntriesFast();
  
  if(testMode)
    nentries = GetNEntriesTest(verbosity,nentries);
  
  cout << endl;
  cout << " Making Calibrated Data Tree " << endl;
  cout << " " << nentries << " entries " << endl;
  
  for(Long64_t jentry = 0; jentry < nentries; jentry++) {
    
    ientry = LoadTree(jentry);
    if (ientry < 0) break;
    rawRootTree->GetEntry(jentry);   
    
    if(verbosity > 1){
      cout << endl;
      cout << " entry = " << jentry << endl;
    }
    
    for( int iSample = 0 ; iSample < NSamples; iSample++){
      
      if(verbosity > 2){
	cout << endl;
	cout << " waveform[" << iSample << "] = " << waveform[iSample] << endl;
	
      }
      
    }
    
  }  
  return;  
}

// Authored by Steve Quillin
// dx is the untransformed data spacing 
TH1F* PMTAnalyser::FFTShift(TH1F* hFFT, Float_t dx){ 
  
  Int_t   N    = hFFT->GetNbinsX(); 
  Float_t df   = 1./dx/N / 1.E6; // bin widthin MHz
  Int_t   newN = N/2; 

  TString tStr = "";
  tStr.Form(" Counts / %.1f MHz ",df);
  
  TString label;
  label  = "FFT Peak Frequency;";
  label += "FFT Frequency (MHz);";
  label += tStr;
    
  TH1F* hFFT_Shift = new TH1F("hFFT_Shift",
			      label,
			      newN, 0.,df*newN); 
  
  for (int qq=1; qq<=newN; qq++) 
    hFFT_Shift->SetBinContent(qq,hFFT->GetBinContent(qq)); 
  
  hFFT->Delete();
  
  return hFFT_Shift; 
} 


void PMTAnalyser::PlotAccumulatedFFT(){
  
  TCanvas * canvas = new TCanvas();
  
  Float_t w = 1000., h = 500.;
  canvas->SetWindowSize(w,h);

  Long64_t nentries = rawRootTree->GetEntriesFast();   
  cout << endl;
  cout << " Running: PlotAccumulatedFFT " << endl;
  cout << " " << nentries << " entries " << endl;
  
  TH1F * hFFT, * hTemp;
  hTemp = Get_hFFT(0);
  hFFT = (TH1F*)hTemp->Clone(); 
  hTemp->Delete();

  for(Long64_t jentry = 1; jentry < nentries; jentry++) {
    hTemp = Get_hFFT(jentry);
    
    if (peakV_mV > 10.)
    hFFT->Add(hTemp);
    hTemp->Delete();
  }

  hFFT->SetBinContent(1,0.);
  
  double_t norm = 1;
  hFFT->Scale(norm/hFFT->Integral(),"width");

  hFFT->SetMaximum(0.1);
  hFFT->SetMinimum(0.0001);
  
  gPad->SetLogy();
  hFFT->Draw("hist");

  TString hName = "./FFT/hFFT_";
  hName += FileID;
  hName += ".root";
  
  canvas->SaveAs(hName);
  
  hFFT->Delete();
  
}

Bool_t PMTAnalyser::IsValidEntry(Long64_t entry){
  
  Long64_t nFileEntries = rawRootTree->GetEntriesFast();   
  
  if( entry > (nFileEntries-1) ){
    cout << endl;
    cout << " Last entry in file is " << (nFileEntries-1) << endl;
    return kFALSE;
  }
  
  return kTRUE;
}

void PMTAnalyser::PlotFFT(Long64_t entry){

  if(!IsValidEntry(entry))
    return;
  
  TCanvas * canvas = new TCanvas();
  
  Float_t w = 600., h = 500.;
  canvas->SetWindowSize(w,h);
  
  TH1F *  hFFT = Get_hFFT(entry);
  hFFT->SetNameTitle("hFFT","hFFT");
  
  gPad->SetLogy();
  hFFT->Draw();

  TString hName;
  hName = "./FFT/";
  hName += FileID;
  hName += "_FFT_C";
  hName += ".png";
  canvas->SaveAs(hName);
  
  string sysCommand = "open ";
  sysCommand += hName;
  system(sysCommand.c_str());
  
  hFFT->Delete();
}

void PMTAnalyser::PlotWaveform(Long64_t entry){
  
  if(!IsValidEntry(entry))
    return;

  TCanvas * canvas = new TCanvas();
  
  Float_t w = 600., h = 500.;
  canvas->SetWindowSize(w,h);

  TH1F * hWave = new TH1F("hWave","Waveform;Time (ns); Voltage (ADC counts)",
			  NSamples, 0., waveformDuration);

  Get_hWave(entry,hWave);
  
  Float_t lineXMin = 0.;
  Float_t lineXMax = 15.;

  // Set pointers to desired event in tree to
  // access correct waveform in Get_baseline_ADC()
  rawRootTree->GetEntry(entry);
 
  Float_t lineYMin = Get_baseline_ADC();
  Float_t lineYMax = lineYMin;

  TLine *baseline_0 = new TLine(lineXMin,lineYMin,
				lineXMax,lineYMax); 
  
  Float_t lineXLimit = hWave->GetXaxis()->GetXmax();
  
  TLine *baseline_0_long = new TLine(lineXMax,lineYMin,
				     lineXLimit,lineYMax); 
  
  // alternative method using full waveform
  lineYMin = Get_baseline_ADC(1);
  lineYMax = lineYMin;
  
  TLine *baseline_1  = new TLine(lineXMin,lineYMin,
				 lineXLimit,lineYMax); 
  
  baseline_0->SetLineColor(kBlue);
  baseline_0->SetLineWidth(3.0);

  baseline_0_long->SetLineColor(kRed);
  baseline_0_long->SetLineWidth(3.0);
  
  baseline_1->SetLineColor(kGreen);
  baseline_1->SetLineWidth(3.0);
  
  TString tStr = "";
  TLatex * latex = new TLatex();

  cout << endl;
  cout << " entry = " << entry << endl;
    
  hWave->Draw();
  baseline_0->Draw();
  baseline_0_long->Draw();
  baseline_1->Draw();
  
  tStr.Form("Entry %lld", entry);
  latex->DrawLatex(0.6,0.8,tStr);

  cout << endl;
  cout << " Plotting Waveform" << endl;
  
  TString hName;
  hName = "./Waveforms/";
  hName += FileID;
  hName += "_Wave_C";
  hName += ".png";
  canvas->SaveAs(hName);

  string sysCommand = "open ";
  sysCommand += hName;
  system(sysCommand.c_str());

  hWave->Delete();

  return;
}

TH1F * PMTAnalyser::Get_hFFT(Long64_t entry){
  
  TH1F *  hFFT = new TH1F("hFFT","hFFT",
			  NSamples, 0, NSamples);
  
  TH1F * hWave = new TH1F("hWave","Waveform;Time (ns); Voltage (ADC counts)",
			  NSamples, 0., waveformDuration);

  Get_hWave(entry,hWave); 

  hWave->FFT(hFFT ,"MAG");
 
  hWave->Delete();
    
  Float_t dx = nsPerSample * 1.E-9;
  
  // convert to frequency bins
  hFFT = FFTShift(hFFT,dx);
  
  return hFFT;
}

void PMTAnalyser::Get_hWave(Long64_t entry, TH1F * hWave){
  
  LoadTree(entry);
  rawRootTree->GetEntry(entry);  
  
  for( int iSample = 0 ; iSample < NSamples; iSample++)
    hWave->SetBinContent(iSample+1,(waveform[iSample]));
}

// !!! Temporary settings to be investigated
Bool_t PMTAnalyser::IsSampleInBaseline(int iSample,
				       Short_t option){
  Float_t time_ns = (Float_t)iSample * nsPerSample;
  
  switch (option){
  case(0):
    if (time_ns < 15.)
      return kTRUE;
    else
      return kFALSE;
  case(1):
    return kTRUE;
  }
  return kTRUE;
}

// This method requires to set the tree 
// entry in the parent function
Short_t PMTAnalyser::Get_baseline_ADC(Short_t  option,
				      Long64_t entry){
  
  Int_t baseline_ADC = 0;
  Int_t samplesInBaseline = 0;  

  if( entry > -1){
    LoadTree(entry);
    rawRootTree->GetEntry(entry);  
  }
  
  for( int iSample = 0 ; iSample < NSamples; iSample++){
    
    if ( IsSampleInBaseline(iSample,option)){
      baseline_ADC += waveform[iSample];
      samplesInBaseline++;
    }
    
  }
  baseline_ADC = baseline_ADC/samplesInBaseline;
  
  return (Short_t)baseline_ADC;
}

Float_t PMTAnalyser::Get_baseline_mV(Short_t option,
				     Long64_t entry){
  return (Float_t)Get_baseline_ADC(option,entry) * mVPerBin;
}

Short_t PMTAnalyser::Select_peakSample(Short_t waveform[],
				       Short_t peakVDC){
  

  int nPeaks = 0; // number of repeats
  int peakSampleArray[NSamples]; 
  int peakNumberChoice = 0; 
  int peakSample = -1;
  
  for( int iSample = 0 ; iSample < NSamples; iSample++){
    peakSampleArray[iSample] = 0;
    
    if( waveform[iSample] == peakVDC ){
      peakSampleArray[nPeaks] = iSample;
      nPeaks++;
    }
  } // end of: for( int iSample = 0 ; iSample < N......
  
  if(nPeaks == 0)
    cerr << " no peaks found is...... impossible " << endl;
  
  // randomly assign integer from [0,(nPeaks-1)]
  peakNumberChoice = (Int_t)rand3->Uniform(nPeaks);
  
  peakSample = peakSampleArray[peakNumberChoice];
  
  return peakSample;
}

// Short_t PMTAnalyser::GetNPeaks(Int_t   event = 0,
// 			       Float_t threshold = 0.0){
//   Int_t nPeaks = 0;
//   rawRootTree->GetEntry(entry);
  
//   Float_t baseline_mV = Get_baseline_mV(threshold);
  
//   for( int iSample = 0 ; iSample < NSamples; iSample++){
//     if( voltage_mV(waveform[iSample]  
//   }
//   return nPeaks;
// }

//void PMTAnalyser::PlotNPeaks(){
  
//   for (Long64_t jentry = 0; jentry < nentries; jentry++) {
  
//   }
//  return;
//}

Float_t PMTAnalyser::TimeOfPeak(Float_t threshold = 0.0)
{
  if (rawRootTree == 0) return -1.;
  
  int verbosity = 0;
  
  Long64_t ientry;
  Long64_t nentries = rawRootTree->GetEntriesFast();
  
  if(testMode)
    nentries = GetNEntriesTest(verbosity,nentries);

  cout << endl;
  cout << " Running: Time of Peak " << endl;
  cout << " " << nentries << " entries " << endl;
  
  TCanvas * canvas = new TCanvas();
  
  Float_t rangeT = waveformDuration;
  int     binsT  = NSamples;
  
  Float_t rangeV[2];
  rangeV[0] = -12.;
  rangeV[1] = 110.;

  if(Test == 'R'){
    rangeV[0] = -100.;
    rangeV[1] = 1400.;
  }
  int binsV = (int)NVDCBins/4;
    
  binsV = binsV * (int)(rangeV[1] - rangeV[0])/1000/VoltageRange;

  if( Test=='A' ){
    rangeT = 220.;
    binsT = 110;
  }

  Int_t nHistos = 3;
  TH1F * hPeakT_ns[nHistos];
  
  TString hName = "hPeakT_ns";
    
  for ( Int_t iHist = 0 ; iHist < nHistos ; iHist++ ){
    
    hName.Form("hPeakT_ns_%d",iHist);
    
    hPeakT_ns[iHist] = new TH1F(hName,
				"time of peak voltage; Time (ns);Counts",
				binsT,0.,rangeT);
  }
  
  TH1F * hPeakV_mV = new TH1F("hPeakV_mV",
			      "peak voltage ; Peak Voltage (mV);Counts",
			      binsV,rangeV[0],rangeV[1]);
  
  binsT = binsT / 2;
  binsV = binsV / 2;
    
  TH2F * hPeakT_PeakV = new TH2F("hPeakT_PeakV",
				 "peak time vs peak voltage; Peak Time (ns); Peak Voltage (mV)",
				 binsT,0.,rangeT,
				 binsV,rangeV[0],rangeV[1]);
				 
  
  binsT = binsT * 2;
  binsV = binsV * 2;
 
  Float_t baseline_mV = 0.0;
  Short_t peakSample = -1;
  
  int peakADC = 0;
  
  for (Long64_t jentry = 0; jentry < nentries; jentry++) {
    
    ientry = LoadTree(jentry);
    if (ientry < 0) break;
    rawRootTree->GetEntry(jentry);   
 
    if(verbosity > 1){
      cout << endl;
      cout << " entry = " << jentry << endl;
    }
  
    // event-by-event baseline    
    baseline_mV = Get_baseline_mV();

    //------------------------------------
    
    // BinToRoot method
    hPeakT_ns[0]->Fill(peakT_ns);
    
    //------------------------------------
    // Randomise peak time from peak
    // voltage duplicates 
    
    if(negPulsePol)
      peakADC = minVDC;
    else
      peakADC = maxVDC;

    peakSample = Select_peakSample(waveform,peakADC);
    peakT_ns   = peakSample * nsPerSample;

    //--------------------------------------
    //--------------------------------------

    peakV_mV = waveform[peakSample] * mVPerBin;
    
    if(negPulsePol)
      peakV_mV = baseline_mV - peakV_mV; 
    else
      peakV_mV = peakV_mV - baseline_mV; 

    hPeakT_PeakV->Fill(peakT_ns,peakV_mV);
    hPeakV_mV->Fill(peakV_mV);
    
    //gPad->SetLogy();
    hPeakT_ns[1]->Fill(peakT_ns);
    hPeakT_ns[1]->SetLineColor(kGreen);
    
    if( peakV_mV < threshold )
      continue;
    
    hPeakT_ns[2]->Fill(peakT_ns);

    if(verbosity > 1){    
      cout << endl; 
      cout << " peakT_ns          = " << peakT_ns          << endl;;
      cout << " peakV_mV          = " << peakV_mV          << endl;;
      cout << " minVDC in mV      = " << minVDC*mVPerBin   << endl;
      cout << " minT in ns        = " << minT*nsPerSample  << endl;
    }
    
  }
  // 
  //------------------------------------
  
  if(verbosity > 0){  
    cout << endl;
    cout << " nentries = " << nentries << endl;
  }
  
  //------------------------------------
  // Fitting
  
  TF1 * fPeakTimeGaus = new TF1("fPeakTimeGaus","gaus(0)",0.,220.);
  
  Float_t peakMean = -1.;

  double rangeFit = 8.5;

  double maxBin = hPeakT_ns[1]->GetMaximumBin();
  
  double maxPeakT_ns = hPeakT_ns[1]->GetXaxis()->GetBinCenter(maxBin);
  
  if(Test!='D'){
    hPeakT_ns[1]->Fit("gaus","Q","", 
		     maxPeakT_ns - rangeFit, 
		     maxPeakT_ns + rangeFit );
    
    
    fPeakTimeGaus = hPeakT_ns[1]->GetFunction("gaus");  
    
    // fit again based on first results
    // 2 sigma range (95 %)   
    maxPeakT_ns = fPeakTimeGaus->GetParameter(1);
    rangeFit = fPeakTimeGaus->GetParameter(2) * 2.0;
    
    hPeakT_ns[1]->Fit("gaus","Q","", 
		     maxPeakT_ns - rangeFit, 
		     maxPeakT_ns + rangeFit );
    
    fPeakTimeGaus = hPeakT_ns[1]->GetFunction("gaus");  
    
    hPeakT_ns[1]->GetFunction("gaus")->SetLineColor(kBlue);
    
    peakMean = fPeakTimeGaus->GetParameter(1);
    
  }
  
  hPeakT_ns[0]->Draw();

  hPeakT_ns[1]->SetMinimum(100);
  
  hPeakT_ns[0]->SetLineColor(kBlue);

  Int_t lowBin  = 0;
  Int_t highBin = waveformDuration;

  if( Test !='R' ){
    lowBin  = maxBin*rangeT/binsT - rangeFit*3;
    highBin = maxBin*rangeT/binsT + rangeFit*10;
  }
  else if( Test=='R'){
    lowBin  = maxBin - 20;
    highBin = maxBin + 20;
  }

  hPeakT_ns[1]->GetXaxis()->SetRange(lowBin, highBin);
  
  hPeakT_ns[1]->Draw();
  //gPad->SetLogy();
  hPeakT_ns[2]->Draw("same");
  
  TLatex * latex = new TLatex();
  latex->SetNDC();
  latex->SetTextSize(0.025);
  latex->SetTextAlign(12);  //align at top
  
  TString tStr = " %d entries ";
  
  tStr.Form("Gaus Mean = %.2f  ",
	    fPeakTimeGaus->GetParameter(1));
   
  //latex->SetTextColor(kBlue);  

  latex->DrawLatex(0.7,0.8,tStr);
  
  tStr.Form("Gaus Sigma = %.2f  ",
	    fPeakTimeGaus->GetParameter(2));
  
  latex->DrawLatex(0.7,0.75,tStr);
  
  hName = "./Plots/PeakTime_";
  hName += FileID;
  hName += ".pdf";
  
  canvas->SaveAs(hName);
  
  hPeakV_mV->Draw();
  gPad->SetLogy(kTRUE);
  
  hName = "./Plots/PeakVolt_";
  hName += FileID;
  hName += ".pdf";
  canvas->SaveAs(hName);
  
  gPad->SetLogy(kFALSE);

  hPeakT_PeakV->Draw("colz");
  gPad->SetLogz(kTRUE);

  hName = "./Plots/Volt_Time_";
  hName += FileID;
  hName += ".pdf";
  
  canvas->SaveAs(hName);
  
  return peakMean;
}

  Int_t PMTAnalyser::DarkRate(Float_t threshold = 10)
{
  if (rawRootTree == 0) return -1;
  
  int verbosity = 0;
  
  Long64_t ientry;
  
  Long64_t nentries = rawRootTree->GetEntriesFast();
  Long64_t nDark    = 0 ;
  
  if(testMode)
    nentries = 100000;
  
  cout << endl;
  cout << " Calculating Dark Rate " << endl;
  cout << " " << nentries << " entries " << endl;

  for (Long64_t jentry=0; jentry < nentries; jentry++) {
    
    ientry = LoadTree(jentry);    
    if (ientry < 0) break;
    rawRootTree->GetEntry(jentry);   
    
    for( int iSample = 0 ; iSample < NSamples; iSample++){
      // do something
    }

    if( peakV_mV > threshold )
      nDark++;

    if(verbosity > 1){
      cout << endl;
      cout << " jentry = " << jentry << endl;
      cout << " nDark  = " << nDark  << endl;

    }
    
  }

  Float_t darkRate = (Float_t)nDark/nentries;
  
  darkRate = darkRate/waveformDuration * 1.0e9;
  
  if(verbosity > 0){  
    cout << endl;
    cout << " nentries = " << nentries << endl;
    cout << " nDark    = " << nDark    << endl;
    cout << " rate     = " << darkRate << endl;
  }
  
  return darkRate;
}


// !!! IN PROGRESS
Int_t PMTAnalyser::FFT_Filter(){
  
  if (rawRootTree == 0) return -1;

  TH1F * hWave = new TH1F("hWave","hWaveform;Time /ns;ADC counts",
			  NSamples, 0., waveformDuration);
  
  TH1F * hFFT  = new TH1F("hFFT","hFFT",
			      NSamples, 0, NSamples);
  
  Long64_t ientry;
  Long64_t nentries = rawRootTree->GetEntriesFast();

  if( testMode )
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

    hFFT->Reset();
    hWave->FFT(hFFT ,"MAG");
    
    ///------------------
    //  Do the filtering
    //if(IsCleanFFTWaveform(hFFT))
    // do someit
    ///-------------------
    
  } //end of: for (Long64_t jentry = 0; jentry

  // Draw plots and append to root file
  
  return 1;
}


Bool_t PMTAnalyser::IsCleanFFTWaveform(TH1F * hFFT){

  // delete zero frequency data
  hFFT->SetBinContent(1,0.);
  
  if(hFFT->GetMaximumBin() == 2)
    return kTRUE;
  else
    return kFALSE;
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
  
  TH1F* hWave = new TH1F("hWave","hWaveform;Time /ns;ADC counts",
			 NSamples, 0., waveformDuration);
  
  TH1F* hFFT = new TH1F("hFFT","hFFT;",
			    NSamples, 0, NSamples);
  
  TH1F* hFFT_MaxBin  = new TH1F("hFFT_MaxBin",
				    "hFFT_MaxBin",
				    NSamples, 0, NSamples);
  
  TH1F* hMaxADC = new TH1F("hMaxADC","Waveform ADC Maximum;maxADC;counts",
			   1000, 200.,1200.); 
  
  TH1F* hMaxADC_Filtered = new TH1F("hMaxADC_Filtered",
				    "hMaxADC_Filtered;maxADC;counts",
				    1000, 200.,1200.);
  
  
  Long64_t ientry;
  Long64_t nentries = rawRootTree->GetEntriesFast();
  
  double aoff   = 8700.; // arbitrary offset / baseline
  double fftoff = 0;     // fft offset / baseline

  cout << endl;
  cout << " Performing FFT " << endl;
  
  if( testMode)
    nentries = 10000;
  
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
    
    hFFT->Reset();

    hWave->FFT(hFFT ,"MAG");
    
    hFFT->SetBinContent(1,0.);

    // Store maximum FFT bin 
    hFFT_MaxBin->Fill(hFFT->GetMaximumBin());
    
    if(hFFT->GetMaximumBin() == 2 )
      hMaxADC_Filtered->Fill( hWave->GetMaximum());
    
    fftoff = hFFT->GetBinContent(1)/(double)NSamples; 
    
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
   
   TLatex * latex = new TLatex();
   TString entriesStr = " %d entries ";
  
   latex->SetNDC();

   latex->SetTextSize(0.025);
   latex->SetTextAlign(12);  //align at top
   
   entriesStr.Form("hMaxADC: %d entries ",
		   (int)hMaxADC->GetEntries());
   
   latex->DrawLatex(0.6,0.8,entriesStr);
   
   latex->SetTextColor(kBlue);
   
   entriesStr.Form("FFT Filtered: %d entries ",
		   (int)hMaxADC_Filtered->GetEntries());
   latex->DrawLatex(0.6,0.75,entriesStr);
  
   canvas->cd(2);  
   
   hFFT_MaxBin->SetLineColor(kBlue);  
   hFFT_MaxBin->Draw();
   
   canvas->cd(3);  
   
   hFFT->Draw();

   canvas->cd(4);  

   return canvas;
}

void PMTAnalyser::SetTestMode(Bool_t userTestMode){
  testMode = userTestMode;
}


void PMTAnalyser::SetStyle(){
  
  TStyle *wmStyle = GetwmStyle();
  
  const Int_t NCont = 255;
  const Int_t NRGBs = 5;
  
  // Color scheme for 2D plotting with a better defined scale 
  Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };          
  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  
  wmStyle->SetNumberContours(NCont);
  
  
  // // General
//   // OPTIONS - FILL LINE TEXT MARKER
  
//   wmStyle->SetFillColor(0);
//   wmStyle->SetTextSize(0.05);
  
//   //-----------  Canvas
  
//   wmStyle->SetCanvasBorderMode(0);
//   wmStyle->SetCanvasColor(kWhite);
  
//   //------------- Pad
  
//   wmStyle->SetPadBorderMode(0); 
//   wmStyle->SetPadColor(kWhite);
  
//   // Make more room for X and Y titles
//   // one pad
//   // wmStyle->SetPadRightMargin(0.05);  //percentage
//   // wmStyle->SetPadLeftMargin(0.1);    //percentage
//   // wmStyle->SetPadBottomMargin(0.12); //percentage
  
//   // six sub-pads
//   wmStyle->SetPadRightMargin(0.16);  //percentage
//   wmStyle->SetPadLeftMargin(0.2);    //percentage
//   wmStyle->SetPadBottomMargin(0.14); //percentage
  
//   //----------- Histogram
  
//   // Histos
//   wmStyle->SetHistLineWidth(1);
//   wmStyle->SetMarkerStyle(20);
  
//   //  FILL CONTOURS LINE BAR 
//   //  Frames
//   wmStyle->SetFrameBorderMode(0);
  
//   //  FILL BORDER LINE
//   //  Graphs
//   //  LINE ERRORS
 
//   //---------  Axis 
  
//   wmStyle->SetLabelFont(132,"XYZ"); 
//   wmStyle->SetLabelSize(0.04,"XYZ");
//   wmStyle->SetLabelOffset(0.01 ,"Y");
  
//   //---------  Title
//   wmStyle->SetOptTitle(1);
//   wmStyle->SetTitleStyle(0);
//   wmStyle->SetTitleBorderSize(0);


//   wmStyle->SetTitleSize(0.03,"t");
//   wmStyle->SetRTitleFont(132,"t"); 

//   wmStyle->SetTitleFont(132,"XYZ"); 

//   wmStyle->SetTitleSize(0.05,"XYZ");
  
//   wmStyle->SetTitleOffset(1.0,"XYZ");
  
//   // 6 sub-pads
//   wmStyle->SetTitleOffset(1.6,"Y");
  
//   //----------  Stats
//   wmStyle->SetOptStat(0);
//   wmStyle->SetStatStyle(0);

//   wmStyle->SetOptFit(1);
  
//   //----------  Legend
//   wmStyle->SetLegendBorderSize(0);
//   //wmStyle->SetLegendFont(132);
  
  gROOT->SetStyle("wmStyle");
  gROOT->ForceStyle();
}

// Author: Ben Wade
// s1520134@ed.ac.uk
//
// Designed to take in the waveform of an event 
// (and 2 floats for the values)
// and fit a Gaussian peak with an exponential 
// tailoff (ie: Crystalball fit), then use this 
// to calculate the rise and fall time of the 
// signals and assign them to the variables taken in

void PMTAnalyser::RiseFallTime(int totPulses = 10,
			       float peakMean = 65.){
  
  //Making the canvas for the plots
  TCanvas * can = new TCanvas;
  Float_t w = 1000., h = 500.;
  can->SetWindowSize(w,h);

  TH1F * Rise = new TH1F("Rise",
			 "Pulse Rise Time;Rise Time (ns);Counts",
			 100, 0.0, 10.);
  

  TH1F * Fall = new TH1F("Fall", 
			 "Pulse Fall Time;Fall Time (ns);Counts",
			 200, 7.5, 27.5);

  if(Run >= 70 ){
    Rise->SetBins(64,11.5,21.5);
    Fall->SetBins(64,11.5,21.5);
  }
  
  Long64_t nentries  = rawRootTree->GetEntriesFast();
  Long64_t entry     = 0;
  int      nPulses   = 0;    // counter
  
  float    thresh_mV_low  = 15. ;
  
  if( Test == 'G' )
    thresh_mV_low += 5.0*(HVStep-3.);
  
  float    thresh_mV_high = thresh_mV_low + 20.;
  
  if(Run >= 70 )
    thresh_mV_high = 2000;
  
  float    range_mV = 0.;
  
  int      baseline = 0;
  
  cout << endl;
  cout << " Rise and Fall time analysis " << endl;
  cout << " Analysing " << totPulses << " pulses " << endl;

  TH1F * hWave = new TH1F("hWave","Waveform;Time (ns); Voltage (ADC counts)",
			  NSamples, 0., waveformDuration);
  
  int maxADC  = -100000, minADC  = 100000;
  int peakADC = 0, floorADC  = 0;
  float peakT    = -1000;

  TString fName = "";

  if(negPulsePol)
    fName = "[0]-crystalball(1)";
  else 
    fName = "[0]+crystalball(1)";

  TF1* fWave = new TF1("fWave",fName,
		       0, waveformDuration);
  
  cout << endl;
  cout << " peakMean = " << peakMean << endl;

  // fit waveforms with crystal ball fit
  while ( nPulses < totPulses ){ 
  
    // find random entry number within event sample
    entry = (Long64_t)round(rand()*nentries/RAND_MAX); 

    //hWave->GetXaxis()->SetRange(0,waveformDuration);
    
    // get histogram of waveform
    Get_hWave(entry,hWave);
    
    maxADC = hWave->GetMaximum();
    minADC = hWave->GetMinimum();

    // use full waveform to calculate baseline
    if( Run >= 70 )
      baseline = Get_baseline_ADC(0,entry);
    else
      baseline = Get_baseline_ADC(1,entry);
    
    if(negPulsePol){      
      peakADC  = hWave->GetMinimum();
      floorADC = hWave->GetMaximum();
    }
    else{
      peakADC  = hWave->GetMaximum();
      floorADC = hWave->GetMinimum();
    }
    
    // vito pulses with ringing above threshold 
    if( TMath::Abs( floorADC - baseline )*mVPerBin > thresh_mV_low )
      continue;
    
//     cout << endl;
//     cout << " Passes pulse vito " <<  endl;

    range_mV = (maxADC - minADC)*mVPerBin;
    
    // pulse amplitude range
    if(range_mV  < thresh_mV_low ||
       range_mV  > thresh_mV_high )
      continue;
    
//     cout << endl;
//     cout << " Passes range vito " <<  endl;
    
    if(negPulsePol)
      peakT = hWave->GetBinCenter(hWave->GetMinimumBin());
    else 
      peakT = hWave->GetBinCenter(hWave->GetMaximumBin());
    
    // insist on pulse being synched with LED trigger
    // +/- one sigma acceptance
    // Not Applied to Dark Count data
    if( TMath::Abs(peakT - peakMean) > 8. &&
	Test !='D' )
      continue;
    
//     cout << endl;
//     cout << " Passes timing vito " << endl;

    // end of vitos
    // pulse has been selected

    nPulses++;    
    
    cout << endl;
    cout << " Fitting pulse number " << nPulses <<  endl;
	
    // Base, Const, Mean, Sigma, Alpha, N
    fWave->SetParameters(floorADC,10,peakT,10,-10,10);
    
    if(Run >= 70)
      fWave->SetParLimits(1, 0, 10000);
    else
      fWave->SetParLimits(1, 0, 1000);
    
    fWave->SetParLimits(2, 0, waveformDuration);
    fWave->SetParLimits(4, -50, 0);
    
    hWave->Draw();
    hWave->Fit("fWave", "QR"); 

//     if(Run == 70 )
//       hWave->GetXaxis()->SetRange(0,waveformDuration*1./3);
//     if(Run == 71 )
//       hWave->GetXaxis()->SetRange(waveformDuration*2/3,waveformDuration);

    // Determine rise and fall times
    // from function fit
    float fPeak = 0., fFloor = 0.;
    
    if(negPulsePol){
      fPeak  = fWave->GetMinimum(0,waveformDuration);
      fFloor = fWave->GetMaximum(0,waveformDuration);

    }
    else{
      fPeak  = fWave->GetMaximum(0,waveformDuration);
      fFloor = fWave->GetMinimum(0,waveformDuration);
    }
    
    float fPeakT = fWave->GetX(fPeak,0,waveformDuration);    

    float fBase = fWave->GetParameter(0);
    
    float fFull = fBase;
    float f090  = fBase;
    float f010  = fBase;
          
    if(negPulsePol){
      fFull = fBase - fPeak;
      f090  -= 0.9*fFull;
      f010  -= 0.1*fFull;
    }
    else{
      fFull  = fPeak - fBase;
      f090  += 0.9*fFull;
      f010  += 0.1*fFull;
    }
    
    // range in X to search for y values 
    float fXAtPeak = fWave->GetX(fPeak,0,waveformDuration);
    float fPreMin  = fXAtPeak - 10.; 
    
    if(Run >= 70)
      fPreMin = fXAtPeak - 25.;

    float fPostMin = fXAtPeak + 25;
    
    float timeRise10 = fWave->GetX(f010,fPreMin,fXAtPeak);
    float timeRise90 = fWave->GetX(f090,fPreMin,fXAtPeak);
    
    float timeFall10 = fWave->GetX(f010,fXAtPeak,fPostMin);
    float timeFall90 = fWave->GetX(f090,fXAtPeak,fPostMin);
    
    float riseTime   = timeRise90 - timeRise10;
    float fallTime   = timeFall10 - timeFall90;
    
//     cout << endl;
//     cout << " fPeak      = " << fPeak      << endl;
//     cout << " fBase      = " << fBase      << endl;
//     cout << " fFull      = " << fFull      << endl;
//     cout << " f090       = " << f090       << endl;
//     cout << " f010       = " << f010       << endl;
//     cout << " fXAtPeak   = " << fXAtPeak   << endl;
//     cout << " fPreMin    = " << fPreMin    << endl;
//     cout << " fPostMin   = " << fPostMin   << endl;
//     cout << " timeRise10 = " << timeRise10 << endl;
//     cout << " timeRise90 = " << timeRise90 << endl;
//     cout << " timeFall10 = " << timeFall10 << endl;
//     cout << " timeFall90 = " << timeFall90 << endl;
//     cout << " fallTime   = " << fallTime   << endl;
//     cout << " riseTime   = " << riseTime   << endl;

    // save waveform fits
    bool doPlot = false; 
    int  oneIn = 10; // of waveforms saved 
    if(nPulses%(oneIn) == 0 && doPlot){

      // Horizontal lines
      TLine * lBase = new TLine(0,fBase,waveformDuration,fBase);
      lBase->SetLineColor(kBlue);
      lBase->Draw();

      TLine * l10 = new TLine(0,f010,waveformDuration,f010);
      l10->SetLineColor(kGreen);
      l10->Draw();
      
      TLine * l90 = new TLine(0,f090,waveformDuration,f090);
      l90->SetLineColor(kOrange);
      l90->Draw();
      
      TLine * lPeak = new TLine(0,fPeak,waveformDuration,fPeak);
      lPeak->SetLineColor(kMagenta);
      lPeak->Draw();
      
      // Vertical lines
      TLine * lRise10 = new TLine(timeRise10,fFloor,timeRise10,fPeak);
      lRise10->SetLineColor(kRed);
      lRise10->Draw();
      
      TLine * lRise90 = new TLine(timeRise90,fFloor,timeRise90,fPeak);
      lRise90->SetLineColor(kRed);
      lRise90->Draw();
      
      if( Run >= 70 ){
	TLine * l100 = new TLine(fPeakT,fPeak,fPeakT,fPeak);
	lRise90->SetLineColor(kMagenta);
	lRise90->Draw();
      }
      
      TLine * lFall90 = new TLine(timeFall90,fFloor,timeFall90,fPeak);
      lFall90->SetLineColor(kBlue);
      lFall90->Draw();

      TLine * lFall10 = new TLine(timeFall10,fFloor,timeFall10,fPeak);
      lFall10->SetLineColor(kBlue);
      lFall10->Draw();
      
      char OutFile[128];
      if(Test=='G')
	sprintf(OutFile, "./WaveformFits/Waveform_Run_%d_entry_%lld_HV_%d.png",Run,entry,HVStep);
      else
	sprintf(OutFile, "./WaveformFits/Waveform_Run_%d_entry_%lld_Test_%c.png",Run,entry,Test);
      can->SaveAs(OutFile);
    }
    
    Rise->Fill(riseTime);
    Fall->Fill(fallTime);
  }
  
  hWave->Delete();   
  
  Rise->Draw();
  
  TString hName = FileID;
  hName = hName + ".png";
 
  //Rise->Fit("gaus");
  can->SaveAs("./RiseFall/Rise_" + hName);
  
  Fall->Draw();
  //Fall->Fit("gaus");
  can->SaveAs("./RiseFall/Fall_" + hName);
  
}
