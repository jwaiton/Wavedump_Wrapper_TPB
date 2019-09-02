#define TConvert_cxx
#include "TConvert.h"
#include <TH2.h>
#include <math.h>
//#include <climits>
#include <limits.h>

#include "../BinToRoot/wmStyle.C"

void TConvert::Noise(){
  
  if (fChain == 0) return;
  
  float min_mV = -(float)fRange_V/0.002;
  float max_mV = (float)fRange_V/0.002;
  float mVperBin = f_mvPerBin;
  int   nBins = 0;
  
  // fix binning and set number of bins
  Set_THF_Params(&min_mV,&max_mV,&mVperBin,&nBins);
  
  TH1F * hMean_mV =  new TH1F("hMean_mV",
			      "; mean voltage (mV); Counts",
			      nBins,min_mV,max_mV);

  // prepare for range starting at zero
  min_mV = 0.0;
  nBins  = 0;
  Set_THF_Params(&min_mV,&max_mV,&mVperBin,&nBins);

  TH1F * hPPV =  new TH1F("hPPV",
			  "; peak to peak voltage (mV); Counts",
			  nBins,min_mV,max_mV);

  TH1F * hPeak =  new TH1F("hPeak",
			  "; peak voltage (mV); Counts",
			   nBins,min_mV,max_mV);
  
  
  // prepare for range starting at zero
  // and 2D plotting
  
  // reduce resolution
  mVperBin = f_mvPerBin/4.;
  min_mV = 0.0;
  nBins  = 0;
  Set_THF_Params(&min_mV,&max_mV,&mVperBin,&nBins);
  
  TH2F * hPPV_Peak =  new TH2F("hPPV_Peak",
			       "; peak to peak voltage (mV); peak voltage",
			       nBins,min_mV,max_mV,
			       nBins,min_mV,max_mV);


  double mean_mV = 0.;
  float  wave_mV = 0.;
  float  ppV     = 0.;
  
  for (int iEntry = 0; iEntry < nentries; iEntry++) {
    fChain->GetEntry(iEntry);

    min_mV = 1000.; 
    max_mV = -1000.;
    
    //wave->clear();
    
    for (short i = 0; i < fNSamples; ++i){
      wave_mV  = (float)ADC->at(i);
      wave_mV *= f_mvPerBin;
      wave_mV -= 1000.;

      if(fPulsePol=='N')
	wave_mV = -wave_mV;
	  
      if(wave_mV > max_mV)
	max_mV = wave_mV;

      if(wave_mV < min_mV)
	min_mV = wave_mV;

      mean_mV += (double)wave_mV;
      //wave->push_back(wave_mV);
    
    }

    ppV = max_mV - min_mV;
    
    mean_mV = mean_mV/fNSamples;
    
    //printf(" \n mean_mV = %f \n", mean_mV);
    
    hMean_mV->Fill(mean_mV);
    hPPV->Fill(ppV);
    
    hPeak->Fill(max_mV);
    
    hPPV_Peak->Fill(ppV,max_mV);
    // if(iEntry >= 10000)
//       break;
    
  }
  
  std::string outFolder = "./Plots/Noise/";
  std::string outName = outFolder + "hMean_mV.pdf";
  
  gPad->SetLogy();
  
  hMean_mV->SetAxisRange(-100., 100.,"X");
  hMean_mV->SetMinimum(0.1);
  hMean_mV->Draw();
  
  canvas->SaveAs(outName.c_str());

  hPPV->SetAxisRange(-0., 250.,"X");
  hPPV->SetMinimum(0.1);
  hPPV->Draw();
  
  outName = outFolder + "hPPV.pdf";
  canvas->SaveAs(outName.c_str());

  hPeak->SetAxisRange(-0., 250.,"X");
  hPeak->SetMinimum(0.1);
  hPeak->Draw();
  outName = outFolder + "hPeak.pdf";
  canvas->SaveAs(outName.c_str());
  
  gPad->SetLogy(false);
  gPad->SetLogz();
  
  hPPV_Peak->SetAxisRange(-0., 50.,"X");
  hPPV_Peak->SetAxisRange(-0., 50.,"Y");
  
  hPPV_Peak->Draw("colz");
  
  outName = outFolder + "hPPV_Peak.pdf";
  canvas->SaveAs(outName.c_str());

}

