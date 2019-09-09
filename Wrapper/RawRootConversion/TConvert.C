#define TConvert_cxx
#include "TConvert.h"
#include <TH2.h>
#include <math.h>
#include <limits.h>

#include "../BinToRoot/wmStyle.C"

void TConvert::InitBaseline(){
  
  printf("\n ------------------------------ \n");
  printf("\n Getting Baseline Info          \n");
  
  float range_mV = fRange_V*1000.;
  
  float mVPerBin = f_mVPerBin;
  float min_mV   = -range_mV/2.;
  float max_mV   =  range_mV/2.;
  int   nBins    = 0;
  
  // fix binning and set number of bins
  Set_THF_Params(&min_mV,&max_mV,&mVPerBin,&nBins);
  
  hBase = new TH1F("hBase",
		   "hBase;baseline voltage (mV);Counts",
		   nBins,min_mV,max_mV);
  
  hPeak = new TH1F("hPeak",
		   "hPeak;peak voltage (mV);Counts",
		   nBins,min_mV,max_mV);
  
  hBase_Peak = new TH2F("hBase_Peak",
			"hBase_Peak;baseline voltage (mV);peak voltage (mV)",
			nBins,min_mV,max_mV,
			nBins,min_mV,max_mV);
  
  hFloor_Peak = new TH2F("hFloor_Peak",
		       "hFloor_Peak;floor voltage (mV);peak voltage (mV)",
		       nBins,min_mV,max_mV,
		       nBins,min_mV,max_mV);
  

  float minEvent = 0;
  float maxEvent = float(nEvents_Base-1);
  float eventsPerBin = 0.; // dummy
  int   nEvents;

  minEvent = 0.;
  maxEvent = 0. + (float)nEvents_Base - 1.;
  eventsPerBin = 0.;
  nEvents = nEvents_Base;
    
  Set_THF_Params(&minEvent,&maxEvent,&eventsPerBin,&nEvents);

  hEvent_Base = new TH2F("hEvent_Base",
			 ";Event;baseline voltage (mV)",
			 nEvents,minEvent,maxEvent,
			 nBins,min_mV,max_mV);
 
}

void TConvert::SaveBaseline(std::string outFolder){
  
  TLegend *leg = new TLegend(0.21,0.2,0.31,0.9);
    
  //  char title[128] = "";
  leg->SetTextSize(0.025);
  leg->SetHeader("Baseline start","C");
  
//   Int_t colors[] = {kRed-7,kRed,kRed+2,
// 		    kRed-5,kOrange,kOrange+2,
// 		    kOrange+4,kYellow+1,kGreen+1,
// 		    kGreen+3,kGreen-5,kCyan+1,
// 		    kCyan+3,kBlue-7,kBlue,
// 		    kBlue+3,kViolet,kMagenta+1};
  
  leg->SetMargin(0.4); 


  hBase->SetAxisRange(-25., 25.,"X");
  hBase->SetMinimum(0.1);

  hBase->Draw();
  
  //  sprintf(title,"%.0f ns",iBase*10.);
  //  leg->AddEntry(hBase[iBase],title,"L");  
  //  leg->Draw();
  
  gPad->SetLogy();
  std::string outName = outFolder + "hBase.pdf";
  canvas->SaveAs(outName.c_str());
  
  hPeak->SetAxisRange(-5., 75.,"X");
  hPeak->Draw();

  outName = outFolder + "hPeak.pdf";
  canvas->SaveAs(outName.c_str());
  
  gPad->SetLogy(false);

  hBase_Peak->SetAxisRange(-25.,25.,"X");
  hBase_Peak->SetAxisRange(-5., 45.,"Y");
  
  hBase_Peak->Draw("col");
  
  gPad->SetLogz();
  
  outName = outFolder + "hBase_Peak.pdf";
  canvas->SaveAs(outName.c_str());

  //
  hFloor_Peak->SetAxisRange(-25.,25.,"X");
  hFloor_Peak->SetAxisRange(-5., 45.,"Y");
  
  hFloor_Peak->Draw("col");
  
  gPad->SetLogz();
  
  outName = outFolder + "hFloor_Peak.pdf";
  canvas->SaveAs(outName.c_str());

  
  gPad->SetLogz(false);

  float w = 10000., h = 100.;
  canvas->SetWindowSize(w,h);
  
  gPad->SetGrid(1, 1);// gPad->Update();
  gPad->SetLogz();
  
  float baseMean_mV = 0.0;
  float min_mV = -1.5, max_mV = 1.5;
  baseMean_mV = hBase->GetMean(); 
  min_mV = baseMean_mV - 1.5;
  max_mV = baseMean_mV + 1.5;
  
  hEvent_Base->SetAxisRange(min_mV,max_mV,"Y");
  hEvent_Base->Draw("col");

  outName = outFolder + "hEvent_Base.pdf";
  canvas->SaveAs(outName.c_str());
  
  
  w = 1000., h = 800.;
  canvas->SetWindowSize(w,h);
  gPad->SetGrid(0, 0);
  gPad->SetLogz(false);
}

