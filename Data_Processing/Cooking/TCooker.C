#define TCooker_cxx
#include "TCooker.h"
#include <TH2.h>
#include <math.h>
#include <limits.h>

#include "wmStyle.C"

void TCooker::Cook(){
  
  // initialise trees
  InitCooking();

  // create variables in standard units
  // and find waveform peak
  DoCooking();

  SaveMetaData();
  SaveCookedData();
  
  printf("\n Cooking is complete            \n");
  printf("\n ------------------------------   ");
  printf("\n ------------------------------ \n");


}

void TCooker::InitCooking(){

  printf("\n ------------------------------ \n");
  printf("\n Initialising Cook \n");


  InitCookedDataFile();
  InitMetaDataTree();
  InitCookedDataTree();

}

void TCooker::InitCookedData(){

  printf("\n ------------------------------ \n");
  printf("\n Initialising Cooked Data      \n");
  
  InitCookedDataFile("OPEN");
  ConnectToCookedTree();
  
}


void TCooker::InitCookedDataFile(string option){

  string fileName = GetFileID();
  fileName += ".root";
    
  if     (!strcmp(option.c_str(),"RECREATE")) 
    printf("\n Preparing: ");
  else if(!strcmp(option.c_str(),"OPEN")) 
    printf("\n Opening:   ");
  else   {
    printf("\n Error: option unknown \n");
    return;
  }
    
  printf("\n  %s \n",fileName.c_str());

  outFile = new TFile(fileName.c_str(),
		      option.c_str(),
		      fileName.c_str());

}

void TCooker::SaveMetaData(){
  
  printf("\n ------------------------------ \n");
  printf("\n Writing meta data              \n");   
  
  sprintf(FileID,"%s",f_fileID.c_str());
  
  printf("\n FileID =  %s \n",FileID);

  metaTree->Fill();

  metaTree->Write();
  metaTree->Delete();
  
}

void TCooker::SaveCookedData(){

  
  printf("\n ------------------------------ \n");
  printf("\n Writing cooked data            \n");
  printf("\n Closing:                         ");
  printf("\n   %s       \n",outFile->GetName());
  printf("\n ------------------------------ \n");
    
  cookedTree->Write();
  cookedTree->Delete();
  outFile->Close();

}

void TCooker::InitCookedDataTree(){
  
  // ----------
  // Temporary tree for calibrating
  string treeName = "Cooked_";
  treeName += GetFileID();

  cookedTree = new TTree(treeName.c_str(),treeName.c_str());

  cookedTree->Branch("wave_mV",&wave_buff);
  
  cookedTree->Branch("min_mV",&min_mV,"min_mV/F");
  cookedTree->Branch("max_mV",&max_mV,"max_mV/F");
  cookedTree->Branch("ppV_mV",&ppV_mV,"ppV_mV/F");
  cookedTree->Branch("mean_mV",&mean_mV,"mean_mV/F");
  cookedTree->Branch("peak_samp",&peak_samp,"peak_samp/S");

  
}

void TCooker::InitMetaDataTree(){
  
  // ----------
  // Temporary tree for calibrating
  string treeName = "Meta_";
  treeName += GetFileID();

  metaTree = new TTree(treeName.c_str(),treeName.c_str());
  
  metaTree->Branch("SampFreq",&fSampFreq,"SampFreq/S");
  metaTree->Branch("NSamples",&fNSamples,"NSamples/S");
  metaTree->Branch("NADCBins",&fNADCBins,"NADCBins/I");
  metaTree->Branch("Range_V",&fRange_V,"Range_V/S");
  metaTree->Branch("nsPerSamp",&f_nsPerSamp,"nsPerSamp/F");
  metaTree->Branch("mVPerBin",&f_mVPerBin,"mVPerBin/F");
  metaTree->Branch("Length_ns",&fLength_ns,"Length_ns/F");
  metaTree->Branch("FileID",FileID,"FileID/C");
  
}