void TConvert::ADC_Loop(){
  if (fChain == 0) return;
  
  short nBaseSamps   = 0;
  short baseLowSamp  = 0; 
  short baseHighSamp = 50/(short)f_nsPerSamp;
  
  float baseline = 0;

  for (int iEntry = 0; iEntry < nentries; iEntry++) {
    fChain->GetEntry(iEntry);

    printf("\n ------- \n iEntry %d \n", iEntry);
    
    nBaseSamps = 0;
    baseline = 0;
    // baseline
    for (short i = baseLowSamp; i < baseHighSamp; ++i){
      baseline += ADC->at(i);
      nBaseSamps++;
    }
    
    baseline = baseline/nBaseSamps;
    
    // Peak-to-peak voltage
    // peak voltage
    
    // for (uint i = 0; i < ADC.size(); ++i){
//       //ADC->at(i) = 
//     }


    //PrintVec(*ADC);
    
    if(iEntry == 10)
      break;
    
  }
}

void TConvert::GetDAQInfo()
{
  
  printf("\n ------------------------------ \n");
  printf("\n Getting DAQ Info               \n");
  
  PrintConstants();

  if (fChain == 0) return;
  
  BeforeDAQ();

  double time     = 0;
  double prevTime = 0;
  
  int trigCycles    = 0;
  int trigEntry     = 0;
  int prevTrigEntry = 0;
  int dTrigEntry    = 0;
  
  int nRateEvents = nentries/100;

  int deltaEvents = 0;
  double   deltaT      = 0.;
  double   dTime       = 0.; // time between events
  double   eventRate;

  double   meanRate = 0;

  int nbytes = 0, nb = 0;

  for (int iEntry = 0; iEntry < nentries; iEntry++) {
    nb = fChain->GetEntry(iEntry);   nbytes += nb;    
    
    //-----------------------------
    // Process Header Information
    time     = GetElapsedTime(iEntry,&trigCycles,prevTime);
    dTime    = time - prevTime; 
    prevTime = time; // now set for next entry
    
    deltaT   += dTime;   // integrated time
    trigEntry = HEAD[4]; // absolute entry number
    
    hTT_EC->Fill(GetTrigTimeTag(iEntry),trigEntry);

    deltaEvents++; // integrated event count
    dTrigEntry    = trigEntry - prevTrigEntry;
    prevTrigEntry = trigEntry;
    
    // skip first entry for intergrated
    // and differential variable plots
    if( iEntry==0 ) 
      continue;
    
    hTrigFreq->Fill(1./dTime/1000.);  

    meanRate += 1./dTime/1000.;
    
    CountMissedEvents(dTrigEntry); // any unwritten events?

    // process after event integration period
    if(iEntry%nRateEvents == 0 ){
      
      hNEventsTime->Fill(time/60.,iEntry);
      
      eventRate = deltaEvents/deltaT;
      hEventRate->Fill(time/60.,eventRate/1000.);  
    
      // reset integrated variables
      deltaEvents = 0;
      deltaT = 0.;
      
    } // end of: if(iEntry%nRateEvents
    
    // Process Header Information
    //-----------------------------
    
  } // end of: for (int iEntry = 0...

  meanRate = meanRate/nentries;

  printf("\n mean event rate = %.2f kHz \n\n",meanRate);

  AfterDAQ();

  printf("\n ------------------------------ \n");
}

void TConvert::Set_THF_Params(float * minX, 
			      float * maxX,
			      float * binWidth,
			      int   * nBins){
  
  if     (*nBins==0)
    *nBins = (int)roundf((*maxX - *minX)/(*binWidth));
  else if(*nBins > 0 && *binWidth < 1.0E-10)
    *binWidth = (*maxX - *minX)/(float)*nBins;
  else
    fprintf(stderr,"\n Error in Set_THF_Params \n");
  
  *nBins += 1;
  *minX -= 0.5*(*binWidth);
  *maxX += 0.5*(*binWidth);

};

void TConvert::InitCanvas(){

  canvas = new TCanvas();
  float w = 1000., h = 500.;
  canvas->SetWindowSize(w,h);

}


void TConvert::BeforeDAQ(){
  
  nMissedEvents = 0;
  InitHistosDAQ();

}

void TConvert::AfterDAQ(){
  
  SaveHistosDAQ();
  
}

