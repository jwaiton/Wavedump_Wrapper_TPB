#define PMTAnalyser_cxx
#include "PMTAnalyser.h"
#include <TH2.h>
#include <TF1.h>
#include <TStyle.h>
#include "TBranch.h"
#include "TLatex.h"
#include "TROOT.h"
#include "TRandom3.h"

#include "TApplication.h"

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
  
  for (Long64_t jentry = 0; jentry < nentries; jentry++) {
    
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
	cout << " sample = " << iSample << endl;
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
    
  TH1F* hFFT_Shift=new TH1F("hFFT_Shift",
			  label,
			  newN, 0.,df*newN); 
  

  for (int qq=1; qq<=newN; qq++) 
    hFFT_Shift->SetBinContent(qq,hFFT->GetBinContent(qq)); 
  
  return hFFT_Shift; 
} 

void PMTAnalyser::PlotWaveform(Int_t firstEntry){
  
  TCanvas * canvas = new TCanvas();
  
  Float_t w = 2000., h = 500.;
  //w = w + canvas->GetWw();
  //h = h + canvas->GetWh();
  canvas->SetWindowSize(w,h);

  static const int nWaveforms = 1;
  
  TH1F *  hWave[nWaveforms];
  TH1F *  hWaveFFT[nWaveforms];
  TString hName;

  TString hNameTemp = "hWave_%d";
  
  for (int i = 0 ; i < nWaveforms ; i++){
    hNameTemp = "hWave_%d";
    hName.Form(hNameTemp,i);
    hWave[i] = new TH1F(hName,"Waveform;Time (ns); Voltage (ADC counts)",
			NSamples, 0., waveformDuration);
    
    hNameTemp = "hWaveFFT_%d";
    hName.Form(hNameTemp,i);
    hWaveFFT[i]= new TH1F(hName,"hWaveFFT",
			  NSamples, 0, NSamples);
  
  }


  canvas->Divide(2,nWaveforms);
  
  TLatex * latex = new TLatex();
  latex->SetNDC();
  latex->SetTextSize(0.025);
  latex->SetTextAlign(12);  //align at top
  
  TString tStr = " %d entry ";
  
  float aoff = 8700.;  
  
  Long64_t entry    = firstEntry;
  int entryRelFrst = 0;

  Long64_t nFileEntries = rawRootTree->GetEntriesFast();   
  
  if( entry > (nFileEntries-1) ){
    cout << endl;
    cout << " Last entry in file is " << (nFileEntries-1) << endl;
    return;
  }
  
  int canNo = 0;
  while ( entryRelFrst < nWaveforms && 
	  firstEntry != -1 ){

    cout << endl;
    cout << " entry = " << entry << endl;
    
    entryRelFrst = entry-firstEntry;

    canNo++;
    canvas->cd(canNo);
    
    LoadTree(entry);
    rawRootTree->GetEntry(entry);  

    for( int iSample = 0 ; iSample < NSamples; iSample++){
      hWave[entryRelFrst]->SetBinContent(iSample+1,(aoff - waveform[iSample]));
      
    }
    
    //hWave[entryRelFrst]->SetMinimum(450);
    //hWave[entryRelFrst]->SetMaximum(850);
    hWave[entryRelFrst]->Draw();
    
    tStr.Form("Entry %lld", entry);
    latex->DrawLatex(0.6,0.8,tStr);
    
    canNo++;
    canvas->cd(canNo);
        
    hWaveFFT[entryRelFrst]->Reset();

    hWave[entryRelFrst]->FFT(hWaveFFT[entryRelFrst] ,"MAG");
    
    Float_t dx = nsPerSample * 1.E-9;
    
    // convert to frequency bins
    hWaveFFT[entryRelFrst] = FFTShift(hWaveFFT[entryRelFrst],dx);

    gPad->SetLogy();
    hWaveFFT[entryRelFrst]->Draw();
      
    entry++;
    entryRelFrst++;
  }
  
  if(firstEntry==-1)
    return;
  
  hNameTemp = "./WaveForms/";
  
  hNameTemp += FileID;
  //hNameTemp += "_Event_%d";
  
  // current
  hNameTemp += "_Event_C";
  hNameTemp += ".png";
  
  //hName.Form(hNameTemp,entry);
  hName = hNameTemp;
  canvas->SaveAs(hName);

  for (int i = 0 ; i < nWaveforms ; i++){
    hWave[i]->Delete();
    hWaveFFT[i]->Delete();
  }    
  
  
}

