#define TConvert_cxx
#include "TConvert.h"
#include <TH2.h>
#include <math.h>
#include <limits.h>

#include "../BinToRoot/wmStyle.C"

void TConvert::ADC_Loop(){
  if (fChain == 0) return;
  
  for (Long64_t iEntry = 0; iEntry < nentries; iEntry++) {
    fChain->GetEntry(iEntry);
    
    printf("\n ------- \n iEntry %lld  \n", iEntry);
    PrintVec(*ADC);
    
    if(iEntry == 10)
      break;
    
  }
}

void TConvert::Header_Loop()
{
  if (fChain == 0) return;
  
  double time     = 0;
  double prevTime = 0;
  
  int trigCycles    = 0;
  int trigEntry     = 0;
  int prevTrigEntry = 0;
  int dTrigEntry    = 0;
  
  Long64_t nRateEvents = nentries/100;

  Long64_t deltaEvents = 0;
  double   deltaT      = 0.;
  double   dTime       = 0.; // time between events
  double   eventRate;

  printf("\n ---------------------------- \n");
  printf("\n Looping over Header entries \n");

  Long64_t nbytes = 0, nb = 0;

  for (Long64_t iEntry = 0; iEntry < nentries; iEntry++) {
    nb = fChain->GetEntry(iEntry);   nbytes += nb;    
    
    //-----------------------------
    // Process Header Information
    time     = GetElapsedTime(iEntry,&trigCycles,prevTime);
    dTime    = time - prevTime; 
    prevTime = time; // set for next entry
    
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
    
  } // end of: for (Long64_t iEntry = 0...

  printf("\n --------------------  \n");

}

void TConvert::Set_THF_Params(float * minX, 
			      float * maxX,
			      float * binWidth,
			      Long64_t * nBins){
  
  if     (*nBins==0)
    *nBins = (Long64_t)roundf((*maxX - *minX)/(*binWidth));
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

void TConvert::InitHistos(){
  
  //----
  float    minTime    = 0.0;
  float    maxTime    = 16.0; // minutes
  float    timePerBin = 1./6000.; // 100th of a second per bin
  Long64_t nTimeBins  = 0;
    
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
  Long64_t nFreqBins  = 0;
  
  Set_THF_Params(&minFreq,&maxFreq,&freqPerBin,&nFreqBins);
  
  hTrigFreq = new TH1F("hTrigFreq",
		       "hTrigFreq; Rate (kHz); Counts",
		       nFreqBins,minFreq,maxFreq);

  //----
  // hTT_EC
  float    minClock   = 0.0;
  float    maxClock   = 35.0;
  Long64_t nClockBins = (Long64_t)UINT_MAX/1000000 + 1; 
  float    secsPerClockBin = 0.;
  
  Set_THF_Params(&minClock,&maxClock,&secsPerClockBin,&nClockBins);
  
  fChain->GetEntry(0);
  float    firstEntry = (float)HEAD[4];
  
  fChain->GetEntry(nentries-1);
  float    lastEntry  = (float)HEAD[4];

  float    entriesPerBin = 1000.;
  Long64_t nEntryBins    = 0;

  Set_THF_Params(&firstEntry,&lastEntry,&entriesPerBin,&nEntryBins);

  hTT_EC = new TH2F("hTT_EC","hTT_EC;Trigger Time Tag (secs);Entry",
		    nClockBins,minClock,maxClock,
		    nEntryBins,firstEntry,lastEntry);

}

void TConvert::PreLoop(){
    
  nMissedEvents = 0;

  InitCanvas();
  InitHistos();

}

void TConvert::SaveHistos(){
  
  int maxBin = hNEventsTime->GetMaximumBin();
  int minBin = 0;

  //maxTime = hNEventsTime->GetXaxis()->GetBinCenter(maxBin);
  maxBin++;
  
  hNEventsTime->GetXaxis()->SetRange(minBin,maxBin);
  hEventRate->GetXaxis()->SetRange(minBin,maxBin);

  hNEventsTime->Draw("HIST P");
  canvas->SaveAs("./hNEventsTime.pdf");
  
  maxBin = hTrigFreq->GetMaximumBin();
  float freqAtMax = hTrigFreq->GetXaxis()->GetBinCenter(maxBin);
  float minFreq = freqAtMax - 0.1;
  float maxFreq = freqAtMax + 0.1;
  
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

void TConvert::PostLoop(){
  
  SaveHistos();

  //DeleteCanvas();
  
}


double TConvert::GetTrigTimeTag(Long64_t entry) {

  b_HEAD->GetEntry(entry);

  return (8.E-9*(double)HEAD[5]);
}

double TConvert::GetElapsedTime(const Long64_t entry, 
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

int TConvert::Get_peakSample(Long64_t entry){
  
  b_ADC->GetEntry(entry);
  
  int   peakSample = -1;
  short peakADC    = SHRT_MAX;
  
  for (unsigned int iSample = 0; iSample < ADC->size(); ++iSample) 
  
    if(ADC->at(iSample) < peakADC){
      peakADC = ADC->at(iSample);
      peakSample = (int)iSample;
    }
  printf("\n peakSample = %d \n ",peakSample);
  
  return peakSample;
  
}

float TConvert::Get_peakT_ns(Long64_t entry){
  
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
  
  printf("\n fDigitiser = %c", fDigitiser);
  
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
  return f_nsPerSamp*(float)fNSamples;
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