void TConvert::InitNoise(){
  
  printf("\n ------------------------------ \n");
  printf("\n Getting Noise Info             \n");

  float range_mV = fRange_V*1000.;
  
  float mVPerBin = f_mVPerBin;
  float min_mV   = -range_mV/2.;
  float max_mV   =  range_mV/2.;
  int   nBins    = 0;
  
  // fix binning and set number of bins
  Set_THF_Params(&min_mV,&max_mV,&mVPerBin,&nBins);
  
  hMean = new TH1F("hMean",
		   ";mean voltage (mV);Counts",
		   nBins,min_mV,max_mV);

  hMax =  new TH1F("hMax",
		   ";max voltage (mV);Counts",
		   nBins,min_mV,max_mV);
  
  hMin =  new TH1F("hMin",
		   ";min voltage (mV);Counts",
		   nBins,min_mV,max_mV);
  
//   printf("\n nBins    = %d \n",nBins);
//   printf("\n min_mV   = %f \n",min_mV);
//   printf("\n max_mV   = %f \n",max_mV);
//   printf("\n mVPerBin = %f \n",mVPerBin);
//   printf("\n f_mVPerBin = %f \n",f_mVPerBin);

  hMin_Max =  new TH2F("hMin_Max",
		       ";minimum voltage (mV);maximum voltage (mV)",
		       nBins,min_mV,max_mV,
		       nBins,min_mV,max_mV);

  // prepare for range starting at zero
  min_mV = 0.0;
  max_mV = range_mV/2.;
  nBins  = 0;
  
  Set_THF_Params(&min_mV,&max_mV,&mVPerBin,&nBins);

  hPPV =  new TH1F("hPPV",
		   ";peak to peak voltage (mV);Counts",
		   nBins,min_mV,max_mV);
 
  

}


void TConvert::SaveNoise(std::string outFolder){

  gPad->SetLogy();
  
  hMean->SetAxisRange(-100., 100.,"X");
  hMean->SetMinimum(0.1);
  hMean->Draw();

  std::string outName = outFolder + "hMean.pdf";
  canvas->SaveAs(outName.c_str());  
  
  hPPV->SetAxisRange(-50.0, 250.,"X");
  hPPV->SetMinimum(0.1);
  hPPV->Draw();
  
  outName = outFolder + "hPPV.pdf";
  canvas->SaveAs(outName.c_str());
  
  hMax->SetAxisRange(-50., 100.,"X");
  hMax->SetMinimum(0.1);
  hMax->Draw();
  outName = outFolder + "hMax.pdf";
  canvas->SaveAs(outName.c_str());
  
  hMin->SetAxisRange(-100., 50.,"X");
  hMin->SetMinimum(0.1);
  hMin->Draw();
  outName = outFolder + "hMin.pdf";
  canvas->SaveAs(outName.c_str());
  
  gPad->SetLogy(false);
  gPad->SetLogz();
  
  hMin_Max->SetAxisRange(-15., 15.,"X");
  hMin_Max->SetAxisRange(-15., 50.,"Y");
  
  hMin_Max->Draw("colz");

  outName = outFolder + "hMin_Max.pdf";
  canvas->SaveAs(outName.c_str());

  gPad->SetLogz(false);

}