void TConvert::InitHistosDAQ(){
  
  //----
  float    minTime    = 0.0;
  float    maxTime    = 16.0; // minutes
  float    timePerBin = 1./6000.; // 100th of a second per bin
  int nTimeBins  = 0;
    
  Set_THF_Params(&minTime,&maxTime,&timePerBin,&nTimeBins);
  
  hNEventsTime = new TH1F("hNEventsTime",
			  "hNEventsTime;Time (mins);Event",
			  nTimeBins,minTime,maxTime);
  
  hEventRate = new TH1F("hEventRate",
			"hEventRate;Time (mins);Mean Rate (kHz)",
			nTimeBins,minTime,maxTime);

  //----
  float    minFreq    = 0.0; 
  float    maxFreq    = 100.0;    // kHz
  float    freqPerBin = 1./10000; // 1/10 Hz
  int nFreqBins  = 0;
  
  Set_THF_Params(&minFreq,&maxFreq,&freqPerBin,&nFreqBins);
  
  hTrigFreq = new TH1F("hTrigFreq",
		       "hTrigFreq; Rate (kHz); Counts",
		       nFreqBins,minFreq,maxFreq);

  //----
  // hTT_EC
  float minClock   = 0.0;
  float maxClock   = 35.0;
  int   nClockBins = (int)round(UINT_MAX/1000000) + 1; 
  float secsPerClockBin = 0.;
  
  Set_THF_Params(&minClock,&maxClock,&secsPerClockBin,&nClockBins);
  
  fChain->GetEntry(0);
  float firstEntry = (float)HEAD[4];
  
  fChain->GetEntry(nentries-1);
  float lastEntry  = (float)HEAD[4];

  float entriesPerBin = 1000.;
  int   nEntryBins    = 0;

  Set_THF_Params(&firstEntry,&lastEntry,&entriesPerBin,&nEntryBins);

  hTT_EC = new TH2F("hTT_EC","hTT_EC;Trigger Time Tag (secs);Entry",
		    nClockBins,minClock,maxClock,
		    nEntryBins,firstEntry,lastEntry);

}

void TConvert::SaveHistosDAQ(std::string outFolder){
  
  int maxBin = hNEventsTime->GetMaximumBin();
  int minBin = 0;

  //maxTime = hNEventsTime->GetXaxis()->GetBinCenter(maxBin);
  maxBin++;
  
  hNEventsTime->GetXaxis()->SetRange(minBin,maxBin);
  hEventRate->GetXaxis()->SetRange(minBin,maxBin);

  hNEventsTime->Draw("HIST P");

  std::string outName = outFolder + "hNEventsTime.pdf";
  
  canvas->SaveAs(outName.c_str());
  
  maxBin = hTrigFreq->GetMaximumBin();
  float freqAtMax = hTrigFreq->GetXaxis()->GetBinCenter(maxBin);
  float minFreq = freqAtMax - 0.1;
  float maxFreq = freqAtMax + 0.1;
  
  minBin = hTrigFreq->GetXaxis()->FindBin(minFreq);
  maxBin = hTrigFreq->GetXaxis()->FindBin(maxFreq);

  hTrigFreq->GetXaxis()->SetRange(minBin,maxBin);

  hTrigFreq->Draw("hist");

  outName = outFolder + "hTrigFreq.pdf";
  
  canvas->SaveAs(outName.c_str());

  hEventRate->SetMinimum(minFreq);
  hEventRate->SetMaximum(maxFreq);
  
  hEventRate->Draw("HIST P");

  outName = outFolder + "hEventRate.pdf";
  canvas->SaveAs(outName.c_str());
  
  hTT_EC->Draw("colz");

  outName = outFolder + "hTT_EC.pdf";
  canvas->SaveAs(outName.c_str());

}


double TConvert::GetTrigTimeTag(int entry) {

  b_HEAD->GetEntry(entry);

  return (8.E-9*(double)HEAD[5]);
}

double TConvert::GetElapsedTime(const int entry, 
			      int  * cycles,
			      double prevTime) {

  b_HEAD->GetEntry(entry);
  
  double time = GetTrigTimeTag(entry);
  time += 8.E-9*(double)UINT_MAX/2*(*cycles); 

  if(time < prevTime){
    time += 8.E-9*(double)UINT_MAX/2; // add 17 seconds 
    (*cycles)+= 1;
  }
  
  time -= startTime;

  //printf("\n time = %f \n",time);

  return time;
}

void TConvert::CountMissedEvents(int dTrigEntry){

  if( dTrigEntry != 1 ){
    nMissedEvents += (dTrigEntry-1);
    printf("\n %d missed events \n",nMissedEvents);
  }
  
}

