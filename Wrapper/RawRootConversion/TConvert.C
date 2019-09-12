#define TConvert_cxx
#include "TConvert.h"
#include <TH2.h>
#include <math.h>
#include <limits.h>

#include "../BinToRoot/wmStyle.C"

void TConvert::Cook(){
  
  // initialise trees
  InitCooking();

  // create variables in standard units
  // and find waveform peak
  DoCooking();

  SaveCookedData();

}

void TConvert::InitCooking(){

  printf("\n ------------------------------ \n");
  printf("\n Initialising Cooking \n");

  InitCookedDataFile();
  InitCookedDataTree();

}

void TConvert::InitCookedData(){
  
//   InitCookedDataFile();
   ConnectToCookedTree();
  
}


void TConvert::InitCookedDataFile(){
  
  string fileName = GetFileID();
  fileName += ".root";
  
  outFile = new TFile(fileName.c_str(),
		      "RECREATE",
		      fileName.c_str());
  

}

void TConvert::SaveCookedData(){

  cookedTree->Write();
}

void TConvert::InitCookedDataTree(){
  
  // ----------
  // Temporary tree for calibrating
  string treeName = "Cooked_";
  treeName += GetFileID();

  cookedTree = new TTree(treeName.c_str(),treeName.c_str());

  cookedTree->Branch("min_cook_mV",&min_cook_mV,"min_cook_mV/F");
  cookedTree->Branch("max_cook_mV",&max_cook_mV,"max_cook_mV/F");
  cookedTree->Branch("ppV_cook_mV",&ppV_cook_mV,"ppV_cook_mV/F");
  cookedTree->Branch("mean_cook_mV",&mean_cook_mV,"mean_cook_mV/F");
  cookedTree->Branch("peak_samp",&peak_samp,"peak_samp/S");
  
}

void TConvert::DoCooking(){
  
  printf("\n ------------------------------ \n");
  printf("\n Cook Variables                 \n");
  
  float wave_cook_mV = 0;

  for (int iEntry = 0; iEntry < nentries; iEntry++) {
    rawTree->GetEntry(iEntry);
  
    min_cook_mV  =  1000.;
    max_cook_mV  = -1000.;
    ppV_cook_mV  =  0.;
    mean_cook_mV =  0.;
    peak_samp   =  0;
    
    for (short iSamp = 0; iSamp < fNSamples; ++iSamp){
      wave_cook_mV = ADC_To_Wave(ADC->at(iSamp));
    
      //printf("\n iSamp       = %d  \n ",iSamp);
      //printf("\n wave_cook_mV = %f  \n ",wave_cook_mV);
  
      // min
      if( wave_cook_mV < min_cook_mV)
	min_cook_mV = wave_cook_mV;
      
      // max, peak_samp
      if( wave_cook_mV >= max_cook_mV ){
	max_cook_mV = wave_cook_mV;
	peak_samp  = iSamp;
      }
      // mean
      mean_cook_mV += wave_cook_mV;
    }
    
    ppV_cook_mV  = max_cook_mV - min_cook_mV;
    mean_cook_mV = mean_cook_mV/(float)fNSamples;
    
    //printf("\n event       = %d  \n ",iEntry);
    //printf("\n peak_samp   = %d  \n ",peak_samp);
    //printf("\n max_cook_mV  = %f  \n ",max_cook_mV);
    //printf("\n min_cook_mV  = %f  \n ",min_cook_mV);

    cookedTree->Fill();
  }
  
  
}




//------------------------------
void TConvert::Calibrate(){

  InitCalibration();
  
  DoCalibration();
  
  SaveCalibratedData();
  
}

void TConvert::InitCalibration(){

  printf("\n ------------------------------ \n");
  printf("\n Initialising Calibration \n");

  //InitCalibratedDataFile();
  InitCalibratedDataTree();
  //ConnectToCookedTree();

}

void TConvert::DoCalibration(){
  
  SubtractBaseline();
  
}


