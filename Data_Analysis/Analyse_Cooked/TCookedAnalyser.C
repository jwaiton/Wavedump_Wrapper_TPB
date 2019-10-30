#define TCookedAnalyser_cxx
#include "TCookedAnalyser.h"
#include <TH2.h>
#include <TF1.h>
#include <math.h>
#include <limits.h>

#include "wmStyle.C"

string TCookedAnalyser::GetCookedTreeID(){
  return "Cooked_" + FileID;  
}

string TCookedAnalyser::GetMetaTreeID(){
  return "Meta_" + FileID;  
}

string TCookedAnalyser::GetFileID(){
  return FileID;
}

float TCookedAnalyser::Get_LED_delay(){
  
  printf("\n ------------------------------ \n");
  printf("\n Getting LED delay   \n");

  InitCanvas();
    
  float range    = Length_ns;
  float binWidth = nsPerSamp;
  float min_time = 0.0;
  float max_time = range;
  int   nBins = 0;
  
  Set_THF_Params(&min_time,&max_time,&binWidth,&nBins);
  
  TH1F * hPeakTime = new TH1F("hPeakTime",
			      "hPeakTime;peak time (ns);Counts",
			      nBins,min_time,max_time);
  float LED_delay   = 1.0;
  float delay_width = 1.0;
  float peak_time = 0.;
  float thresh_mV = 10.;
  
  for (int iEntry = 0; iEntry < nentries; iEntry++) {
    cookedTree->GetEntry(iEntry);
    
    peak_time = peak_samp * nsPerSamp;
    
    if(peak_mV > thresh_mV)
      hPeakTime->Fill(peak_time);

  }
  
  hPeakTime->Fit("gaus","Q","",min_time,max_time);
  
  TF1 * fPeak = hPeakTime->GetFunction("gaus");
  LED_delay   = fPeak->GetParameter(1);
  delay_width = fPeak->GetParameter(2);
  
  float min_delay   = LED_delay - 3.*delay_width; 
  float max_delay   = LED_delay + 3.*delay_width; 

  hPeakTime->Fit("gaus","QR","",min_delay,max_delay);
  
  hPeakTime->Draw();  
  canvas->SaveAs("./Plots/Timing/hPeakTime.pdf");
 
  DeleteCanvas();

  LED_delay   = hPeakTime->GetFunction("gaus")->GetParameter(1);
  delay_width = hPeakTime->GetFunction("gaus")->GetParameter(2);
  
  //printf(" delay = %.1f (%.1f)", LED_delay, delay_width);

  return LED_delay;
}

