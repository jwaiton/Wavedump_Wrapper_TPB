#define TAnalyserTemplate_cxx
#include "TAnalyserTemplate.h"

#include "TSystem.h"
#include <TH2.h>
#include <TF1.h>
#include <TLine.h>
#include <math.h>
#include <limits.h>

#include "wmStyle.C"

//----------
//   

// An example of how to access the data at the event 
// and sample level
// The event level variables are 
void TAnalyserTemplate::Loop_Over_Samples(int verbosity = 2){
  
  float wave_mV = 0.0;
  float time_ns = 0.0;

  // comment out below to silence sample-by-sample printing 
  // verbosity = 1; 
  
  for (int iEntry = 0; iEntry < nentries; iEntry++) {
    cookedTree->GetEntry(iEntry);
    
    // print out event-by-event variables from TTree
    if( verbosity > 0 ){
      cout << endl;
      cout << " entry  =  " << iEntry  << endl;
      cout << " start_s = " << start_s << endl;
      cout << " peak_mV = " << peak_mV << endl;
    }
    
    for( short iSamp = 0 ; iSamp < NSamples; iSamp++){
  
      // the sample time is calculated
      // from the sample number 
      time_ns = iSamp * nsPerSamp;
      
      // the voltage for each sample is
      // calculated from the ADC values
      wave_mV = ADC_To_Wave(ADC->at(iSamp));
      
      // print out from 
      if( verbosity > 1 ){
	cout << endl;
	cout << " sample  = " << iSamp  << endl;
	cout << " time_ns = " << time_ns << endl;
	cout << " wave_mV = " << wave_mV << endl;
      }
      
    } // end of: for( short iSamp = 0 ; iSam
    
  } // end of: for (int iEntry = 0; iEn
  
}

//
void TAnalyserTemplate::Pulse_Height(float thresh_mV){
  
  Init_Pulse_Height();
  
  int nOverThreshold = 0;

  for (int iEntry = 0; iEntry < nentries; iEntry++) {
    cookedTree->GetEntry(iEntry);
     
    hPulse_Height->Fill(peak_mV);
    
    if( peak_mV < thresh_mV)
      continue;
    
    nOverThreshold++;
    
  }// end: for (int iEntry 

  float rateOverThresh = (float)nOverThreshold/nentries;
  rateOverThresh = rateOverThresh/Length_ns * 1.0e9;
  
  printf("\n \n nentries = %d \n",nentries);
  printf("\n counts over threshold = %d \n",nOverThreshold);
  printf("\n rate over threshold   = %.0f \n",rateOverThresh);
  
  // save plot in ./Plots folder
  Save_Pulse_Height();
  
}

void TAnalyserTemplate::Init_Pulse_Height(){
  
  printf("\n ------------------------------ \n");
  printf("\n Pulse Height Analysis          \n");
  
  // range in mV
  float range = (float)roundf(Range_V)*1000.;
  
  float max      =  range/2;
  float min      = -range/2;
  float binWidth = Wave_To_Amp_Scaled_Wave(mVPerBin);
  int   nBins    = 0;
  
  //  fix binning and set number of bins
  Set_THF_Params(&min,&max,&binWidth,&nBins);
  
  hPulse_Height = new TH1F("hPulse_Height",
			   "hPulse_Height;peak voltage (mV);Counts",
			   nBins,min,max);
}


void TAnalyserTemplate::Save_Pulse_Height(string outPath){

  string sys_command = "mkdir -p ";
  sys_command += outPath;
  gSystem->Exec(sys_command.c_str());

  InitCanvas();

  TLegend *leg = new TLegend(0.21,0.2,0.31,0.9);
    
  leg->SetTextSize(0.025);
  leg->SetHeader("Baseline start","C");
  
  leg->SetMargin(0.4); 

  gPad->SetLogy();
  
  hPulse_Height->SetAxisRange(-5., 75.,"X");
  hPulse_Height->SetMinimum(0.1);
  hPulse_Height->Draw();
  
  TLine * lVert = new TLine(10,0,10,20);
  lVert->SetLineColor(kBlue);
  lVert->SetLineWidth(2);
  lVert->SetLineStyle(2);
  lVert->Draw();

  string outName = outPath + "hPulse_Height.pdf";
  canvas->SaveAs(outName.c_str());

  DeleteCanvas();
}

float TAnalyserTemplate::ADC_To_Wave(short ADC){

  float wave = ADC * mVPerBin;

  wave -= Range_V*1000./2.;
  
  wave = Wave_To_Amp_Scaled_Wave(wave);
  
  return wave;
}

float TAnalyserTemplate::Wave_To_Amp_Scaled_Wave(float wave){
  return wave/AmpGain*10.;
}


//------------------------------
//Fix histogram binning
void TAnalyserTemplate::Set_THF_Params(float * minX, 
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

void TAnalyserTemplate::InitCanvas(float w,float h){
  
  canvas = new TCanvas();
  canvas->SetWindowSize(w,h);
  
}

void TAnalyserTemplate::DeleteCanvas(){
  delete canvas;
}

void TAnalyserTemplate::PrintMetaData(){ 

  printf("\n ------------------------------ \n");
  printf("\n Printing Meta Data \n");

  if (!metaTree) return;
  metaTree->Show(0);
  
}

string TAnalyserTemplate::GetCookedTreeID(){
  string CookedTreeID = FileID;
  return "Cooked_" + CookedTreeID;
}

string TAnalyserTemplate::GetMetaTreeID(){
  return "Meta_Data";
}

string TAnalyserTemplate::GetFileID(){
  return FileID;
}

void TAnalyserTemplate::SetStyle(){
  
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
