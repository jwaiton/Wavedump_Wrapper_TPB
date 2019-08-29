#define TConvert_cxx
#include "TConvert.h"
#include <TH2.h>
#include <math.h>
#include <climits>

#include "../BinToRoot/wmStyle.C"

void TConvert::PreLoop(){

}

void TConvert::PostLoop(){
  
}

void TConvert::Loop()
{

  if (fChain == 0) return;
  
  PreLoop();

  Long64_t nentries = fChain->GetEntriesFast();

  //----------
  // hNEventsTime & hEventRate
  
  float minTime    = 0.0;
  float maxTime    = 16.0; // minutes
  float timeRange  = maxTime - minTime;
  float timePerBin = 1./60.; // one second per bin
  int   nTimeBins  = (int)roundf(timeRange/timePerBin);
  minTime -= 0.5*timePerBin;
  maxTime += 0.5*timePerBin;

  TH1D * hNEventsTime = new TH1D("hNEventsTime",
				 "hNEventsTime;Time (mins);Event",
				 nTimeBins,minTime,maxTime);
  
  TH1D * hEventRate = new TH1D("hEventRate",
			       "hEventRate;Time (mins);Mean Rate (kHz)",
			       nTimeBins,minTime,maxTime);

  //-----------
  // hTrigFreq
  
  float minFreq = 0.0; 
  float maxFreq = 20.0; // kHz
  float freqRange = maxFreq - minFreq;
  float freqPerBin = 1./100; // 10 Hz
  int   nFreqBins = (int)roundf(freqRange/freqPerBin);
  minFreq -= 0.5*freqPerBin;
  maxFreq += 0.5*freqPerBin;
  
  TH1D * hTrigFreq = new TH1D("hTrigFreq",
			      "hTrigFreq; Rate (kHz); Counts",
			      nFreqBins,minFreq,maxFreq);
  //-----------
  
  //-----------
  // hTT_EC
  float minClock   = 0.0;
  float rangeClock = 34.0; // seconds
  float maxClock   = minClock + rangeClock;
  int   nClockBins = UINT_MAX/1000000 + 1; 

  float secsPerClockBin = rangeClock/nClockBins;
  minClock -= 0.5*secsPerClockBin;
  maxClock += 0.5*secsPerClockBin;
  
  fChain->GetEntry(0);
  float firstEntry  = (float)HEAD[4];
  
  fChain->GetEntry(nentries-1);
  float lastEntry    = (float)HEAD[4];
  float binsPerEntry = 1./1000.;
  
  float entriesRange  = lastEntry - firstEntry;
  int   nEntryBins   = (int)roundf(entriesRange * binsPerEntry) + 1; 
  firstEntry -= (0.5/binsPerEntry);
  lastEntry  += (0.5/binsPerEntry);
  
  
  TH2F * hTT_EC = new TH2F("hTT_EC","hTT_EC;Trigger Time Tag (secs);Entry",
			   nClockBins,minClock,maxClock,
			   nEntryBins,firstEntry,lastEntry);
  //--------
  
  
  TCanvas * canvas = new TCanvas();
  float w = 1000., h = 500.;
  canvas->SetWindowSize(w,h);
  
  double trigTime     = 0;
  double startTime    = 0;
  double elapsedTime  = 0;
  double prevElapsedTime = 0;
  
  int cycles = 0;
  int recordEntry = 0;
  int prevRecordEntry = 0;
  int dRecordEntry = 0;

  int missedEvents = 0;
  
  Long64_t nRateEvents = nentries/100;
  double timePerNRateEvents = 0.;
  double timeSinceLastEvent = 0.;

  printf("\n --------------------  \n");
  printf("\n Looping over events \n");

  Long64_t nbytes = 0, nb = 0;
  
  for (Long64_t jentry = 0; jentry < nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;    

    trigTime =  8.E-9*(double)HEAD[5]; // it cycles
    
    if(jentry==0)
      startTime = trigTime;

    elapsedTime = 8.E-9*((double)HEAD[5] + (double)UINT_MAX/2*cycles); 
    
    elapsedTime -= startTime;
    
    if(elapsedTime < prevElapsedTime){
      elapsedTime += 8.E-9*(double)UINT_MAX/2; // add 17 seconds 
      cycles++;
      //printf("\n elapsedTime = %f \n",elapsedTime);
    }
    
    timeSinceLastEvent = elapsedTime - prevElapsedTime;

    recordEntry = HEAD[4];
    
    dRecordEntry = recordEntry - prevRecordEntry;
    
    if( dRecordEntry != 1 && jentry !=0){
      missedEvents += (dRecordEntry-1);
      printf("\n Record   Entry = %d \n",recordEntry);
      printf("\n Previous Entry = %d \n",prevRecordEntry);
      printf("\n Missed Events  = %d \n",dRecordEntry);
    }
    
    hTT_EC->Fill(trigTime,recordEntry);
    
    if(jentry!=0){
      hTrigFreq->Fill(1./1000/timeSinceLastEvent);  
    }
    
    timePerNRateEvents += timeSinceLastEvent;
  
    // plot event rate per nRateEvents
    if( jentry!=0 &&
	jentry%nRateEvents == 0 ){
      
      double eventRate = (double)jentry/timePerNRateEvents/1000.;
      
      hNEventsTime->Fill(elapsedTime/60.,jentry);
      hEventRate->Fill(elapsedTime/60.,eventRate);  
    }
    
    prevElapsedTime    = elapsedTime;
    prevRecordEntry    = recordEntry;
    timeSinceLastEvent = -elapsedTime;
    
  }

  printf("\n --------------------  \n");

  int maxBin = hNEventsTime->GetMaximumBin();
  int minBin = 0;

  maxTime = hNEventsTime->GetXaxis()->GetBinCenter(maxBin);
  maxBin++;
  
  hNEventsTime->GetXaxis()->SetRange(minBin,maxBin);
  hEventRate->GetXaxis()->SetRange(minBin,maxBin);

  hNEventsTime->Draw("HIST P");
  canvas->SaveAs("./hNEventsTime.pdf");
  
  maxBin = hTrigFreq->GetMaximumBin();
  double freqAtMax = hTrigFreq->GetXaxis()->GetBinCenter(maxBin);
  minFreq = freqAtMax - 0.1;
  maxFreq = freqAtMax + 0.1;
  
  minBin = hTrigFreq->GetXaxis()->FindBin(minFreq);
  maxBin = hTrigFreq->GetXaxis()->FindBin(maxFreq);

  hTrigFreq->GetXaxis()->SetRange(minBin,maxBin);

  hTrigFreq->Draw("hist");
  canvas->SaveAs("./hTrigFreq.pdf");

  hEventRate->SetMinimum(minFreq);
  hEventRate->SetMaximum(maxFreq);
  
  hEventRate->Draw("HIST P");
  canvas->SaveAs("./hEventRate.pdf");
  
  hTT_EC->Draw("colz");
  canvas->SaveAs("./hTT_EC.pdf");

}