void TCooker::DoCooking(){
  
  printf("\n ------------------------------ \n");
  printf("\n Cooking                       \n");
  
  for (int iEntry = 0; iEntry < nentries; iEntry++) {
    rawTree->GetEntry(iEntry);
  
    //printf("\n iEntry = %d  \n ",iEntry);
    wave_buff.clear();
    
    min_mV    =  1000.;
    max_mV    = -1000.;
    ppV_mV    =  0.;
    mean_mV   =  0.;
    peak_samp =  0;
    
    for (short iSamp = 0; iSamp < fNSamples; ++iSamp){
      wave_buff.push_back(ADC_To_Wave(ADC->at(iSamp)));
      
//       printf("\n iSamp     = %d  \n ",iSamp);
//       printf("\n ADC       = %hd \n ",ADC->at(iSamp));
//       printf("\n wave_buff = %f  \n ",wave_buff.at(iSamp));
	
      // min      
      if( wave_buff.at(iSamp) < min_mV)
	min_mV = wave_buff.at(iSamp);
      
      // max, peak_samp
      if( wave_buff.at(iSamp) >= max_mV ){
	max_mV = wave_buff.at(iSamp);
	peak_samp  = iSamp;
      }
      // mean
      mean_mV += wave_buff.at(iSamp);
     }
    
    ppV_mV  = max_mV - min_mV;
    mean_mV = mean_mV/(float)fNSamples;
    
    //printf("\n event       = %d  \n ",iEntry);
    //printf("\n peak_samp   = %d  \n ",peak_samp);
    //printf("\n max_mV  = %f  \n ",max_mV);
    //printf("\n min_mV  = %f  \n ",min_mV);

    cookedTree->Fill();
  }
  
}

void TCooker::CloseCookedFile(){
  
  outFile->Close();  
}

////
void TCooker::SetFileID(){

  //!!! Temporary
  f_fileID = "Run_1_PMT_130_Loc_0_Test_D";
  
  printf("\n Warning: Hard-coding File ID");
  printf("\n \t %s \n ",f_fileID.c_str());
  
}

string TCooker::GetFileID(){
  return f_fileID;
}

string TCooker::GetCookedTreeID(){
  return "Cooked_" + GetFileID();  
}

float TCooker::ADC_To_Wave(short ADC){

  float wave_mV_local = ADC * Get_mVPerBin();
  wave_mV_local -= 1000.;
  
  if(fPulsePol=='N')
    wave_mV_local = -wave_mV_local;
  
  return wave_mV_local;
}

bool TCooker::IsSampleInBaseline(short iSample,
				 short option = 1){
  
  float time  = (float)iSample * SampleToTime();
  float width = 30.;
  
  switch(option){
  case(0):
    width = width + 20; // pulse at beginning of waveform 
    break;
  case(2):
    time = time - fLength_ns + width; // end of waveform
    break;
  }
  
  if( time >= 0 && time < width )
    return true;
  else
    return false;
}

//------------------------------

void TCooker::Baseline(){
    
  InitBaseline();
  
  float base_mV = 0.;
  short nBaseSamps = 0;
  
  for (int iEntry = 0; iEntry < nentries; iEntry++) {
    cookedTree->GetEntry(iEntry);

    nBaseSamps = 0;
    
    // standard baseline region
    if( !IsSampleInBaseline(peak_samp,0) ){
      
      for (short iSamp = 0; iSamp < fNSamples; ++iSamp){
        
	if(IsSampleInBaseline(iSamp,1)){
	  base_mV += wave_mV->at(iSamp);
	  nBaseSamps++;
	}
      }//end: for (short iSamp =
      
    }     
    else{ // recalculate baseline if peak in standard region
      
      for (short iSamp = 0; iSamp < fNSamples; ++iSamp){
	if(IsSampleInBaseline(iSamp,2)){
	  base_mV += wave_mV->at(iSamp);
	  nBaseSamps++;
	}
      }
    }
    
    base_mV /= (float)nBaseSamps;

    hBase->Fill(base_mV);
    
    max_mV -= base_mV;
    min_mV  -= base_mV;
    
    hPeak->Fill(max_mV);
       
    hBase_Peak->Fill(base_mV,max_mV);
    hMin_Peak->Fill(min_mV,max_mV);
  
    if( iEntry > 0 && iEntry < 10000 )
      hEvent_Base->Fill(iEntry,base_mV);
    
    // printf("\n event  = %d  \n ",iEntry);
    // printf("\n min_mV = %f  \n ",min_mV);
    
  }// end: for (int iEntry 
  
  SaveBaseline();
  
}

