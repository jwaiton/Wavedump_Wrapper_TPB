#include <TH1.h>
#include <TF1.h>
#include <TMath.h>
#include <string.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TRandom.h>
//#include <../Data_Analysis/Fitting/Poisson.C>

#include "wmStyle.C"

void SetStyle();
void Set_THF_Params(float *, float *, float *, int *);

TH1F * Generate_Waveform(float npe   = 1.,
			 float width = 5.,
			 float mean  = 100.){
  
  int   NSamples = 0;
  float minX = 0., maxX = 298., nsPerSample = 2.;
  
  Set_THF_Params(&minX,&maxX,&nsPerSample,&NSamples);
  
  TH1F * hW = new TH1F("hW",
		       "Waveform;Time (ns); Voltage (mV)",
		       NSamples, minX, maxX);
  TF1 * f = nullptr;
  
  // Generate charge from Poisson distn. here
  // Set mu = 0.15
  // then n = 1 is 400 mVns
  
  char buffer [50];
  
  string str_func = "gausn(x,%f,%f,%f)";
  
  sprintf(buffer,str_func.c_str(),npe*400,mean,width);
  
  f = new TF1("fGauss",buffer,0,300);
  //f->Draw(); 
  
  for( int bin = 1 ; bin <= NSamples ; bin++)
    hW->Fill(bin*nsPerSample,f->Eval(bin*nsPerSample));
  
  return hW;
}

TH1F * Generate_Charge_Dist(float mu    = 0.15, // Poisson
			    float mean  = 0.,   // Gaussian
			    float sigma = 0.4){ // Gaussian
  
  TH1F * hQ = new TH1F("hQ_Gen",
		       "hQ_Gen;Charge (mV ns);Counts",
		       101,-100,1900);
  
  TRandom * rand1 = new TRandom(1); 
  TRandom * rand2 = new TRandom(2);
  
  float Q   = 400.; // mVns
  int   npe = 0;
  
  int nEvents = 6647355;
  for ( int i = 0 ; i < nEvents ; i++ ){

    npe = rand1->Poisson(mu);

    Q = 400*npe;
    //cout << " Q = " << Q << endl;
    
    if   (Q < 1.0e-12 )
      Q += rand2->Gaus(0,25.);
    else 
      Q += rand2->Gaus(0,sigma*Q);
    
    hQ->Fill(Q);
  }
  
  return hQ;
}

void Generate(float mu = 0.15){
  
  TCanvas * canvas = new TCanvas(); 
  canvas->SetWindowSize(1000,800);
  
  TH1F *hW1, *hQ1;

  float npe = 1;
  
  hW1 = Generate_Waveform(npe);
  
  hQ1 = Generate_Charge_Dist(mu);
  
  SetStyle();
  
  hW1->Draw("HIST");
  hQ1->Draw();

  hQ1->SaveAs("hQ_Gen.root");
  
}

void SetStyle(){
  
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

void Set_THF_Params(float * minX,
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

}