void TConvert::InitCalibratedDataFile(){

  string fileName = GetFileID();
  fileName += ".root";
  
  outFile = new TFile(fileName.c_str(),
		      "RECREATE",
		      fileName.c_str());
  
}

void TConvert::InitCalibratedDataTree(){
  
  // ----------
  // Output event tree for saving calibrated data
  string treeName = "Events_";
  treeName += GetFileID();
  treeName += ".root";
  
  printf("\n Output Events Tree is: \n \t %s \n",treeName.c_str());

  outTree = new TTree(treeName.c_str(),treeName.c_str());

  outTree->Branch("min_mV",&min_mV,"min_mV/F");
  outTree->Branch("peak_mV",&peak_mV,"peak_mV/F");
  outTree->Branch("base_mV",&base_mV,"base_mV/F");
  outTree->Branch("peak_time_ns",&peak_time_ns,"peak_time_ns/F");
  outTree->Branch("event_time_ns",&event_time_ns,"event_time_ns/F");
  outTree->Branch("peak_samp",&peak_samp,"peak_samp/S");
  
  // ----------
  // Output meta data tree for saving calibration constants
  // To Do
}

void TConvert::SaveCalibratedData(){
  
  outTree->Write();
  //outFile->Delete();  
}


////
void TConvert::SetFileID(){

  //!!! Temporary
  f_fileID = "Run_1_PMT_130_Loc_0_Test_D";
  
}

string TConvert::GetFileID(){
  
  return f_fileID;
  
}

string TConvert::GetCookedTreeID(){
  return "Cooked_" + GetFileID();  
}

string TConvert::GetCalibratedTreeID(){
  return "Calibrated_" + GetFileID();  
}


void TConvert::SubtractBaseline(){

  printf("\n ------------------------------ \n");
  printf("\n  Baseline Subtraction \n");

  float wave_mV  = 0;
  int   nBaseSamps = 0;

  for (int iEntry = 0; iEntry < nentries; iEntry++) {
    
    rawTree->GetEntry(iEntry);
    
    nBaseSamps = 0;
    base_mV    = 0.;
    
    // pulse was outside of baseline region
    if( !IsSampleInBaseline(peak_samp,0) ){
      
      for (short iSamp = 0; iSamp < fNSamples; ++iSamp){
	wave_mV = ADC_To_Wave(ADC->at(iSamp));
	
	if(IsSampleInBaseline(iSamp,1)){
	  base_mV += wave_mV;
	  nBaseSamps++;
	}
      }
    }
    else{ // pulse was in standard baseline region
      
      for (short iSamp = 0; iSamp < fNSamples; ++iSamp){
	wave_mV = ADC_To_Wave(ADC->at(iSamp));
	
	if(IsSampleInBaseline(iSamp,2)){
	  base_mV += wave_mV;
	  nBaseSamps++;
	}
      }
    }

    base_mV /= (float)nBaseSamps;
    
    min_mV  = min_cook_mV - base_mV;
    peak_mV = max_cook_mV - base_mV;
    
    peak_time_ns = peak_samp * f_nsPerSamp;

    outTree->Fill();

    // printf("\n min_cook_mV = %f  \n ",min_cook_mV);
    // printf("\n event     = %d  \n ",iEntry);
    // printf("\n peak_samp = %d  \n ",peak_samp);
    // printf("\n base_mV   = %f  \n ",base_mV);
    // printf("\n min_mV    = %f  \n ",min_mV);
    
  } 
}