void TCooker::InitBaseline(){
  
  printf("\n ------------------------------ \n");
  printf("\n Analysing Baseline           \n\n");
  
  float mVPerBin = Get_mVPerBin();
  float low_mV   = -GetRange_mV()/2.;
  float high_mV   =  GetRange_mV()/2.;
  int   nBins    = 0;
  
  // fix binning and set number of bins
  Set_THF_Params(&low_mV,&high_mV,&mVPerBin,&nBins);
  
  hBase = new TH1F("hBase",
		   "hBase;baseline voltage (mV);Counts",
		   nBins,low_mV,high_mV);
  
  hPeak = new TH1F("hPeak",
		   "hPeak;peak voltage (mV);Counts",
		   nBins,low_mV,high_mV);
  
  hBase_Peak = new TH2F("hBase_Peak",
			"hBase_Peak;baseline voltage (mV);peak voltage (mV)",
			nBins,low_mV,high_mV,
			nBins,low_mV,high_mV);
  
  hMin_Peak = new TH2F("hMin_Peak",
		       "hMin_Peak;min voltage (mV);peak voltage (mV)",
		       nBins,low_mV,high_mV,
		       nBins,low_mV,high_mV);

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
			 nBins,low_mV,high_mV);
 
}

void TCooker::SaveBaseline(string outFolder){

  printf("\n Saving Baseline Study Plots \n\n");

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
  hBase_Peak->SetAxisRange(-5., 65.,"Y");
  
  hBase_Peak->Draw("col");
  
  gPad->SetLogz();
 
  outName = outFolder + "hBase_Peak.pdf";
  canvas->SaveAs(outName.c_str());
  
  hMin_Peak->SetAxisRange(-25.,5.,"X");
  hMin_Peak->SetAxisRange(-5., 65.,"Y");
  
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

//------------------------------
// Fix histogram binning
void TCooker::Set_THF_Params(float * minX, 
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

void TCooker::InitCanvas(float w,float h){

  canvas = new TCanvas();
  canvas->SetWindowSize(w,h);

}

void TCooker::DeleteCanvas(){
  delete canvas;
}

double TCooker::GetTrigTimeTag() {

  unsigned int TTT;
   
  // limit to 31 bits 
  if( HEAD[5] > (unsigned int)INT_MAX )
    TTT = HEAD[5] - INT_MAX;
  else
    TTT = HEAD[5];
  
  return (double)TTT*8.E-9;
}

double TCooker::GetTrigTimeTag(int entry) {

  b_HEAD->GetEntry(entry);

  return GetTrigTimeTag();
}

double TCooker::GetElapsedTime(int * cycles,
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

void TCooker::CountMissedEvents(int dTrigEntry){

  if( dTrigEntry != 1 ){
    nMissedEvents += (dTrigEntry-1);
    printf("\n %d missed events \n",nMissedEvents);
  }
  
}

float TCooker::GetLength_ns(){
  return fLength_ns;
}

short TCooker::GetNSamples(){
  return fNSamples;
}

// private
void TCooker::SetDigitiser(char digitiser){

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

float TCooker::GetRange_mV(){
  return (float)fRange_V*1000.;
}

float TCooker::Get_mVPerBin(){
  return f_mVPerBin;
}

void TCooker::SetSampSet(char sampSet){
  
  if  (fDigitiser=='V')
    fSampSet = 'V';
  else
    fSampSet = sampSet;
  
  return;
}

void TCooker::SetPulsePol(char pulsePol){
  
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

void TCooker::SetConstants(){
  
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

void TCooker::PrintConstants(){ 
  
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
void TCooker::Noise(){

  InitNoise();
  
  for (int iEntry = 0; iEntry < nentries; iEntry++) {
    cookedTree->GetEntry(iEntry);

    //printf(" \n mean_mV = %f \n", mean_mV);
    
    hMean_Cooked->Fill(mean_mV);
    hPPV_Cooked->Fill(ppV_mV);
    hMax_Cooked->Fill(max_mV);
    hMin_Cooked->Fill(min_mV);
    hMin_Max_Cooked->Fill(min_mV,max_mV);
    
   }// end: for (int iEntry = 0;
  
  // find peak of mean voltage in mV
  int     max_bin_mean = hMean_Cooked->GetMaximumBin();
  TAxis * x_axis       = hMean_Cooked->GetXaxis();
  float   peak_mean_mV = x_axis->GetBinCenter(max_bin_mean);
  
  thresh_mV       = 10.0; // ideally 1/4 of 1 p.e.
  th_low_mV       = 5.0;  // 
  
  noise_thresh_mV = peak_mean_mV - thresh_mV;
  noise_th_low_mV = peak_mean_mV - th_low_mV;
  
  // standard threshold rel mean peak
  int thresh_bin   = hMin_Cooked->FindBin(noise_thresh_mV);
  int noise_counts = hMin_Cooked->Integral(0,thresh_bin);
  
  float noise_rate = (float)noise_counts/nentries;
  noise_rate = noise_rate/fLength_ns * 1.0e9;

  // low threshold rel mean peak
  thresh_bin   = hMin_Cooked->FindBin(noise_th_low_mV);
  noise_counts = hMin_Cooked->Integral(0,thresh_bin);
  
  float noise_rate_low = (float)noise_counts/nentries;
  noise_rate_low = noise_rate_low/fLength_ns * 1.0e9;
  
  printf("\n Mean voltage %.2f mV \n",peak_mean_mV);
  printf("\n Noise Rate @ %.2f mV \t %.2f Hz \n",noise_th_low_mV,noise_rate_low);
  printf("\n Noise Rate @ %.2f mV \t %.2f Hz \n",noise_thresh_mV,noise_rate);

  SaveNoise();

}

void TCooker::InitNoise(){
  
  printf("\n ------------------------------ \n");
  printf("\n Analysing Noise   \n");

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


void TCooker::SaveNoise(string outFolder){

  printf("\n Saving Noise Monitoring Plots \n\n");

  InitCanvas();
  
  gPad->SetLogy();
  
  hMean_Cooked->SetAxisRange(-30., 120.,"X");
  hMean_Cooked->SetMinimum(0.1);
  hMean_Cooked->Draw();

  string outName = outFolder + "hMean_Cooked.pdf";
  canvas->SaveAs(outName.c_str());  
  
  hPPV_Cooked->SetAxisRange(-5.0, 145.,"X");
  hPPV_Cooked->SetMinimum(0.1);
  hPPV_Cooked->Draw();
  
  outName = outFolder + "hPPV_Cooked.pdf";
  canvas->SaveAs(outName.c_str());
  
  hMax_Cooked->SetAxisRange(-20.,80.,"X");
  hMax_Cooked->SetMinimum(0.1);
  hMax_Cooked->Draw();
  outName = outFolder + "hMax_Cooked.pdf";
  canvas->SaveAs(outName.c_str());
  
  
  hMin_Cooked->SetAxisRange(-30.,20.,"X");
  hMin_Cooked->SetMinimum(0.1);
  hMin_Cooked->Draw();

  TLine * l_thresh = new TLine(noise_thresh_mV,1,noise_thresh_mV,1000);
  l_thresh->SetLineStyle(2);
  l_thresh->SetLineColor(kRed);
  l_thresh->SetLineWidth(2);
  l_thresh->Draw();
  
  TLine * l_th_low = new TLine(noise_th_low_mV,1,noise_th_low_mV,1000);
  l_th_low->SetLineStyle(2);
  l_th_low->SetLineColor(kBlue);
  l_th_low->SetLineWidth(2);
  l_th_low->Draw();
  
  outName = outFolder + "hMin_Cooked.pdf";
  canvas->SaveAs(outName.c_str());
  
  gPad->SetLogy(false);
  gPad->SetLogz();
  
  hMin_Max_Cooked->SetAxisRange(-25., 15.,"X");
  hMin_Max_Cooked->SetAxisRange(-15., 50.,"Y");
  
  hMin_Max_Cooked->Draw("colz");

  outName = outFolder + "hMin_Max_Cooked.pdf";
  canvas->SaveAs(outName.c_str());

  gPad->SetLogz(false);
  
  DeleteCanvas();
  
}


//------------------------------
void TCooker::Waveform(char option){

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
    break;
    SaveFFT();
  case('b'):
    SaveWaveFFT();
    break;
  default:
    break;
  }
  
}

void TCooker::InitWaveform(){
  
  printf("\n ------------------------------ \n");
  printf("\n Plotting Waveform \n\n");
  
  hWave = new TH1F("hWave","Waveform;Time (ns); Amplitude (mV)",
		   fNSamples, 0.,fLength_ns);
  
}

void TCooker::InitFFT(){
  
  printf("\n ------------------------------ \n");
  printf("\n Plotting FFT \n\n");

  hFFT = new TH1F("hFFT","FFT; Frequency (MHz); Magnitude",
		  fNSamples/2, 0, fSampFreq/2 );
  
}


void TCooker::SaveWaveform(string outFolder){

  printf("\n Saving Waveform Plot \n\n");
  
  InitCanvas();
  
  hWave->Draw();
  
  string outName = outFolder + "hWave.pdf";
  
  canvas->SaveAs(outName.c_str());
  
  DeleteCanvas();
  
}

void TCooker::SaveFFT(string outFolder){

  printf("\n Saving FFT Plot \n\n");
  
  InitCanvas();
  
  hFFT->SetBinContent(1,0.);
  hFFT->Draw();
  
  string outName = outFolder + "hFFT.pdf";
  
  canvas->SaveAs(outName.c_str());
  
  DeleteCanvas();
  
}

void TCooker::SaveWaveFFT(string outFolder){

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

 void TCooker::DAQ()
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

void TCooker::InitDAQ(){
  
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

void TCooker::SaveDAQ(string outFolder){
  
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



short TCooker::SetSampleFreq(){
  
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

float TCooker::Set_nsPerSamp(){
  return 1000./fSampFreq;
}

float TCooker::SampleToTime(){
  return f_nsPerSamp;
}

short TCooker::SetNSamples(){
  
  rawTree->GetEntry(0);   
  
  uint hdrByts = 24;
  uint smpByts = HEAD[0] - hdrByts;

  if(fDigitiser=='V')
    return smpByts/2; // shorts
  else
    return smpByts;   // ints
}

float TCooker::SetLength_ns(){
  return f_nsPerSamp*fNSamples;
}

int TCooker::SetNADCBins(){
  
  if( fDigitiser == 'D' )
    return 4096;
  else   
    return 16384;
}

short TCooker::SetRange_V(){

  if(fDigitiser=='V')
    return 2;
  else
    return 1;
  
}

float TCooker::Set_mVPerBin(){
  
  return 1000.*fRange_V/fNADCBins;
  
}


void TCooker::SetStyle(){
  
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
