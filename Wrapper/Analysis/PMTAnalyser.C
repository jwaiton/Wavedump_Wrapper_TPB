#define PMTAnalyser_cxx
#include "PMTAnalyser.h"
#include <TH2.h>
#include <TF1.h>
#include <TStyle.h>
#include "TBranch.h"
#include "TLatex.h"
#include "TROOT.h"

#include <stdlib.h>

#include "../BinaryConversion/wmStyle.C"

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
  
  //   //!!!
  //nentries = 10000;
  
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

  TH1F *  hWave;
  TString hName;
  hWave = Get_hWave(entry);
  hName = "hWave";
  
  hWave->SetNameTitle(hName,
		      "Waveform;Time (ns); Voltage (ADC counts)");
    
  
  Float_t lineXMin = 0.;
  Float_t lineXMax = 35.;

  Float_t lineYMin = Get_baseline_ADC(entry);
 
  Float_t lineYMax = lineYMin;

  TLine *baselineLine = new TLine(lineXMin,lineYMin,
				  lineXMax,lineYMax); 
  
  Float_t lineXLimit = hWave->GetXaxis()->GetXmax();
  
  TLine *bigLine = new TLine(lineXMax,lineYMin,
			     lineXLimit,lineYMax); 
  
  baselineLine->SetLineColor(kBlue);
  baselineLine->SetLineWidth(4.0);

  bigLine->SetLineColor(kRed);
  bigLine->SetLineWidth(4.0);
  
  TString tStr = "";
  TLatex * latex = new TLatex();

  cout << endl;
  cout << " entry = " << entry << endl;
    
  //hWave->SetMinimum(450);
  //hWave->SetMaximum(850);
  
  // if(digitiser=='D')
  //     hWave->GetXaxis()->SetRange(1,500);
      
  //hWave->GetXaxis()->SetRange(64,192);
    
  hWave->Draw();
  baselineLine->Draw();
  bigLine->Draw();
  
  tStr.Form("Entry %lld", entry);
  latex->DrawLatex(0.6,0.8,tStr);

  cout << endl;
  cout << " Plotting Waveform" << endl;
  
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
  
  TH1F * hWave = Get_hWave(entry); 

  hWave->FFT(hFFT ,"MAG");
 
  hWave->Delete();
    
  Float_t dx = nsPerSample * 1.E-9;
  
  // convert to frequency bins
  hFFT = FFTShift(hFFT,dx);
  
  return hFFT;
}

TH1F * PMTAnalyser::Get_hWave(Long64_t entry){
  
  TH1F *  hWave = nullptr;
  hWave = new TH1F("hWave","Waveform;Time (ns); Voltage (ADC counts)",
		   NSamples, 0., waveformDuration);
  
  LoadTree(entry);
  rawRootTree->GetEntry(entry);  
  
  for( int iSample = 0 ; iSample < NSamples; iSample++)
    hWave->SetBinContent(iSample+1,(waveform[iSample]));

  //hWave->SetBinContent(iSample+1,(aoff - waveform[iSample]));
 
  return hWave;
}

// Short_t * PMTAnalyser::Get_waveform(Long64_t entry){
  
//   rawRootTree->GetEntry(entry);
  
//   return waveform;
// }

Short_t PMTAnalyser::Get_baseline_ADC(Long64_t entry){
  
  rawRootTree->GetEntry(entry);
  
  Int_t baseline_ADC = 0;
  Float_t time_ns = 0.;
  Int_t   samplesInBaseline = 0;  

  for( int iSample = 0 ; iSample < NSamples; iSample++){
    
    time_ns = (Float_t)iSample * nsPerSample;
    
    if ( time_ns  < 35.){
      baseline_ADC += waveform[iSample];
      samplesInBaseline++;
    }
    
  }
  baseline_ADC = baseline_ADC/samplesInBaseline;
  
  return (Short_t)baseline_ADC;
}