void TConvert::Dark(float thresh_mV){
  
  InitDark();
  
  int nDark = 0;
  int nDark_noise = 0;
  
  for (int iEntry = 0; iEntry < nentries; iEntry++) {
    outTree->GetEntry(iEntry);
     
    if(peak_mV > thresh_mV)
      nDark_noise++;

    // Noise Rejection 
    if( min_mV < -2.5 && peak_mV < thresh_mV)
      continue;
    
    if( peak_mV < -2*min_mV && peak_mV > thresh_mV )
      continue;
    
    hD_Peak->Fill(peak_mV);
    hD_Min_Peak->Fill(min_mV,peak_mV);
    
    if( peak_mV < thresh_mV)
      continue;
    
    nDark++;
    
  }// end: for (int iEntry 
  
  float darkRate = (float)nDark/nentries;
  darkRate = darkRate/fLength_ns * 1.0e9;
  
  printf("\n \n nentries = %d \n",nentries);
  printf("\n dark counts (noise rejected) = %d \n",nDark);
  printf("\n dark rate   (noise rejected) = %.0f \n",darkRate);
  
  darkRate = (float)nDark_noise/nentries;
  darkRate = darkRate/fLength_ns * 1.0e9;
  
  printf("\n dark counts (with noise)     = %d \n",nDark_noise);
  printf("\n dark rate   (with noise)     = %.0f \n\n",darkRate);

  SaveDark();
  
}

void TConvert::InitDark(){
  
  printf("\n ------------------------------ \n");
  printf("\n Dark Counts Analysis           \n");
  
  float mVPerBin =  Get_mVPerBin();
  float max_mV   =  GetRange_mV()/2.;
  float min_mV   = -GetRange_mV()/2.;
  int   nBins    = 0;
  
  // fix binning and set number of bins
  Set_THF_Params(&min_mV,&max_mV,&mVPerBin,&nBins);
  
//   hBase = new TH1F("hBase",
// 		   "hBase;baseline voltage (mV);Counts",
// 		   nBins,min_mV,max_mV);
  
  hD_Peak = new TH1F("hD_Peak",
		     "hD_Peak;peak voltage (mV);Counts",
		     nBins,min_mV,max_mV);
  
//   hBase_Peak = new TH2F("hBase_Peak",
// 			"hBase_Peak;baseline voltage (mV);peak voltage (mV)",
// 			nBins,min_mV,max_mV,
// 			nBins,min_mV,max_mV);
  
  hD_Min_Peak = new TH2F("hD_Min_Peak",
			   "hD_Min_Peak;min voltage (mV);peak voltage (mV)",
			   nBins,min_mV,max_mV,
			   nBins,min_mV,max_mV);
  
}


void TConvert::SaveDark(string outFolder){

  InitCanvas();

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

  gPad->SetLogy();
  
  hD_Peak->SetAxisRange(-5., 75.,"X");
  hD_Peak->SetMinimum(0.1);
  hD_Peak->Draw();
  
  TLine * lVert = new TLine(10,0,10,20);
  lVert->SetLineColor(kBlue);
  lVert->SetLineWidth(2);
  lVert->SetLineStyle(2);
  lVert->Draw();

  string outName = outFolder + "hD_Peak.pdf";
  canvas->SaveAs(outName.c_str());
  
  gPad->SetLogy(false);

//   hBase_Peak->SetAxisRange(-25.,25.,"X");
//   hBase_Peak->SetAxisRange(-5., 45.,"Y");
  
//   hBase_Peak->Draw("col");
  
//   gPad->SetLogz();
  
//   outName = outFolder + "hBase_Peak.pdf";
//   canvas->SaveAs(outName.c_str());

  //
  hD_Min_Peak->SetAxisRange(-25.,25.,"X");
  hD_Min_Peak->SetAxisRange(-5., 45.,"Y");
  
  gPad->SetGrid(1, 1);
  hD_Min_Peak->Draw("col");
  
  gPad->SetLogz();
  
  outName = outFolder + "hD_Min_Peak.pdf";
  canvas->SaveAs(outName.c_str());

  gPad->SetGrid(0,0);
  
  gPad->SetLogz(false);

  DeleteCanvas();
}


float TConvert::ADC_To_Wave(short ADC){

  float wave_mV = ADC * Get_mVPerBin();
  wave_mV -= 1000;
  
  if(fPulsePol=='N')
    wave_mV = -wave_mV;
  
  return wave_mV;
}

