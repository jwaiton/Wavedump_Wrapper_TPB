#define TRawAnalyser_cxx
#include "TRawAnalyser.h"
#include <TH2.h>
#include <math.h>
#include <limits.h>

#include "wmStyle.C"

float TRawAnalyser::ADC_To_Wave(short ADC){

  float wave = ADC * Get_mVPerBin();
  
  wave -= GetRange_mV()/2.;
  
  if(fPulsePol=='N')
    wave = -wave;
  
  return wave;
}

//------------------------------
// Fix histogram binning
void TRawAnalyser::Set_THF_Params(float * minX, 
			      float * maxX,
			      float * binWidth,
			      int   * nBins){
  
  if     (*nBins==0)
    *nBins = (int)roundf((*maxX - *minX)/(*binWidth));
  else if(*nBins > 0 && *binWidth < 1.0E-10)
    *binWidth = (*maxX - *minX)/(*nBins);
  else
    fprintf(stderr,"\n Error in Set_THF_Params \n");
  
  *nBins += 1;
  *minX -= 0.5*(*binWidth);
  *maxX += 0.5*(*binWidth);

};

void TRawAnalyser::InitCanvas(float w,float h){

  canvas = new TCanvas();
  canvas->SetWindowSize(w,h);

}

void TRawAnalyser::DeleteCanvas(){
  delete canvas;
}

double TRawAnalyser::GetTrigTimeTag() {

  unsigned int TTT;
   
  // limit to 31 bits 
  if( HEAD[5] > (unsigned int)INT_MAX )
    TTT = HEAD[5] - INT_MAX;
  else
    TTT = HEAD[5];
  
  return (double)TTT*8.E-9;
}

double TRawAnalyser::GetTrigTimeTag(int entry) {

  b_HEAD->GetEntry(entry);

  return GetTrigTimeTag();
}

double TRawAnalyser::GetElapsedTime(int * cycles,
				double prevTime) {

  double time = GetTrigTimeTag();
  
  // range is 31 bit with 8 ns LSB = 17.18 seconds 
  // using half range of INT here
  double range = ((double)(INT_MAX)+1.0)*8.E-9;
  
  time += range*(double)(*cycles); 
  
  if(time < prevTime){
    time += range; 
    (*cycles)+= 1;
  }
  
  time -= startTime;
  
  //printf("\n time = %f \n",time);
  
  return time;
}

void TRawAnalyser::CountMissedEvents(int dTrigEntry){

  if( dTrigEntry != 1 ){
    nMissedEvents += (dTrigEntry-1);
    printf("\n %d missed events \n",nMissedEvents);
  }
  
}

float TRawAnalyser::GetLength_ns(){
  return fLength_ns;
}

short TRawAnalyser::GetNSamples(){
  return fNSamples;
}