Float_t PMTAnalyser::Get_baseline_mV(Short_t waveform[],
 				     Float_t peakT_ns){
  int   samplesInBaseline = 0;  
  
  Float_t baseline_mV = 0.0;
  Float_t time_ns     = 0.;
  Float_t voltage_mV = 0.;
  
  for( int iSample = 0 ; iSample < NSamples; iSample++){
    time_ns    = iSample * nsPerSample;
    voltage_mV = waveform[iSample] * mVPerBin;

    if( peakT_ns >= 50.0 ) {
      
      // leave at least 15 ns for rise 
      if( time_ns < 35.0 ){
	baseline_mV += voltage_mV;
	samplesInBaseline++;
      }
      
    }
    else { 
      // if peak before signal region
      if( time_ns >= 150.0 && 
	  time_ns <  185.0 ){
	baseline_mV += voltage_mV;
	samplesInBaseline++;
      }
      
    }
    
  } // end of: for( int iSample = 0 ; iSample < N......
  
  baseline_mV = baseline_mV / samplesInBaseline; 
  
  return baseline_mV;
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

Float_t PMTAnalyser::TimeOfPeak()
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
  int   binsT  = NSamples;
  
  Float_t rangeV[2];
  rangeV[0] = -12.;
  rangeV[1] = 110.;

  int binsV = (int)NVDCBins/4;
    
  binsV = binsV * (int)(rangeV[1] - rangeV[0])/1000/VoltageRange;
  
  if( Test=='A' ){
    rangeT = 220.;
    binsT = 110;
  }

  TH1F * hPeakT_ns_1 = new TH1F("hPeakT_ns_1",
				"time of peak voltage; Time (ns);Counts",
				binsT,0.,rangeT);
  
  TH1F * hPeakT_ns_2 = new TH1F("hPeakT_ns_2",
				"peak time (randomise repeat); Time (ns);Counts",
				binsT,0.,rangeT);
  
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
  
  for (Long64_t jentry = 0; jentry < nentries; jentry++) {
    
    ientry = LoadTree(jentry);
    if (ientry < 0) break;
    rawRootTree->GetEntry(jentry);   
 
    if(verbosity > 1){
      cout << endl;
      cout << " entry = " << jentry << endl;
    }
    
    //------------------------------------
    // event-by-event baseline

    baseline_mV = Get_baseline_mV(waveform,
				  peakT_ns);
    
    //------------------------------------
    
    // BinToRoot method
    hPeakT_ns_1->Fill(peakT_ns);
    
    //------------------------------------
    // Randomise peak time from peak
    // voltage duplicates 
    
    // WARNING only implemented for negative pulses
    peakSample = Select_peakSample(waveform,
				   minVDC);
    
    peakT_ns   = peakSample * nsPerSample;

    //--------------------------------------
    //--------------------------------------
    
    hPeakT_ns_2->Fill(peakT_ns);
    
    peakV_mV = waveform[peakSample] * mVPerBin;
    peakV_mV = baseline_mV - peakV_mV; 
  
    hPeakT_PeakV->Fill(peakT_ns,peakV_mV);
    hPeakV_mV->Fill(peakV_mV);

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

  double maxBin = hPeakT_ns_2->GetMaximumBin();
  
  double maxPeakT_ns = hPeakT_ns_2->GetXaxis()->GetBinCenter(maxBin);
  
  if(Test!='D'){
    hPeakT_ns_2->Fit("gaus","Q","", 
		     maxPeakT_ns - rangeFit, 
		     maxPeakT_ns + rangeFit );
    
    
    fPeakTimeGaus = hPeakT_ns_2->GetFunction("gaus");  
    
    // fit again based on first results
    // 2 sigma range (95 %)   
    maxPeakT_ns = fPeakTimeGaus->GetParameter(1);
    rangeFit = fPeakTimeGaus->GetParameter(2) * 2.0;
    
    hPeakT_ns_2->Fit("gaus","Q","", 
		     maxPeakT_ns - rangeFit, 
		     maxPeakT_ns + rangeFit );
    
    fPeakTimeGaus = hPeakT_ns_2->GetFunction("gaus");  
    
    hPeakT_ns_2->GetFunction("gaus")->SetLineColor(kBlue);
    
    peakMean = fPeakTimeGaus->GetParameter(1);
    
  }
  
  hPeakT_ns_1->SetMinimum(0);
    
  hPeakT_ns_1->Draw();

  hPeakT_ns_2->SetMinimum(0);
  
  hPeakT_ns_1->SetLineColor(kBlue);
  
  Int_t lowBin = maxBin*rangeT/binsT - rangeFit*3;
  Int_t highBin = maxBin*rangeT/binsT + rangeFit*3;

  hPeakT_ns_2->GetXaxis()->SetRange(lowBin, highBin);
  hPeakT_ns_2->Draw();
  
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
  
  TString hName = "./Plots/PeakTime_";
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
//   wmStyle->SetTitleFont(132,"t"); 

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