bool TConvert::IsSampleInBaseline(short iSample,
				  short option = 1){
  
  float time  = (float)iSample * SampleToTime();
  float width = 30.;

  switch(option){
  case(0):
    width = width + 20; // pulse at beginning of waveform 
  case(2):
    time = time - fLength_ns + width; // end of waveform
  }
  
  if( time >= 0 && time < width )
    return true;
  else
    return false;
}

void TConvert::Baseline(){
    
  InitBaseline();
  
  // duplicates so be careful of scope:
  // use local_ prefix
  float base_mV_local = 0.;
  float peak_mV_local = -1000.;  
  float min_mV_local  = 1000.;  
  short peak_samp_local = 0;

  short nBaseSamps = 0;
  float wave_mV = 0.;  
  
  for (int iEntry = 0; iEntry < nentries; iEntry++) {
    rawTree->GetEntry(iEntry);
    
    min_mV_local  =  1000.;
    peak_mV_local = -1000.;
    peak_samp_local = 0;  

    base_mV_local = 0.;
    nBaseSamps   = 0;

    for (short iSamp = 0; iSamp < fNSamples; ++iSamp){
      wave_mV = ADC_To_Wave(ADC->at(iSamp));
      
      if(wave_mV >= peak_mV_local){
	peak_mV_local  = wave_mV;
	peak_samp_local = iSamp;
      }
      
      if(wave_mV < min_mV_local){
	min_mV_local = wave_mV;
      }
            
      if(IsSampleInBaseline(iSamp,1)){
	base_mV_local += wave_mV;
	nBaseSamps++;
      }
    }//end: for (short iSamp =
    
    // recalculate baseline if peak was in 
    // standard baseline region
    if( IsSampleInBaseline(peak_samp_local,0) ){
      base_mV_local = 0.;
      nBaseSamps = 0;
      for (short iSamp = 0; iSamp < fNSamples; ++iSamp){
	wave_mV = ADC_To_Wave(ADC->at(iSamp));
	
	if(IsSampleInBaseline(iSamp,2)){
	  base_mV_local += wave_mV;
	  nBaseSamps++;
	}
	
      }
    }
    
    base_mV_local /= (float)nBaseSamps;
    hBase->Fill(base_mV_local);

    peak_mV_local -= base_mV_local;
    min_mV_local  -= base_mV_local;
    
    hPeak->Fill(peak_mV_local);
       
    hBase_Peak->Fill(base_mV_local,peak_mV_local);
    hMin_Peak->Fill(min_mV_local,peak_mV_local);
  
    if( iEntry > 0 && iEntry < 10000 )
      hEvent_Base->Fill(iEntry,base_mV_local);
    
    // printf("\n event  = %d  \n ",iEntry);
    // printf("\n min_mV_local = %f  \n ",min_mV_local);
    
  }// end: for (int iEntry 
  
  SaveBaseline();
  
}