//------------------------------
void TCookedAnalyser::Noise(){
  
  InitNoise();
  
  for (int iEntry = 0; iEntry < nentries; iEntry++) {
    cookedTree->GetEntry(iEntry);

    //printf(" \n mean_mV = %f \n", mean_mV);
    
    hMean_Cooked->Fill(mean_mV);
    hPPV_Cooked->Fill(peak_mV-min_mV);
    hPeak_Cooked->Fill(peak_mV);
    hMin_Cooked->Fill(min_mV);
    hMin_Peak_Cooked->Fill(min_mV,peak_mV);
    
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
  noise_rate = noise_rate/Length_ns * 1.0e9;

  // low threshold rel mean peak
  thresh_bin   = hMin_Cooked->FindBin(noise_th_low_mV);
  noise_counts = hMin_Cooked->Integral(0,thresh_bin);
  
  float noise_rate_low = (float)noise_counts/nentries;
  noise_rate_low = noise_rate_low/Length_ns * 1.0e9;
  
  printf("\n Mean voltage %.2f mV \n",peak_mean_mV);
  printf("\n Noise Rate @ %.2f mV  %.2f Hz \n",noise_th_low_mV,noise_rate_low);
  printf("\n Noise Rate @ %.2f mV  %.2f Hz \n",noise_thresh_mV,noise_rate);

  SaveNoise();

}

void TCookedAnalyser::InitNoise(){
  
  printf("\n ------------------------------ \n");
  printf("\n Analysing Noise   \n");
  
  float range = (float)roundf(Range_V)*1000.;
  
  float binWidth =  mVPerBin;
  float minX     = -range/2.;
  float maxX     =  range/2.;
  int   nBins    = 0;
  
  // fix binning and set number of bins
  Set_THF_Params(&minX,&maxX,&binWidth,&nBins);
  
  hMean_Cooked = new TH1F("hMean_Cooked",
			  ";mean voltage (mV);Counts",
			  nBins,minX,maxX);

  hPeak_Cooked =  new TH1F("hPeak_Cooked",
			   ";peak voltage (mV);Counts",
			   nBins,minX,maxX);
  
  hMin_Cooked =  new TH1F("hMin_Cooked",
			  ";min voltage (mV);Counts",
			  nBins,minX,maxX);
  
  //   printf("\n nBins    = %d \n",nBins);
//   printf("\n minX   = %f \n",minX);
//   printf("\n maxX   = %f \n",maxX);
//   printf("\n binWidth = %f \n",binWidth);
//   printf("\n mVPerBin = %f \n",mVPerBin);

  hMin_Peak_Cooked =  new TH2F("hMin_Peak_Cooked",
			       "peak vs min ; min voltage (mV);peak voltage (mV)",
			       nBins,minX,maxX,
			       nBins,minX,maxX);

  // prepare for range starting at zero
  minX = 0.0;
  maxX = range/2.;
  nBins  = 0;
  
  Set_THF_Params(&minX,&maxX,&binWidth,&nBins);
  
  hPPV_Cooked =  new TH1F("hPPV_Cooked",
			  ";peak to peak voltage (mV);Counts",
			  nBins,minX,maxX);

}


void TCookedAnalyser::SaveNoise(string outFolder){

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
  
  hPeak_Cooked->SetAxisRange(-20.,80.,"X");
  hPeak_Cooked->SetMinimum(0.1);
  hPeak_Cooked->Draw();
  outName = outFolder + "hPeak_Cooked.pdf";
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
  
  hMin_Peak_Cooked->SetAxisRange(-25., 15.,"X");
  hMin_Peak_Cooked->SetAxisRange(-15., 50.,"Y");
  
  hMin_Peak_Cooked->Draw("colz");

  outName = outFolder + "hMin_Peak_Cooked.pdf";
  canvas->SaveAs(outName.c_str());

  gPad->SetLogz(false);
  
  DeleteCanvas();
  
}


void TCookedAnalyser::Dark(float thresh_mV){
  
  InitDark();
  
  int nDark = 0;
  int nDark_noise = 0;

  for (int iEntry = 0; iEntry < nentries; iEntry++) {
    cookedTree->GetEntry(iEntry);
     
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
  darkRate = darkRate/Length_ns * 1.0e9;
  
  printf("\n \n nentries = %d \n",nentries);
  printf("\n dark counts (noise rejected) = %d \n",nDark);
  printf("\n dark rate   (noise rejected) = %.0f \n",darkRate);
  
  darkRate = (float)nDark_noise/nentries;
  darkRate = darkRate/Length_ns * 1.0e9;
  
  printf("\n dark counts (with noise)     = %d \n",nDark_noise);
  printf("\n dark rate   (with noise)     = %.0f \n\n",darkRate);

  SaveDark();
  
}

void TCookedAnalyser::InitDark(){
  
  printf("\n ------------------------------ \n");
  printf("\n Dark Counts Analysis           \n");
    
  float range = (float)roundf(Range_V)*1000.;

  float max      =  range/2;
  float min      = -range/2;
  float binWidth = mVPerBin;
  int   nBins    = 0;

  //  fix binning and set number of bins
  Set_THF_Params(&min,&max,&binWidth,&nBins);
  
//   hBase = new TH1F("hBase",
// 		   "hBase;baseline voltage (mV);Counts",
// 		   nBins,min,max);
  
  hD_Peak = new TH1F("hD_Peak",
		     "hD_Peak;peak voltage (mV);Counts",
		     nBins,min,max);
  
//   hBase_Peak = new TH2F("hBase_Peak",
// 			"hBase_Peak;baseline voltage (mV);peak voltage (mV)",
// 			nBins,min,max,
// 			nBins,min,max);
  
  hD_Min_Peak = new TH2F("hD_Min_Peak",
			 "hD_Min_Peak;min voltage (mV);peak voltage (mV)",
			 nBins,min,max,
			 nBins,min,max);

}


void TCookedAnalyser::SaveDark(string outFolder){

  InitCanvas();

  TLegend *leg = new TLegend(0.21,0.2,0.31,0.9);
    
  leg->SetTextSize(0.025);
  leg->SetHeader("Baseline start","C");
  
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
  
  
//   outName = outFolder + "hBase_Peak.pdf";
//   canvas->SaveAs(outName.c_str());

  gPad->SetLogz();
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

float TCookedAnalyser::ADC_To_Wave(short ADC){

  float wave = ADC * mVPerBin;

  wave -= Range_V*1000./2.;

  return wave;
}


//------------------------------
void TCookedAnalyser::Waveform(char option){

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
  
  int  entry  = 0;
  char answer = 'R';
  
  // Plotting Loop
  while ( answer!='X' ){
    
    printf("\n Which entry to plot? \n");
    printf("\n F - First entry \n");
    printf("\n N - Next \n");
    printf("\n P - Previous \n");
    printf("\n R - Random selection \n");
    printf("\n X - eXit with no plotting \n");
    
    // note deliberate use of whitespace before %c
    scanf(" %c", &answer);
    
    switch(answer){
    case('F'):
      entry = 0;
      break;
    case('R'):
      entry = (int)round(rand3->Uniform(nentries)); ;
      break;
    case('N'):
      entry++;
      break;
    case('P'):
      entry--;
    default:
      entry = -1;
    }
    
    if(entry > -1)
      printf("\n plotting entry %d \n",entry);
    else{
      printf("\n exiting waveform plotting \n");
      return;
    }
    
    cookedTree->GetEntry(entry);
    
    for( short iSamp = 0 ; iSamp < NSamples; iSamp++)
      hWave->SetBinContent(iSamp+1,(ADC_To_Wave(ADC->at(iSamp))));
    
    hWave->FFT(hFFT ,"MAG");
    
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
  
}

void TCookedAnalyser::InitWaveform(){
  
  printf("\n ------------------------------ \n");
  printf("\n Plotting Waveform \n\n");
  
  hWave = new TH1F("hWave","Waveform;Time (ns); Amplitude (mV)",
		   NSamples, 0.,Length_ns);
  
}

void TCookedAnalyser::InitFFT(){
  
  printf("\n ------------------------------ \n");
  printf("\n Plotting FFT \n\n");

  hFFT = new TH1F("hFFT","FFT; Frequency (MHz); Magnitude",
		  NSamples/2, 0, SampFreq/2 );
  
}


void TCookedAnalyser::SaveWaveform(string outFolder){

  printf("\n Saving Waveform Plot \n\n");
  
  InitCanvas();
  
  hWave->Draw();
  
  string outName = outFolder + "hWave.pdf";
  
  canvas->SaveAs(outName.c_str());
  
  DeleteCanvas();
  
}

void TCookedAnalyser::SaveFFT(string outFolder){

  printf("\n Saving FFT Plot \n\n");
  
  InitCanvas();
  
  hFFT->SetBinContent(1,0.);
  hFFT->Draw();
  
  string outName = outFolder + "hFFT.pdf";
  
  canvas->SaveAs(outName.c_str());
  
  DeleteCanvas();
  
}

void TCookedAnalyser::SaveWaveFFT(string outFolder){

  printf("\n Saving Waveform and FFT Plots \n\n");
  
  InitCanvas(1600.);
  
  canvas->Divide(2,1);
  
  canvas->cd(1);
  hWave->Draw();
  
  canvas->cd(2);
  hFFT->SetBinContent(1,0.);
  hFFT->Draw();
  
  string outName = outFolder + "hWaveFFT.pdf";
  
  canvas->SaveAs(outName.c_str());
  
  string sysCommand = "evince ";
  sysCommand += outName;
  sysCommand += " &";
  system(sysCommand.c_str());

  DeleteCanvas();
  
}




//------------------------------
//Fix histogram binning
void TCookedAnalyser::Set_THF_Params(float * minX, 
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

void TCookedAnalyser::InitCanvas(float w,float h){
  
  canvas = new TCanvas();
  canvas->SetWindowSize(w,h);
  
}

void TCookedAnalyser::DeleteCanvas(){
  delete canvas;
}

void TCookedAnalyser::PrintMetaData(){ 

  printf("\n ------------------------------ \n");
  printf("\n Printing Meta Data \n");

  if (!metaTree) return;
  metaTree->Show(0);
  
}

void TCookedAnalyser::SetStyle(){
  
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