void PMTAnalyser::TimeOfPeak()
{
  if (rawRootTree == 0) return;
  
  int verbosity = 0;
  
  Long64_t ientry;
  Long64_t nentries = rawRootTree->GetEntriesFast();
  
  if(testMode)
    nentries = GetNEntriesTest(verbosity,nentries);

  cout << endl;
  cout << " Running: Time of Peak " << endl;
  cout << " " << nentries << " entries " << endl;

  float voltage_mV = 0.;
  float time_ns = 0.;

  // Count peaks
  int   nPeaks = 0;
  int   peakSampleArray[NSamples]; 
  int   peakSample = -1; // minT (take 2)
  int   peakNumberChoice = -1;
  int   samplesInBaseline = 0;
  float baseline_mV = 0.0;
  
  TCanvas * canvas = new TCanvas();
  
  float rangeT = waveformDuration;
  int   binsT  = NSamples;
  

  float rangeV[2];
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
				 
  
  TRandom3 * random = new TRandom3(); 
      
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
    
    samplesInBaseline = 0;
    baseline_mV = 0.0;
    
    if( verbosity > 1 ){
      cout << " peakT_ns = " << peakT_ns << endl;
    }
    
    for( int iSample = 0 ; iSample < NSamples; iSample++){
      
      time_ns    = iSample * nsPerSample;
      voltage_mV = waveform[iSample] * mVPerBin;
      
      // Which of two regions was peak in?
      // use other region for baseline
      if( peakT_ns >= 50.0 ) {
	
	// exclude 35 - 50 ns 
	// in case peak is at 50 ns
	// ~ 10 ns rise time
	if( time_ns < 35.0 ){
	  baseline_mV += voltage_mV;
	  samplesInBaseline++;
	}
	
      }
      else { 
	// peak < 50 ns 
	// leave at least 100 ns 
	// after peak 
	if( time_ns >= 150.0 && 
	    time_ns <  185.0 ){
	  baseline_mV += voltage_mV;
	  samplesInBaseline++;
	}
	
      }
      
      if( verbosity > 2 ){
	cout << endl;
	cout << " samplesInBaseline = " << samplesInBaseline << endl;
	cout << " voltage_mV        = " << voltage_mV        << endl;
	cout << " baseline_mV       = " << baseline_mV       << endl;
      }
      
    } // end of: for( int iSample = 0 ; iSample < N......
    
    baseline_mV = baseline_mV / samplesInBaseline; 
    
    if( verbosity > 1 ){
      cout << endl;
      cout << " baseline_mV       = " << baseline_mV       << endl;
    }
    
    // event-by-event baseline
    //------------------------------------
    
    //------------------------------------
    // Find peaks
    nPeaks = 0;
    peakNumberChoice = -1;
    peakSample = -1;

    for( int iSample = 0 ; iSample < NSamples; iSample++){
      peakSampleArray[iSample] = 0;
      
      time_ns    = iSample * nsPerSample;
      voltage_mV = waveform[iSample] * mVPerBin;
      voltage_mV = voltage_mV - baseline_mV;
      
      // count peaks
      if( waveform[iSample] == minVDC ){
	peakSampleArray[nPeaks] = iSample;
	nPeaks++;
      }
      
      if( verbosity > 2){
	cout << endl;
	cout << " waveform[" << iSample << "] = " << waveform[iSample] << endl; 
	cout << " voltage = " << voltage_mV << " mV " << endl;
      }
    
    } // end of: for( int iSample = 0 ; iSample < N......
    
    // Find peaks
    //------------------------------------
    
    if( verbosity > 1 ){
      if( nPeaks > 1 ){
	cout << endl;
	cout << " " <<  nPeaks << " peaks " <<  endl;
	for ( int i = 0 ; i < nPeaks ; i++ )
	cout << " peakSampleArray[" << i << "] = " 
	     << peakSampleArray[i] << endl;
      }
    }
    // BinToRoot method
    hPeakT_ns_1->Fill(peakT_ns);
    
    // New Method
    
    // randomly assign integer from [0,(nPeaks-1)]
    peakNumberChoice = (Int_t)random->Uniform(nPeaks);
      
    peakSample = peakSampleArray[peakNumberChoice];
    
    peakT_ns = peakSample * nsPerSample;
    
    peakV_mV = waveform[peakSample] * mVPerBin;
    peakV_mV = baseline_mV - peakV_mV; 
    
    hPeakT_ns_2->Fill(peakT_ns);
    
    if(nPeaks == 0)
      cerr << " no peaks found is...... impossible " << endl;
   
    hPeakT_PeakV->Fill(peakT_ns,peakV_mV);
    hPeakV_mV->Fill(peakV_mV);

    if(verbosity > 1){    
      cout << endl;
      cout << " nPeaks            = " << nPeaks           << endl;
      
      if(nPeaks > 1 ){
	cout << " peakNumberChoice  = " << peakNumberChoice << endl;
	cout << " Peak Sample       = " << peakSample       << endl;;
      }
      cout << " peakT_ns          = " << peakT_ns         << endl;;
      cout << " peakV_mV          = " << peakV_mV         << endl;;
      cout << " minVDC            = " << minVDC           << endl;
      cout << " minT              = " << minT             << endl;
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
  
  if(Test!='D'){
    
    double maxPeakT_ns = hPeakT_ns_2->GetXaxis()->GetBinCenter(hPeakT_ns_2->GetMaximumBin());
  
    double rangeFit = 8.5;
    
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
  }
  
  hPeakT_ns_1->SetMinimum(0);
  hPeakT_ns_1->Draw();
  
  TLatex * latex = new TLatex();
  latex->SetNDC();
  latex->SetTextSize(0.025);
  latex->SetTextAlign(12);  //align at top
  
  TString tStr = " %d entries ";
  
  tStr.Form("Gaus Mean = %.2f  ",
	    fPeakTimeGaus->GetParameter(1));
   
  latex->SetTextColor(kBlue);  

  latex->DrawLatex(0.6,0.8,tStr);
  
  tStr.Form("Gaus Sigma = %.2f  ",
	    fPeakTimeGaus->GetParameter(2));
  
  latex->DrawLatex(0.6,0.75,tStr);

  hPeakT_ns_2->SetMinimum(0);
  hPeakT_ns_2->SetLineColor(kBlue);
  hPeakT_ns_2->Draw("same");
  
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
  
  return;
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
  
  TH1F * hWaveFFT  = new TH1F("hWaveFFT","hWaveFFT",
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

    hWaveFFT->Reset();
    hWave->FFT(hWaveFFT ,"MAG");
    
    ///------------------
    //  Do the filtering
    //if(IsCleanFFTWaveform(hWaveFFT))
    // do someit
    ///-------------------
    
  } //end of: for (Long64_t jentry = 0; jentry

  // Draw plots and append to root file
  
  return 1;
}


Bool_t PMTAnalyser::IsCleanFFTWaveform(TH1F * hWaveFFT){

  // delete zero frequency data
  hWaveFFT->SetBinContent(1,0.);
  
  if(hWaveFFT->GetMaximumBin() == 2)
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
  
  TH1F* hWaveFFT = new TH1F("hWaveFFT","hWaveFFT;",
			    NSamples, 0, NSamples);
  
  TH1F* hWaveFFT_MaxBin  = new TH1F("hWaveFFT_MaxBin",
				    "hWaveFFT_MaxBin",
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
    
    hWaveFFT->Reset();

    hWave->FFT(hWaveFFT ,"MAG");
    
    hWaveFFT->SetBinContent(1,0.);

    // Store maximum FFT bin 
    hWaveFFT_MaxBin->Fill(hWaveFFT->GetMaximumBin());
    
    if(hWaveFFT->GetMaximumBin() == 2 )
      hMaxADC_Filtered->Fill( hWave->GetMaximum());
    
    fftoff = hWaveFFT->GetBinContent(1)/(double)NSamples; 
    
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
   
   hWaveFFT_MaxBin->SetLineColor(kBlue);  
   hWaveFFT_MaxBin->Draw();
   
   canvas->cd(3);  
   
   hWaveFFT->Draw();

   canvas->cd(4);  

   return canvas;
}

void PMTAnalyser::SetTestMode(Bool_t userTestMode){
  testMode = userTestMode;
}


void PMTAnalyser::SetStyle(){
  
  TStyle     *watchStyle  = new TStyle("watchStyle",
				       "My Root Styles");
  
  const Int_t NCont = 255;
  const Int_t NRGBs = 5;
  
  // Color scheme for 2D plotting with a better defined scale 
  Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };          
  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  
  watchStyle->SetNumberContours(NCont);
  
  // General
  // OPTIONS - FILL LINE TEXT MARKER
  
  watchStyle->SetFillColor(0);
  watchStyle->SetTextSize(0.05);
  
  //-----------  Canvas
  
  watchStyle->SetCanvasBorderMode(0);
  watchStyle->SetCanvasColor(kWhite);
  
  //------------- Pad
  
  watchStyle->SetPadBorderMode(0); 
  watchStyle->SetPadColor(kWhite);
  
  // Make more room for X and Y titles
  // one pad
  // watchStyle->SetPadRightMargin(0.05);  //percentage
  // watchStyle->SetPadLeftMargin(0.1);    //percentage
  // watchStyle->SetPadBottomMargin(0.12); //percentage
  
  // six sub-pads
  watchStyle->SetPadRightMargin(0.16);  //percentage
  watchStyle->SetPadLeftMargin(0.2);    //percentage
  watchStyle->SetPadBottomMargin(0.14); //percentage
  
  //----------- Histogram
  
  // Histos
  watchStyle->SetHistLineWidth(1);
  watchStyle->SetMarkerStyle(20);
  
  //  FILL CONTOURS LINE BAR 
  //  Frames
  watchStyle->SetFrameBorderMode(0);
  
  //  FILL BORDER LINE
  //  Graphs
  //  LINE ERRORS
  
  //---------  Axis 
  
  watchStyle->SetLabelFont(132,"XYZ"); 
  watchStyle->SetLabelSize(0.04,"XYZ");
  watchStyle->SetLabelOffset(0.01 ,"Y");
  
  //---------  Title
  watchStyle->SetOptTitle(1);
  watchStyle->SetTitleStyle(0);
  watchStyle->SetTitleBorderSize(0);


  watchStyle->SetTitleSize(0.03,"t");
  watchStyle->SetTitleFont(132,"t"); 

  watchStyle->SetTitleFont(132,"XYZ"); 

  watchStyle->SetTitleSize(0.05,"XYZ");
  
  watchStyle->SetTitleOffset(1.0,"XYZ");
  
  // 6 sub-pads
  watchStyle->SetTitleOffset(1.6,"Y");
  
  //----------  Stats
  watchStyle->SetOptStat(0);
  watchStyle->SetStatStyle(0);

  watchStyle->SetOptFit(1);
  
  //----------  Legend
  watchStyle->SetLegendBorderSize(0);
  //watchStyle->SetLegendFont(132);
  
  gROOT->SetStyle("watchStyle");
  gROOT->ForceStyle();
}