int TConvert::Get_peakSample(short ADC[]){
  
  int   peakSample = 0;
  short peakADC    = 32767;
    
  for ( int i = 0 ; i < fNSamples ; i++) {
    if(ADC[i] < peakADC){
      peakADC = ADC[i];
      peakSample = i;
    }
  }
  
  //printf("\n peakSample = %d \n ",peakSample);
  
  return peakSample;

}

float TConvert::Get_peakT_ns(short ADC[]){
  
  return 2.0*Get_peakSample(ADC);

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
  fRange_mV = SetRange_mV();
  
  // dependent on above
  f_nsPerSamp = Set_nsPerSamp();
  f_mvPerBin  = Set_mVPerBin();
  fLength_ns  = SetLength_ns();
}

void TConvert::PrintConstants(){ 

  printf("\n \t fDigitiser  = %c \n",fDigitiser);
  if(fDigitiser=='D')
    printf("\n \t fSampSet    = %c \n",fSampSet);
  printf("\n \t fPulsePol   = %c \n",fPulsePol);
  printf("\n \t fSampFreq   = %d \n",fSampFreq);
  printf("\n \t fNSamples   = %d \n",fNSamples);
  printf("\n \t fNADCBins   = %d \n",fNADCBins);
  printf("\n \t fRange_mV   = %d \n",fRange_mV);
  printf("\n \t f_nsPerSamp = %.1f \n",f_nsPerSamp);
  printf("\n \t f_mvPerBin  = %.4f \n",f_mvPerBin);
  printf("\n \t fLength_ns  = %.1f \n\n",fLength_ns);

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
  
  unsigned int hdrByts = 24;
  unsigned int smpByts = HEAD[0] - hdrByts;

  if(fDigitiser=='V')
    return smpByts/2; // shorts
  else
    return smpByts;   // ints
}

float TConvert::SetLength_ns(){
  return f_nsPerSamp*fNSamples;
}

int TConvert::SetNADCBins(){
  
  if( fDigitiser == 'D' )
    return 4096;
  else   
    return 16384;
}

short TConvert::SetRange_mV(){

  if(fDigitiser=='V')
    return 2000;
  else
    return 1000;
  
}

float TConvert::Set_mVPerBin(){
  
  return (float)fRange_mV/fNADCBins;
  
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
