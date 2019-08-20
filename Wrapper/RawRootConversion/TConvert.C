#define TConvert_cxx
#include "TConvert.h"
#include <TH2.h>
#include <math.h>

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

  float minClock   = 0.0;
  float rangeClock = 34.0; // seconds
  float maxClock   = minClock + rangeClock;
  int   nClockBins = UINT_MAX/1000000 + 1; 

  float secsPerClockBin = rangeClock/nClockBins;
  minClock -= 0.5*secsPerClockBin;
  maxClock += 0.5*secsPerClockBin;

  float minTime    = 0.0;
  float maxTime    = 16.0; // minutes
  float timeRange  = maxTime - minTime;
  float timePerBin = 1./60.; // one second per bin
  int   nTimeBins  = (int)roundf(timeRange/timePerBin);
  minTime -= 0.5*timePerBin;
  maxTime += 0.5*timePerBin;

  fChain->GetEntry(0);
  float firstEntry  = (float)HEAD[4];
  
  fChain->GetEntry(nentries-1);
  float lastEntry    = (float)HEAD[4];
  float binsPerEntry = 1./1000.;
  
  float entriesRange  = lastEntry - firstEntry;
  int   nEntryBins   = (int)roundf(entriesRange * binsPerEntry) + 1; 
  firstEntry -= (0.5/binsPerEntry);
  lastEntry  += (0.5/binsPerEntry);
  
  TCanvas * canvas = new TCanvas();
  float w = 1000., h = 500.;
  canvas->SetWindowSize(w,h);
  
  TH2F * hTT_EC = new TH2F("hTT_EC","hTT_EC;Trigger Time (seconds);Entry",
			   nClockBins,minClock,maxClock,
			   nEntryBins,firstEntry,lastEntry);
  
  TH1D * hTime = new TH1D("hTime","hTime;Time (mins);Events Recorded",
			  nTimeBins,minTime,maxTime);

  TH1D * hRate = new TH1D("hRate","hRate;Time (mins); Record Rate ",
			  nTimeBins,minTime,maxTime);
  
  double trigTime     = 0;
  double startTime    = 0;
  double elapsedTime  = 0;
  double previousTime = 0;
  
  int cycles = 0;
  int recordEntry = 0;
  int prevRecordEntry = 0;
  int dRecordEntry = 0;
  int missRate = 0;
  
  int    eventRate  = 0;
  double clockHand = 0.;
  double timeSinceLastEvent = 0.;

  printf("\n --------------------  \n");
  printf("\n Looping over events \n");

  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry = 0; jentry < nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;    

    trigTime =  8.E-9*(double)HEAD[5]; 
    
    if(jentry==0)
      startTime = trigTime;
    
    elapsedTime = trigTime + 8.E-9*(double)UINT_MAX/2*cycles; 
    
    if(elapsedTime < previousTime){
      cycles++;
      elapsedTime = trigTime + 8.E-9*(double)UINT_MAX/2*cycles; 
    }
    
    elapsedTime -= startTime;
    
    timeSinceLastEvent += elapsedTime;
    
    recordEntry = HEAD[4];
    
    dRecordEntry = recordEntry - prevRecordEntry;
    
    if( dRecordEntry != 1 && jentry !=0){
      missRate++;
      printf("\n Record   Entry = %d \n",recordEntry);
      printf("\n Previous Entry = %d \n",prevRecordEntry);
      printf("\n Missed Events  = %d \n",dRecordEntry);
    }
    
    hTT_EC->Fill(trigTime,recordEntry);
    
    clockHand += timeSinceLastEvent;
    eventRate++; 
      
    if(clockHand >= 1.0){
      
//       printf("\n clockHand = %f   \n",clockHand);
//       printf("\n jentry    = %lld \n",jentry);
//       printf("\n eventRate = %d   \n",eventRate);
      
      hTime->Fill(elapsedTime/60.,jentry);
      hRate->Fill(elapsedTime/60.,eventRate);  
      
      clockHand = 0.;
      eventRate = 0;
      missRate = 0;
    }

    previousTime       = elapsedTime;
    prevRecordEntry    = recordEntry;
    timeSinceLastEvent = -elapsedTime;
    
    // if(jentry == 12000)
//       break;
    
  }

  printf("\n --------------------  \n");
  
  hTime->GetXaxis()->SetRange(minTime,hTime->GetMaximumBin());
  hTime->Draw("hist");
  canvas->SaveAs("./hTime.pdf");
  
  hRate->GetXaxis()->SetRange(minTime,hTime->GetMaximumBin());
  hRate->Draw("hist");
  canvas->SaveAs("./hRate.pdf");
  
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