void TConvert::InitBaseline(){
  
  printf("\n ------------------------------ \n");
  printf("\n Getting Baseline Info        \n\n");
  
  float mVPerBin = Get_mVPerBin();
  float min_mV   = -GetRange_mV()/2.;
  float max_mV   =  GetRange_mV()/2.;
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
  
  hMin_Peak = new TH2F("hMin_Peak",
		       "hMin_Peak;min voltage (mV);peak voltage (mV)",
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

void TConvert::SaveBaseline(string outFolder){

  InitCanvas();

  TLegend *leg = new TLegend(0.21,0.2,0.31,0.9);
    
  //  char title[128] = "";
  leg->SetTextSize(0.025);
  leg->SetHeader("Baseline start","C");
  
  //   Int_t colors[] = {kRed-7,kRed,kRed+2,
  //     kRed-5,kOrange,kOrange+2,
  //     kOrange+4,kYellow+1,kGreen+1,
  //     kGreen+3,kGreen-5,kCyan+1,
  //     kCyan+3,kBlue-7,kBlue,
  //     kBlue+3,kViolet,kMagenta+1};
  
  leg->SetMargin(0.4); 


  hBase->SetAxisRange(-25., 25.,"X");
  hBase->SetMinimum(0.1);

  hBase->Draw();
  
  //  sprintf(title,"%.0f ns",iBase*10.);
  //  leg->AddEntry(hBase[iBase],title,"L");  
  //  leg->Draw();
  
  gPad->SetLogy();
  string outName = outFolder + "hBase.pdf";
  canvas->SaveAs(outName.c_str());
  
  hPeak->SetAxisRange(-5., 75.,"X");
  hPeak->SetMinimum(0.1);
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
  
  hMin_Peak->SetAxisRange(-25.,25.,"X");
  hMin_Peak->SetAxisRange(-5., 45.,"Y");
  
  gPad->SetGrid(1, 1);
  hMin_Peak->Draw("col");
  
  TLine * lVert = new TLine(-2.5,0,-2.5,10.);
  TLine * lDiag = new TLine(-5,10,-20,40.);
  
  lVert->SetLineStyle(2);
  lVert->SetLineColor(kBlue);
  lVert->SetLineWidth(2);

  lDiag->SetLineStyle(2);
  lDiag->SetLineColor(kBlue);
  lDiag->SetLineWidth(2);

  gPad->SetLogz();
    
  lVert->Draw();
  lDiag->Draw();
  
  outName = outFolder + "hMin_Peak.pdf";
  canvas->SaveAs(outName.c_str());

  gPad->SetGrid(0,0);
  
  gPad->SetLogz(false);

  float w = 10000., h = 100.;
  canvas->SetWindowSize(w,h);
  
  gPad->SetGrid(1, 1);
  gPad->SetLogz();
  
  float base_mean = 0.0;
  float minX = -1.5, maxX = 1.5;
  base_mean = hBase->GetMean(); 
  minX = base_mean - 1.5;
  maxX = base_mean + 1.5;
  
  hEvent_Base->SetAxisRange(minX,maxX,"Y");
  hEvent_Base->Draw("col");

  outName = outFolder + "hEvent_Base.pdf";
  canvas->SaveAs(outName.c_str());

  gPad->SetGrid(0, 0);
  w = 1000.;
  h = 800.;
  canvas->SetWindowSize(w,h);
  gPad->SetLogz(false);

  DeleteCanvas();
}

// Fix histogram binning
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

void TConvert::InitCanvas(float w,float h){

  canvas = new TCanvas();
  canvas->SetWindowSize(w,h);

}

void TConvert::DeleteCanvas(){
  delete canvas;
}

double TConvert::GetTrigTimeTag() {

  unsigned int TTT;
   
  // limit to 31 bits 
  if( HEAD[5] > (unsigned int)INT_MAX )
    TTT = HEAD[5] - INT_MAX;
  else
    TTT = HEAD[5];
  
  return (double)TTT*8.E-9;
}

double TConvert::GetTrigTimeTag(int entry) {

  b_HEAD->GetEntry(entry);

  return GetTrigTimeTag();
}

double TConvert::GetElapsedTime(int * cycles,
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

void TConvert::CountMissedEvents(int dTrigEntry){

  if( dTrigEntry != 1 ){
    nMissedEvents += (dTrigEntry-1);
    printf("\n %d missed events \n",nMissedEvents);
  }
  
}

void TConvert::PrintVec(vector<short> & v) {

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

float TConvert::GetRange_mV(){
  return fRange_V*1000.;
}

float TConvert::Get_mVPerBin(){
  return f_mVPerBin;
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
  
  rawTree->GetEntry(0);   
  
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


void TConvert::Noise(){

  InitNoise();
  
  for (int iEntry = 0; iEntry < nentries; iEntry++) {
    cookedTree->GetEntry(iEntry);

    //printf(" \n mean_cook_mV = %f \n", mean_cook_mV);
    
    hMean_Cooked->Fill(mean_cook_mV);
    hPPV_Cooked->Fill(ppV_cook_mV);
    hMax_Cooked->Fill(max_cook_mV);
    hMin_Cooked->Fill(min_cook_mV);
    hMin_Max_Cooked->Fill(min_cook_mV,max_cook_mV);
    
   }// end: for (int iEntry = 0;

  SaveNoise();

}

void TConvert::InitNoise(){
  
  printf("\n ------------------------------ \n");
  printf("\n Getting Noise Info           \n\n");

  float mVPerBin =  Get_mVPerBin();
  float minX     = -GetRange_mV()/2.;
  float maxX     =  GetRange_mV()/2.;
  int   nBins    = 0;
  
  // fix binning and set number of bins
  Set_THF_Params(&minX,&maxX,&mVPerBin,&nBins);
  
  hMean_Cooked = new TH1F("hMean_Cooked",
		   ";mean voltage (mV) [cooked];Counts",
		   nBins,minX,maxX);

  hMax_Cooked =  new TH1F("hMax_Cooked",
		   ";raw max voltage (mV) [cooked];Counts",
		   nBins,minX,maxX);
  
  hMin_Cooked =  new TH1F("hMin_Cooked",
		   ";raw min voltage (mV) [cooked];Counts",
		   nBins,minX,maxX);
  
  //   printf("\n nBins    = %d \n",nBins);
//   printf("\n minX   = %f \n",minX);
//   printf("\n maxX   = %f \n",maxX);
//   printf("\n mVPerBin = %f \n",mVPerBin);
//   printf("\n Get_mVPerBin() = %f \n",Get_mVPerBin());

  hMin_Max_Cooked =  new TH2F("hMin_Max_Cooked",
			      "max vs min before baseline sub.; min voltage (mV) [cooked];max voltage (mV) [cooked]",
			      nBins,minX,maxX,
			      nBins,minX,maxX);

  // prepare for range starting at zero
  minX = 0.0;
  maxX = GetRange_mV()/2.;
  nBins  = 0;
  
  Set_THF_Params(&minX,&maxX,&mVPerBin,&nBins);

  hPPV_Cooked =  new TH1F("hPPV_Cooked",
		   ";peak to peak voltage (mV) [cooked];Counts",
		   nBins,minX,maxX);

}


void TConvert::SaveNoise(string outFolder){

  InitCanvas();
  
  gPad->SetLogy();
  
  hMean_Cooked->SetAxisRange(-100., 100.,"X");
  hMean_Cooked->SetMinimum(0.1);
  hMean_Cooked->Draw();

  string outName = outFolder + "hMean_Cooked.pdf";
  canvas->SaveAs(outName.c_str());  
  
  hPPV_Cooked->SetAxisRange(-50.0, 250.,"X");
  hPPV_Cooked->SetMinimum(0.1);
  hPPV_Cooked->Draw();
  
  outName = outFolder + "hPPV_Cooked.pdf";
  canvas->SaveAs(outName.c_str());
  
  hMax_Cooked->SetAxisRange(-50., 100.,"X");
  hMax_Cooked->SetMinimum(0.1);
  hMax_Cooked->Draw();
  outName = outFolder + "hMax_Cooked.pdf";
  canvas->SaveAs(outName.c_str());
  
  hMin_Cooked->SetAxisRange(-100., 50.,"X");
  hMin_Cooked->SetMinimum(0.1);
  hMin_Cooked->Draw();
  outName = outFolder + "hMin_Cooked.pdf";
  canvas->SaveAs(outName.c_str());
  
  gPad->SetLogy(false);
  gPad->SetLogz();
  
  hMin_Max_Cooked->SetAxisRange(-15., 15.,"X");
  hMin_Max_Cooked->SetAxisRange(-15., 50.,"Y");
  
  hMin_Max_Cooked->Draw("colz");

  outName = outFolder + "hMin_Max_Cooked.pdf";
  canvas->SaveAs(outName.c_str());

  gPad->SetLogz(false);
  
  DeleteCanvas();
  
}

 void TConvert::DAQ()
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

  int    deltaEvents = 0;
  double  deltaT      = 0.;
  double dTime       = 0.; // time between events

  double  eventRate = 0.;
  double meanRate  = 0.;

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

void TConvert::InitDAQ(){
  
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

void TConvert::SaveDAQ(string outFolder){
  
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