void TConvert::PrintVec(std::vector<short> & v) {

  for (const auto& i : v) 
    printf("\n %u \n", i);
  
}

int TConvert::Get_peakSample(int entry){
  
  b_ADC->GetEntry(entry);
  
  int   peakSample = -1;
  short peakADC    = SHRT_MAX;
  
  for (uint i = 0; i < ADC->size(); ++i) 
  
    if(ADC->at(i) < peakADC){
      peakADC = ADC->at(i);
      peakSample = (int)i;
    }
  printf("\n peakSample = %d \n ",peakSample);
  
  return peakSample;
  
}

float TConvert::Get_peakT_ns(int entry){
  
  return f_nsPerSamp*(float)Get_peakSample(entry);
  
} 

// private
void TConvert::SetDigitiser(char digitiser){

  if(digitiser=='V' || 
     digitiser=='D')
    fDigitiser = digitiser;
  else{
    fprintf( stderr, "\n Error: unknown digitiser \n ");
    fprintf( stderr, "\n Setting to default ('V')  \n ");
    fDigitiser = 'V';
  }
  
  return;
}

void TConvert::SetSampSet(char sampSet){
  
  if  (fDigitiser=='V')
    fSampSet = 'V';
  else
    fSampSet = sampSet;
  
  return;
}

void TConvert::SetPulsePol(char pulsePol){
  
  if(pulsePol == 'N' || 
     pulsePol == 'P')
    fPulsePol = pulsePol;
  else{
    fprintf( stderr, "\n Error: unknown pulse polarity \n ");
    fprintf( stderr, "\n Setting to default ('N')  \n ");
    fPulsePol = 'N';
  }
  
  return;
}

void TConvert::SetConstants(){
  
  printf("\n Setting Constants \n");
  
  fSampFreq = SetSampleFreq();
  fNSamples = SetNSamples();
  fNADCBins = SetNADCBins();
  fRange_V  = SetRange_V();

  // dependent on above
  f_nsPerSamp = Set_nsPerSamp();
  f_mvPerBin  = Set_mVPerBin();
  fLength_ns  = SetLength_ns();

}

void TConvert::PrintConstants(){ 

  printf("\n \n Calibration Constants \n");

  if(fDigitiser=='D'){
    printf("\n  desktop digitiser \n");   
    printf("\n  sampling setting     = %c     ",fSampSet);
  }

  printf("\n  sampling frequency   = %d MHz   \n",fSampFreq);
  printf("   period per sample   = %.1f ns \n",f_nsPerSamp);
  printf("\n  samples per waveform = %d   \n",fNSamples);
  printf("   waveform duration   = %.1f ns \n",fLength_ns);
  printf("\n  number of ADC Bins   = %d   \n",fNADCBins);
  printf("  ADC range            = %d V  \n",fRange_V);
  printf("   ADC bin width       = %.2f mV \n",f_mvPerBin);

  if(fPulsePol!='N')
    printf("\n \t pulse polarity       = %c   \n",fPulsePol);

  printf(" \n " );
  
}

short TConvert::SetSampleFreq(){
  
  if(fDigitiser=='D'){
    switch(fSampSet){
    case '0':
	return 5000;
    case '1':
	return 2500;
    case '2':
	return 1000;
    case '3':
	return 750;
    default:
      return 1000;
    }
  }
  else
    return 500; // 'V'
}

float TConvert::Set_nsPerSamp(){
  return 1000./fSampFreq;
}

short TConvert::SetNSamples(){
  
  fChain->GetEntry(0);   
  
  uint hdrByts = 24;
  uint smpByts = HEAD[0] - hdrByts;

  if(fDigitiser=='V')
    return smpByts/2; // shorts
  else
    return smpByts;   // ints
}

float TConvert::SetLength_ns(){
  return f_nsPerSamp*(float)fNSamples;
}

int TConvert::SetNADCBins(){
  
  if( fDigitiser == 'D' )
    return 4096;
  else   
    return 16384;
}

short TConvert::SetRange_V(){

  if(fDigitiser=='V')
    return 2;
  else
    return 1;
  
}

float TConvert::Set_mVPerBin(){
  
  return (float)fRange_V/fNADCBins*1000.;
  
}


void TConvert::SetStyle(){
  
  printf("\n Setting Style \n");

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
 
  gROOT->SetStyle("wmStyle");
  gROOT->ForceStyle();
 
}
