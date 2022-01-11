#include <TH1.h>
#include <TF1.h>
#include <TMath.h>
#include <string.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TRandom3.h>

#include "wmStyle.C"

TRandom3 * rand3 = new TRandom3(0);

void SetStyle();

void Set_THF_Params(float *, float *, float *, int *);

TH1F * Generate_Waveform(float npe   = 1.,
			 float width = 7.5,
			 float mean  = 100.);

TH1F * Generate_Charge_Dist(float npe_mu       = 0.29, // Poisson
			    float charge_sigma = 0.25, // Gaussian (value from data)
			    float charge_decay = 0.5, // Exp
			    float noise_sigma  = 14, // (value from data)
			    float gain         = 0.8);

TH1F * Gen_Dark_Charge_Dist(float darkRate = 3000,
			    float waveDuration = 300, //ns
			    int   nTriggers = 9022667);//6647355);



void Generate(float mu = 0.15){
  
  TCanvas * canvas = new TCanvas("canvas","canvas",1000,0,550,450); 
  //canvas->SetWindowSize(550,450);
  //canvas->SetWindowPosition(1000,500);
    
  TH1F *hW1, *hQ1;

  float npe = 1;
  hW1 = Generate_Waveform(npe);
  //hQ1 = Gen_Dark_Charge_Dist();
  //hQ1 = Generate_Charge_Dist(0.29);
  
  SetStyle();
  //gPad->SetLogy();
  
  hW1->Draw("HIST");
  //hQ1->Draw("HIST");

  //hQ1->SetMinimum(hQ1->GetMaximum()*0.001);
  //gPad->SetLogy();
  
  //hQ1->SaveAs("hQ_Gen.root"); 
  
}

TH1F * Generate_Charge_Dist(float npe_mu, 
			    float charge_sigma,
			    float charge_decay,
			    float noise_sigma,
			    float gain){ 
  
  TH1F * hQ = new TH1F("hQ_Gen",
		       "hQ_Gen;Charge (mV ns);Counts",
		       100,-1000,1000);

  // Mimic real data method
  hQ->GetXaxis()->SetRangeUser(-100,900);
  
  float Q1  = 400., Q = 400; // mVns
  int   npe = 0;

  int nEvents = 3033237;
  for ( int i = 0 ; i < nEvents ; i++ ){

    // generate npe
    npe = rand3->Poisson(npe_mu);

    // convert npe to charge
    Q = Q1*gain*npe;

    // pedestal and noise (dark, out-of-time pulses)
    if   ( npe == 0 ){
      Q += rand3->Gaus(0,noise_sigma); // baseline noise
      
      if( (i%10) == 0 ) 
	Q += rand3->Exp(charge_decay*Q1);

    }
    else {
      //Q = -100;
      Q += rand3->Gaus(0,charge_sigma*Q);
    }
    
    hQ->Fill(Q);
  }
  
  return hQ;
}

TH1F * Gen_Dark_Charge_Dist(float darkRate,
			    float waveDuration,
			    int   nTriggers){

  // rate in ns times length of waveform 
  float probPer_ns  = darkRate * 1.0E-9;
  int   nSamples    = 150.;
  float nsPerSample = 2.;
  
  float probPerSample = probPer_ns*nsPerSample;
  float oneIn = 1./probPerSample;

  cout << " oneIn = " << oneIn << endl;
  
  TH1F * hQ_D, * hW_D, * hW_D_Acc;

  hQ_D = new TH1F("hQ_Gen",
		  "hQ_Gen;Charge (mV ns);Counts",
		  100,-1000,1000);

  srand(time(NULL));

  bool firstTime = true;

  float pulse_width = 7.5; // sigma in ns 
  float npe = 1.; 

  // start generating pulses so tail just fits in waveform
  int firstSample = -3.*pulse_width/nsPerSample; 
  int lastSample  = 3.*pulse_width/nsPerSample+nSamples;  
  
  float baseline, signal, charge;
  for (int iTrig = 0 ; iTrig < nTriggers ; iTrig++){
    charge = 0;

    for ( int iSample = firstSample ; iSample < lastSample ; iSample++) {
      
      if ( rand() % (int)roundf(oneIn) == 0 ){

	//cout << " iTrig,iSamp = " << iTrig << "," << iSample << endl;
	
	hW_D = Generate_Waveform(npe,pulse_width,iSample*nsPerSample);
	
	baseline = hW_D->Integral(hW_D->GetXaxis()->FindBin(1),
				  hW_D->GetXaxis()->FindBin(51));
	
	signal   = hW_D->Integral(hW_D->GetXaxis()->FindBin(52),
				  hW_D->GetXaxis()->FindBin(102));

	charge += (signal-baseline);
	
	//cout << " charge = " << charge << endl;
	
	//hQ_D->Fill(charge);
	
	if(firstTime){
	  hW_D_Acc = (TH1F*)hW_D->Clone();
	  firstTime = false;
	}
	else{
	  hW_D_Acc->Add(hW_D);
	}
	
	//return hW_D;
	
	hW_D->Delete();
	
      }
      
    }// end of: for ( int iSample = 0 ; iSample < ...
    
    charge += rand3->Gaus(0,15); // baseline noise
    hQ_D->Fill(charge);

  }
  
  return hQ_D;
  //return hW_D_Acc;
}

TH1F * Generate_Waveform(float npe,float width,
			 float mean){
  int   NSamples = 0;
  float minX = 0., maxX = 298., nsPerSample = 2.;
  
  Set_THF_Params(&minX,&maxX,&nsPerSample,&NSamples);
  
  TH1F * hW = new TH1F("hW",
		       "Waveform;Time (ns); Voltage (mV)",
		       NSamples, minX, maxX);
  TF1 * f = nullptr;
  
  char buffer [50];
  
  string str_func = "gausn(x,%f,%f,%f)";

  float charge = 400;
  charge = rand3->Gaus(charge,0.25*charge);
  charge *= npe * nsPerSample;
  //cout << " charge = " << charge << endl;
  
  sprintf(buffer,str_func.c_str(),charge,mean,width);
  //printf("\n");
  //  printf("%s",buffer);
  //printf("\n");
    
  f = new TF1("fGauss",buffer,0,300);
  f->Draw(); 
  
  for( int bin = 1 ; bin <= NSamples ; bin++)
    hW->Fill(bin*nsPerSample,f->Eval(bin*nsPerSample));
  
  return hW;
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