// private
void TRawAnalyser::SetDigitiser(char digitiser){

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

float TRawAnalyser::GetRange_mV(){
  return (float)fRange_V*1000.;
}

float TRawAnalyser::Get_mVPerBin(){
  return f_mVPerBin;
}

void TRawAnalyser::SetSampSet(char sampSet){
  
  if  (fDigitiser=='V')
    fSampSet = 'V';
  else
    fSampSet = sampSet;
  
  return;
}

void TRawAnalyser::SetPulsePol(char pulsePol){
  
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

void TRawAnalyser::SetConstants(){
  
  printf("\n Setting Constants \n");
  
  fSampFreq = SetSampleFreq();
  fNSamples = SetNSamples();
  fNADCBins = SetNADCBins();
  fRange_V  = SetRange_V();

  // dependent on above
  f_nsPerSamp = Set_nsPerSamp();
  f_mVPerBin  = Set_mVPerBin();
  fLength_ns  = SetLength_ns();

}

void TRawAnalyser::PrintConstants(){ 
  
  printf("\n ------------------------------ \n");
  printf("\n  Acquisition Settings          \n");

  if(fDigitiser=='D'){
    printf("\n  desktop digitiser \n");   
    printf("\n  sampling setting     = %c     ",fSampSet);
  }

  printf("\n  sampling frequency   = %d MHz \n",fSampFreq);
  printf("   period per sample   = %.1f ns  \n",f_nsPerSamp);
  printf("\n  samples per waveform = %d     \n",fNSamples);
  printf("   waveform duration   = %.1f ns  \n",fLength_ns);
  printf("\n  number of ADC Bins   = %d     \n",fNADCBins);
  printf("  ADC range            = %d V     \n",fRange_V);
  printf("   ADC bin width       = %.2f mV  \n",f_mVPerBin);

  if(fPulsePol!='N')
    printf("\n \t pulse polarity       = %c   \n",fPulsePol);

  printf(" \n " );
  
}


//------------------------------
void TRawAnalyser::Waveform(char option){

  switch(option){
  case('w'):
    InitWaveform();
    break;
  case('f'):
    InitFFT();
    break;
  case('b'):
    InitWaveform();
    InitFFT();
    break;
  default:
    InitWaveform();
    InitFFT();
    break;
  }
  
  int entry = (int)round(rand3->Uniform(nentries)); 

  printf("\n entry %d \n",entry);
  
  rawTree->GetEntry(entry);
  
  for( short iSamp = 0 ; iSamp < fNSamples; iSamp++)
    hWave->SetBinContent(iSamp+1,(ADC_To_Wave(ADC->at(iSamp))));

  hWave->FFT(hFFT ,"MAG");
  
  //char answer = 'n';
  //printf(" Save waveform y/n ?");
  //scanf("%c", &answer);
  
  // if( answer=='y' || answer == 'Y')

  switch(option){
  case('w'):
    SaveWaveform();
    break;
  case('f'):
    SaveFFT();
    break;
  case('b'):
    SaveWaveFFT();
    break;
  default:
    break;
  }
  
}

void TRawAnalyser::InitWaveform(){
  
  printf("\n ------------------------------ \n");
  printf("\n Plotting Waveform \n\n");
  
  hWave = new TH1F("hWave","Waveform;Time (ns); Amplitude (mV)",
		   fNSamples, 0.,fLength_ns);
  
}

void TRawAnalyser::InitFFT(){
  
  printf("\n ------------------------------ \n");
  printf("\n Plotting FFT \n\n");

  hFFT = new TH1F("hFFT","FFT; Frequency (MHz); Magnitude",
		  fNSamples/2, 0, fSampFreq/2 );
  
}


void TRawAnalyser::SaveWaveform(string outFolder){

  printf("\n Saving Waveform Plot \n\n");
  
  InitCanvas();
  
  hWave->Draw();
  
  string outName = outFolder + "hWave.pdf";
  
  canvas->SaveAs(outName.c_str());
  
  DeleteCanvas();
  
}

void TRawAnalyser::SaveFFT(string outFolder){

  printf("\n Saving FFT Plot \n\n");
  
  InitCanvas();
  
  hFFT->SetBinContent(1,0.);
  hFFT->Draw();
  
  string outName = outFolder + "hFFT.pdf";
  
  canvas->SaveAs(outName.c_str());
  
  DeleteCanvas();
  
}

void TRawAnalyser::SaveWaveFFT(string outFolder){

  printf("\n Saving Waveform and FFT Plots \n\n");
  
  InitCanvas(1600.);
  
  canvas->Divide(2,1);
  
  canvas->cd(1);
  hWave->Draw();
  
  canvas->cd(2);
  hFFT->Draw();
  
  string outName = outFolder + "hWaveFFT.pdf";
  
  canvas->SaveAs(outName.c_str());
  
  DeleteCanvas();
  
}

//------------------------------
// 

 void TRawAnalyser::DAQ()
{
  
  nMissedEvents = 0;

  InitDAQ();
  
  double time     = 0;
  double prevTime = 0;
  
  int trigCycles    = 0;
  int trigEntry     = 0;
  int prevTrigEntry = 0;
  int dTrigEntry    = 0;
  
  int nRateEvents = (int)round(nentries/100); // how many to average

  int     deltaEvents = 0;
  double  deltaT      = 0.;
  double  dTime       = 0.; // time between events

  double  eventRate = 0.;
  double  meanRate  = 0.;

  int nbytes = 0, nb = 0;

  for (int iEntry = 0; iEntry < nentries; iEntry++) {
    nb = rawTree->GetEntry(iEntry);   nbytes += nb;    
    
    //-----------------------------
    // Process Header Information
    time = GetElapsedTime(&trigCycles,prevTime);
    
//     printf("\n prevTime = %f \n\n",prevTime);
//     printf("\n time     = %f \n\n",time);
//     printf("\n dTime    = %f \n\n",dTime);
    
    dTime    = time - prevTime; 
    prevTime = time; // now set for next entry
    
    deltaT   += dTime;   // integrated time
    trigEntry = HEAD[4]; // absolute entry number
    
    hTT_EC->Fill(GetTrigTimeTag(),trigEntry);

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
  
  //printf("\n mean event rate = %.2f kHz \n",meanRate);
  printf("\n Mean trigger frequency is %.2f kHz \n\n",hTrigFreq->GetMean());
  
  SaveDAQ();

  printf("\n ------------------------------ \n");
}

void TRawAnalyser::InitDAQ(){
  
  printf("\n ------------------------------ \n");
  printf("\n Getting DAQ Info             \n\n");

  //----
  float minTime    = 0.0;
  float maxTime    = 16.0; // minutes
  float timePerBin = 1./6000.; // 100th of a second per bin
  int   nTimeBins  = 0;
    
  Set_THF_Params(&minTime,&maxTime,&timePerBin,&nTimeBins);
  
  hNEventsTime = new TH1F("hNEventsTime",
			  "hNEventsTime;Time (mins);Event",
			  nTimeBins,minTime,maxTime);
  
  hEventRate = new TH1F("hEventRate",
			"hEventRate;Time (mins);Mean Rate (kHz)",
			nTimeBins,minTime,maxTime);

  //----
  float minFreq    = 0.0; 
  float maxFreq    = 100.0;    // kHz
  float freqPerBin = 1./10000; // 1/10 Hz
  int   nFreqBins  = 0;
  
  Set_THF_Params(&minFreq,&maxFreq,&freqPerBin,&nFreqBins);
  
  hTrigFreq = new TH1F("hTrigFreq",
		       "hTrigFreq; Rate (kHz); Counts",
		       nFreqBins,minFreq,maxFreq);

  //----
  // hTT_EC
  float minClock   = 0.0;
  float maxClock   = ((float)(INT_MAX)+1.0)*8.E-9;
  int   nClockBins = (int)round(UINT_MAX/1000000) + 1; 
  float secsPerClockBin = 0.;
  
  Set_THF_Params(&minClock,&maxClock,&secsPerClockBin,&nClockBins);
  
  rawTree->GetEntry(0);
  float firstEntry = HEAD[4];
  
  rawTree->GetEntry(nentries-1);
  float lastEntry  = HEAD[4];

  float entriesPerBin = 1000.;
  int   nEntryBins    = 0;

  Set_THF_Params(&firstEntry,&lastEntry,&entriesPerBin,&nEntryBins);

  hTT_EC = new TH2F("hTT_EC","hTT_EC;Trigger Time Tag (secs);Entry",
		    nClockBins,minClock,maxClock,
		    nEntryBins,firstEntry,lastEntry);

}

void TRawAnalyser::SaveDAQ(string outFolder){
  
  InitCanvas();
  
  int maxBin = hNEventsTime->GetMaximumBin();
  int minBin = 0;

  //maxTime = hNEventsTime->GetXaxis()->GetBinCenter(maxBin);
  maxBin++;
  
  hNEventsTime->GetXaxis()->SetRange(minBin,maxBin);
  hEventRate->GetXaxis()->SetRange(minBin,maxBin);

  hNEventsTime->Draw("HIST P");

  string outName = outFolder + "hNEventsTime.pdf";
  
  canvas->SaveAs(outName.c_str());
  
  maxBin = hTrigFreq->GetMaximumBin();
  float meanFreq_kHz = hTrigFreq->GetMean();
  float minFreq_kHz  = meanFreq_kHz - 2.;
  float maxFreq_kHz  = meanFreq_kHz + 2.;
  
  hTrigFreq->SetAxisRange(minFreq_kHz, maxFreq_kHz,"X");

  hTrigFreq->Draw("hist");

  outName = outFolder + "hTrigFreq.pdf";
  
  canvas->SaveAs(outName.c_str());

  hEventRate->SetMinimum(minFreq_kHz);
  hEventRate->SetMaximum(maxFreq_kHz);
  
  hEventRate->Draw("HIST P");

  outName = outFolder + "hEventRate.pdf";
  canvas->SaveAs(outName.c_str());
  
  hTT_EC->Draw("colz");

  outName = outFolder + "hTT_EC.pdf";
  canvas->SaveAs(outName.c_str());

  DeleteCanvas();
}



short TRawAnalyser::SetSampleFreq(){
  
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

float TRawAnalyser::Set_nsPerSamp(){
  return 1000./fSampFreq;
}

float TRawAnalyser::SampleToTime(){
  return f_nsPerSamp;
}

short TRawAnalyser::SetNSamples(){
  
  rawTree->GetEntry(0);   
  
  uint hdrByts = 24;
  uint smpByts = HEAD[0] - hdrByts;

  if(fDigitiser=='V')
    return smpByts/2; // shorts
  else
    return smpByts/4;   // ints
}

float TRawAnalyser::SetLength_ns(){
  return f_nsPerSamp*fNSamples;
}

int TRawAnalyser::SetNADCBins(){
  
  if( fDigitiser == 'D' )
    return 4096;
  else   
    return 16384;
}

short TRawAnalyser::SetRange_V(){

  if(fDigitiser=='V')
    return 2;
  else
    return 1;
  
}

float TRawAnalyser::Set_mVPerBin(){
  
  return 1000.*fRange_V/fNADCBins;
  
}


void TRawAnalyser::SetStyle(){
  
  printf("\n Setting Style \n");

  TStyle *wmStyle = GetwmStyle();
 
  const int NCont = 255;
  const int NRGBs = 5;
  
  // Color scheme for 2D plotting with a better defined scale 
  double stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  double red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  double green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  double blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };          
  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  
  wmStyle->SetNumberContours(NCont);
 
  gROOT->SetStyle("wmStyle");
  gROOT->ForceStyle();
 
}
