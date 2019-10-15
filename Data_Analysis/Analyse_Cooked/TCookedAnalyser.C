#define TCookedAnalyser_cxx
#include "TCookedAnalyser.h"
#include <TH2.h>
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

void TCookedAnalyser::Dark(float thresh_mV){
  
   InitDark();
  
  int nDark = 0;
  int nDark_noise = 0;

  for (int iEntry = 0; iEntry < nentries; iEntry++) {
    cookedTree->GetEntry(iEntry);
     
    if(max_mV > thresh_mV)
      nDark_noise++;

    // Noise Rejection 
    if( min_mV < -2.5 && max_mV < thresh_mV)
      continue;
    
    if( max_mV < -2*min_mV && max_mV > thresh_mV )
      continue;
    
    hD_Peak->Fill(max_mV);
    hD_Min_Peak->Fill(min_mV,max_mV);
    
    if( max_mV < thresh_mV)
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
  
//   gPad->SetLogy(false);

//   hBase_Peak->SetAxisRange(-25.,25.,"X");
//   hBase_Peak->SetAxisRange(-5., 45.,"Y");
  
//   hBase_Peak->Draw("col");
  
  gPad->SetLogz();
  
  outName = outFolder + "hBase_Peak.pdf";
  canvas->SaveAs(outName.c_str());

  
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