float TConvert::ADC_To_mV(){

  return f_mVPerBin;
}

float TConvert::ADC_To_Wave(short ADC){

  float wave_mV = ADC * ADC_To_mV();
  wave_mV -= 1000;
  
  if(fPulsePol=='N')
    wave_mV = -wave_mV;
  
  return wave_mV;
}

bool TConvert::IsSampleInBaseline(short iSample,
				  short option = 1){
  
  float time = (float)iSample * SampleToTime();
  
  switch(option){
  case(0):
    time = time + 10; // exclude peak pulse
  case(1):
    break;
  case(2):
    time = time - fLength_ns + 50.; // use end of waveform
  }
  
  if( time >= 0  && time < 50 )
    return true;
  else
    return false;
}

void TConvert::Baseline(){
    
  InitBaseline();

  float base_mV = 0.;
  short nBaseSamps = 0;
  float wave_mV = 0.;  
  float peak_mV = -1000.;  
  float floor_mV = 1000.;  
  short peakSamp = 0;
  
  for (int iEntry = 0; iEntry < nentries; iEntry++) {
    fChain->GetEntry(iEntry);
    
    base_mV  = 0.;
    nBaseSamps = 0;
    
    peak_mV = 0.;
    peakSamp = 0;  
    
    floor_mV = 0.;

    for (short iSamp = 0; iSamp < fNSamples; ++iSamp){
      wave_mV = ADC_To_Wave(ADC->at(iSamp));
      
      if(wave_mV >= peak_mV){
	peak_mV  = wave_mV;
	peakSamp = iSamp;
      }
      
      if(wave_mV < floor_mV){
	floor_mV = wave_mV;
      }
      
      
      if(IsSampleInBaseline(peakSamp,1)){
	base_mV += wave_mV;
	nBaseSamps++;
      }
    }//end: for (short iSamp =
    
    // recalculate baseline if peak was in 
    // standard baseline region
    if( IsSampleInBaseline(peakSamp,0) ){
      base_mV  = 0.;
      nBaseSamps = 0;
      for (short iSamp = 0; iSamp < fNSamples; ++iSamp){
	wave_mV = ADC_To_Wave(ADC->at(iSamp));
	
	if(IsSampleInBaseline(peakSamp,2)){
	  base_mV += wave_mV;
	  nBaseSamps++;
	}
	
      }
    }
    
    base_mV /= (float)nBaseSamps;
    hBase->Fill(base_mV);

    peak_mV  -= base_mV;
    floor_mV -= base_mV;
    
    hPeak->Fill(peak_mV);
       
    hBase_Peak->Fill(base_mV,peak_mV);
    hFloor_Peak->Fill(floor_mV,peak_mV);
  
    if( iEntry > 0 && iEntry < 10000 )
      hEvent_Base->Fill(iEntry,base_mV);
    
  }// end: for (int iEntry 
  
  SaveBaseline();
  
}

void TConvert::Noise(){
  
  InitNoise();
  
  float min_mV  = 1000.; 
  float max_mV  = -1000.;
  float mean_mV = 0.;
  float wave_mV = 0.;
  float ppV_mV  = 0.;
  
  for (int iEntry = 0; iEntry < nentries; iEntry++) {
    fChain->GetEntry(iEntry);

    min_mV  = 1000.; 
    max_mV  = -1000.;
    mean_mV = 0.;
    wave_mV = 0.;
    ppV_mV  = 0.;
    
    
    for (short iSamp = 0; iSamp < fNSamples; ++iSamp){
      
      // map ADC to [-1000,1000] mV
      wave_mV = ADC_To_Wave(ADC->at(iSamp));
      
      if(wave_mV > max_mV)
	max_mV = wave_mV;

      if(wave_mV < min_mV)
	min_mV = wave_mV;

      mean_mV += wave_mV;
    }

    //printf(" \n mean_mV = %f \n", mean_mV);
    
    ppV_mV = max_mV - min_mV;
    mean_mV = mean_mV/fNSamples;

    hMean->Fill(mean_mV);
    hPPV->Fill(ppV_mV);
    hMax->Fill(max_mV);
    hMin->Fill(min_mV);
    hMin_Max->Fill(min_mV,max_mV);

    
  }// end: for (int iEntry = 0;

  SaveNoise();
}

void TConvert::DAQInfo()
{
  
  nMissedEvents = 0;

  InitDAQ();
  
  float time     = 0;
  float prevTime = 0;
  
  int trigCycles    = 0;
  int trigEntry     = 0;
  int prevTrigEntry = 0;
  int dTrigEntry    = 0;
  
  int nRateEvents = nentries/100;

  int    deltaEvents = 0;
  float  deltaT      = 0.;
  double dTime       = 0.; // time between events
  float eventRate;

  float meanRate = 0;

  int nbytes = 0, nb = 0;

  for (int iEntry = 0; iEntry < nentries; iEntry++) {
    nb = fChain->GetEntry(iEntry);   nbytes += nb;    
    
    //-----------------------------
    // Process Header Information
    time     = GetElapsedTime(iEntry,&trigCycles,prevTime);
    
//     printf("\n prevTime = %f \n\n",prevTime);
//     printf("\n time     = %f \n\n",time);
//     printf("\n dTime    = %f \n\n",dTime);
    
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

  SaveDAQ();

  printf("\n ------------------------------ \n");
}

void TConvert::Set_THF_Params(float * minX, 
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

void TConvert::InitCanvas(){

  canvas = new TCanvas();
  float w = 1000., h = 800.;
  canvas->SetWindowSize(w,h);

}

void TConvert::InitDAQ(){
  
  printf("\n ------------------------------ \n");
  printf("\n Getting DAQ Info               \n");

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
  float firstEntry = HEAD[4];
  
  fChain->GetEntry(nentries-1);
  float lastEntry  = HEAD[4];

  float entriesPerBin = 1000.;
  int   nEntryBins    = 0;

  Set_THF_Params(&firstEntry,&lastEntry,&entriesPerBin,&nEntryBins);

  hTT_EC = new TH2F("hTT_EC","hTT_EC;Trigger Time Tag (secs);Entry",
		    nClockBins,minClock,maxClock,
		    nEntryBins,firstEntry,lastEntry);

}

void TConvert::SaveDAQ(std::string outFolder){
  
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


float TConvert::GetTrigTimeTag(int entry) {

  b_HEAD->GetEntry(entry);

  return (float)8.E-9*HEAD[5];
}

float TConvert::GetElapsedTime(const int entry, 
			       int  * cycles,
			       float prevTime) {
  
  b_HEAD->GetEntry(entry);
  
  float time = GetTrigTimeTag(entry);
  time += 8.E-9*UINT_MAX/2*(*cycles); 

  if(time < prevTime){
    time += 8.E-9*UINT_MAX/2; // add 17 seconds 
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
  
  return SampleToTime()*Get_peakSample(entry);
  
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
  f_mVPerBin  = Set_mVPerBin();
  fLength_ns  = SetLength_ns();

}

void TConvert::PrintConstants(){ 
  
  printf("\n ------------------------------ \n");
  printf("\n \n Calibration Constants \n");

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

float TConvert::SampleToTime(){
  return f_nsPerSamp;
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
  return f_nsPerSamp*fNSamples;
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
  
  return 1000.*fRange_V/fNADCBins;
  
}


void TConvert::SetStyle(){
  
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
