#include <cmath>
#include <cstdlib>
#include <fstream>

#include "TApplication.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TFitResult.h"
#include "TObject.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TLegend.h"
#include "TGraph.h"
#include "TString.h"

#include "TH1D.h"
#include "TFile.h"

using namespace std;
using namespace TMath;

//======= Gain Curve Fitters =============
inline double PowerFunc(double x, double k, double n)
{
  return pow((k*x),n)*10.00;
}

double fitPow(double *x, double *k)
{
  return PowerFunc(x[0], k[0], k[1]);
}

//====== Fitting functions ====

inline double gauss(double x,double sigma)
{
  // Normalized gaussian.
  return exp(-x*x/(2*sigma*sigma))/(sqrt(2.*Pi())*sigma);
}


inline double expBackground(double x,double *p)
{
  return sqrt(p[0]*p[0])+sqrt(p[1]*p[1])*exp(-x*p[2]);
}

inline double photon1Signal(double x,double *p)
{
  return abs(p[3]*gauss(x-p[4],p[5]));
}

inline double photon2Signal(double x,double *p)
{
  return abs(p[3]*p[6]*gauss(x-2*p[4],sqrt(2.)*p[5]));
}


double fitFunc(double *x,double *p)
{
  return expBackground(x[0],p)+photon1Signal(x[0],p)+photon2Signal(x[0],p);
}

void addGraph(double (*func)(double,double*),int color,double *p,double xmin,double xmax)
{
  // Extra graphs.
  int graphSteps=500;
 
  // Add error bands.
  TGraph *tg1=new TGraph(graphSteps);
  for (int i=0;i<graphSteps;i++)	{
    double x=xmin+(double)i*(xmax-xmin)/((double)graphSteps+1.);
    double y=func(x,p);
    tg1->SetPoint(i,x,y);
  }
  tg1->SetLineColor(color);
  tg1->SetLineWidth(2);
  tg1->Draw("SAME");
}

//=========================================================================================================================================

int main(int argc,char **argv){	
  
  // ******************
  // * Initialization *
  // ******************
	
  // Set up random number generator.
  //randomSeedTime();
  
  //Read in the HV data ====================================================================================
  string hvfile = "../HVScan.txt";
  ifstream file(hvfile.c_str());
  string hvdat;
  
  vector<int> PMT_number(125,0), HV(125,0);
  vector<vector<int>> HVstep;
  vector<int> step(5,0);
  for (int i=0; i<125; i++)
    HVstep.push_back(step);
  
  vector<vector<int>> pmtHV;
  for (int i=0; i<4; i++)
    pmtHV.push_back(step);
  
  for (int i=0; i<125; i++){
    for (int j=0; j<7; j++){
      file >> hvdat;
      int pmt_info =atof(hvdat.c_str());
      if (j==0){
	PMT_number[i]=pmt_info;
      }
      if (j!=0 && j!=6){
	HVstep[i][j-1]=pmt_info;
      }
      if (j==6){
	HV[i]=pmt_info;
      }
      //printf("j %d, val %d \n",j,pmt_info);
    }
}
  
  //========================================================================================================
  int channel[4]={0,0,0,0};
  char answer;
  char histname[200]= "";
  while(answer!='Y'&& answer!='y'){
    //Determing the PMT number and the applied Voltage=====================================================
    cout << "Input the PMT number in Channel 0 \n" ;
    cout << "Note: please neglect the NB and the zeros before the number \n" <<endl;
    cin  >> channel[0]; 
    cout <<endl;
    
    cout << "Input the PMT number in Channel 1 \n" ;
    cout << "Note: please neglect the NB and the zeros before the number \n" <<endl;
    cin  >> channel[1]; 
    cout <<endl;
    
    cout << "Input the PMT number in Channel 2 \n" ;
    cout << "Note: please neglect the NB and the zeros before the number \n" <<endl;
    cin  >> channel[2]; 
    cout <<endl;
    
    cout << "Input the PMT number in Channel 3 \n";
		cout << "Note: please neglect the NB and the zeros before the number \n" <<endl;
		cin  >> channel[3]; 
		cout <<endl;
		
		
		
		for (int i=0;i<125; i++){
		  for(int j=0; j<4; j++){
		    for(int h=0; h<5; h++){
		      if (channel[j]==PMT_number[i]){
			pmtHV[j][h] =HVstep[i][h];
			printf("HV %d Channel %d Test %d \n",pmtHV[j][h],j,h);
		      }
		    }
		  }
		}
		
		
		cout <<"Please verifiy the following: "<<endl;
		for (int i=0; i<4; i++){
		  if (channel[i]<10)
		    sprintf(histname,"NB000%d is in Channel %d \n",channel[i], i);
		  if (channel[i]>=10 && channel[i] <100)
		    sprintf(histname,"NB00%d is in Channel %d \n",channel[i],  i);
		  if (channel[i]>=100)
		    sprintf(histname,"NB0%d is in Channel %d  \n",channel[i],  i);
		  cout << histname ;
		}
		
		cout <<"Is this correct? (y/n)  "<<endl;
		cin>>answer;
		cout <<answer<<endl;
		
  }
  //======================================================================================================
  
  
  // ***************
  // * Set up ROOT *
  // ***************
  // // Create default ROOT application.
  // TApplication *ta=new TApplication("ta",&argc,argv);
  
  // Data histogram.
  //TH1D* sData=newTH1D("data","Single Photon Energy;Channel;Counts",2000,-1000,9000);
  int PMT[4] = {channel[0],channel[1],channel[2],channel[3]};
  double gainValues[4][5];  //creates an empty array for the gain value for each PMT and each step
  
  
  //	vector<double> centroid(20,0), centroid_error(20,0),area(20,0), area_error(20,0);
  //char filename[30];
  // Fitting the SPE Spectrum =======================================================================
  
  for (int r=0;r<20;r++){ 
    
    TCanvas *tc=new TCanvas("Canvas","ROOT Canvas",1);
    //tc->Connect("TCanvas","Closed()","TApplication",gApplication,"Terminate()");
    tc->SetGrid();
    
    // Create canvas, allowing for window close.
    
    
    // *************************
    // * Create output spectra *
    // *************************
    
    //		if (r<5)mod = PMT[0];if (r>=5&&r<10)mod = PMT[1];if (r>=10&&r<15) mod = PMT[2];if (r>=15) mod = PMT[3];	[taken out for testing,LK]
    int pmtChannel = r%4; // pmt channel [taken out for testing, LK]
    int hv = r%5;  // gain test number [taken out for testing, LK]
    int mod = PMT[pmtChannel]; // 0; // pmt number
    
    if (mod<10)		
      sprintf(histname, "HV_SPE/PMT_NB000%d_HV%d.root",mod, pmtHV[pmtChannel][hv]); 
    if (mod>=10 && mod <100)
      sprintf(histname, "HV_SPE/PMT_NB00%d_HV%d.root",mod, pmtHV[pmtChannel][hv]);
    if (mod>=100)
      sprintf(histname, "HV_SPE/PMT_NB0%d_HV%d.root",mod, pmtHV[pmtChannel][hv]); 
    
    //printf("Voltage: 
    TFile s(histname);
    s.ls();
    
    char root_name[30];
    sprintf(root_name, "SPE%d;1.root",pmtChannel); //changed from mod to pmt
    //sprintf(root_name, "SPE%d;1.root",3);	
    TH1D *speData = (TH1D*)s.Get(root_name);

    printf("Getting data from SPE spectrum...\n");
    speData->GetYaxis()->SetTitle("Counts ");
    speData->GetYaxis()->SetTitleOffset(1.5);
    speData->GetXaxis()->SetTitle("Charge (mv*ns)");	
    
		// ****************
		// * Analyze data *
		// ****************
	
		// Save spectra.
		//ns().SaveData("rawspec.root");
	
		// Draw spectrum.
		speData->Draw("same");
		
		// Fit the data.
		/*
		double fitXMin=20.;
		
		double fitXMax=400.;

		
		if (hv==3||hv==2){ //if (((r+1)%5)==3||((r+1)%5)==2){
			fitXMin = 70;
			fitXMax = 1500;
		}
		
		if (hv>3||hv==0){ //if (((r+1)%5)>3||((r+1)%5)==0){
			fitXMin = 50;
			fitXMax = 2000;
		}
		TF1 *tf11=new TF1("data1Fit",fitFunc,fitXMin,fitXMax,7);
		tf11->SetLineWidth(3);

		// Exponential background.
		tf11->SetParameter(0,4.77335);

		tf11->SetParameter(1,91.6516);		
		tf11->SetParameter(2, 0.0000656504);

		// Single photon gaussian.
		tf11->SetParameter(3,195784);
		tf11->SetParameter(4,158.745);
		if (hv==3||hv==2) //if (((r+1)%5)==3||((r+1)%5)==2)
			tf11->SetParameter(4,200.745);
		if (hv>3||hv==0) //if (((r+1)%5)>3||((r+1)%5)==0)
			tf11->SetParameter(4,300.745);
		tf11->SetParameter(5,161.693);

		// Double and triple photon gaussian.
		tf11->SetParameter(6, 0.223033);
	
	

		
		
		// Perform fit.
		TFitResultPtr tfrp1=speData->Fit("data1Fit","RSE");
	
		// Print results.
		tfrp1->Print();
		
		area[r] =tf11->GetParameter(3); 
		area_error[r]=sqrt((tf11->GetParError(3))*(tf11->GetParError(3))); 

		centroid[r] =tf11->GetParameter(4); 
		centroid_error[r]=sqrt((tf11->GetParError(4))*(tf11->GetParError(4))+centroid[r]*centroid[r]/area[r])+centroid[r]*.03; 
					
		
		
		//Area of the curve
		//cout<< "area under curve" << Area [r]<< endl;
		//cout<< "area error" << AreaError[r] << endl;
		printf("Centroid: %f \n",centroid[r]); 
	
		//addGraph(skewedBackground,kGreen+2,tf11->GetParameters(),fitXMin,fitXMax);
		addGraph(expBackground,kMagenta+2,tf11->GetParameters(),fitXMin,fitXMax);
		addGraph(photon1Signal,kGreen+2,tf11->GetParameters(),fitXMin,fitXMax);
		addGraph(photon2Signal,kCyan+2,tf11->GetParameters(),fitXMin,fitXMax);
		//addGraph(photon3Signal,kOrange+3,tf11->GetParameters(),fitXMin,fitXMax);
		*/

		// Update canvas.
		tc->Update();
		tc->Paint();
		tc->Draw("same");
		tc->Modified();
		
		
		
		//Find the value at the maximum
//		double XMin=20.;
		
//		double XMax=400.;
		//TODO change this to loop over the bins above 20mVns
		double XMin=70.;
		
		double XMax=2000.;

		
		if (hv==3||hv==2||hv==1){ //if (((r+1)%5)==3||((r+1)%5)==2){
			XMin = 70;
			XMax = 2000;
		}
		
		if (hv==0||hv==1){ //if (((r+1)%5)>3||((r+1)%5)==0){
			XMin = 40;
			XMax = 2000;
		}

		speData->GetXaxis()->SetRange((XMin*2+250),(XMax*2+250));
		int binmax = speData->GetMaximumBin();
		double xMax = speData->GetBinCenter(binmax);
		printf(" voltage is  %d , charge is %f \n\n\n\n",pmtHV[pmtChannel][hv],xMax); 
		float amplification = 10.0; //amplification via amplifier
		float splitter = 2.0; //correction for use of splitter box
		float impedence = 50.0;
		double pmtGain = xMax*10e-12*splitter/amplification/impedence/(1.602*10e-19)/1e7; //conversion from charge in mVns to gain
		gainValues[pmtChannel][hv] = pmtGain; //create a list of the gain values
		
		
		// Enter run loop.
		
		TString plotName  = "./Plots/PMT_%d_%d.png";
		plotName.Form("./Plots/PMT_%d_%d.png",mod,pmtHV[pmtChannel][hv]);
		
		cout << " mod     = " << mod << endl;
		cout << " pmtHV[" << pmtChannel << "][" << hv << "] = " << pmtHV[pmtChannel][hv] << endl;
		cout << " plotName = " << plotName << endl;
		//plotName += ".png";

		tc->SaveAs(plotName);
		//ta->Run("false");
  }
  
  
  TCanvas *tc=new TCanvas("Canvas","ROOT Canvas",1);
  
  // Making the HV fit ========================================================================
  TGraph *Gain[4];
  
  for (int i=0;i<4;i++){
    int pmt = i;
    
    double hvVals[5]={0,0,0,0,0};

		double gainVals[5]={0,0,0,0,0};

		for (int j=0; j<5; j++){
			hvVals[j] = pmtHV[i][j];
			gainVals[j] = gainValues[i][j];
			
		}
		
	
		//Plot gain vs voltage for each PMT
		Gain[i] = new TGraph(5,hvVals,gainVals);

  		TString gain_name;
		gain_name.Form("gain%d",pmt);
		cout<<gain_name;

		Gain[i]->SetMarkerStyle(2);
		Gain[i]->SetMarkerSize(1);
		Gain[i]->SetName(gain_name);
		
		//fit a curve of form y = 10*(kx)^n to the data
		double fitMin = hvVals[0];
		double fitMax = hvVals[4];
		TF1 *f14 = new TF1("f14",fitPow,fitMin,fitMax,2);
		f14->SetParameter(0,10);
		f14->SetParameter(1,10);
		f14->SetLineColor(pmt+1);
		TFitResultPtr tfrp14=Gain[i]->Fit("f14","RSE");

		//Draw the plot plus fit
		Gain[i]->Draw("AP");
		Gain[i]->GetYaxis()->SetTitle("Gain (10^7)");
		Gain[i]->GetXaxis()->SetTitle("Applied Voltage (V)");
		TString Title;
		Title.Form("Gain for PMT %d",PMT[i]);
		Gain[i]->SetTitle(Title);
		
		//Bias for 10^7 GAIN ==========================================================================================================
//		double PMTgain = pow(35.0,1/f14->GetParameter(1))/(f14->GetParameter(0));
//		double PMTgainError = abs(pow(35.0,1/f14->GetParameter(1))/(f14->GetParameter(0))
//		                      -pow(35.0,1/(f14->GetParameter(1)+f14->GetParError(1)))/(f14->GetParameter(0)+f14->GetParError(0)));
//		TODO This is a temporary calculation, which evaluates the operating voltage at 10^7 gain
//		not based on the fit to the curve or on a fit to the peak
//
//		//calculate the operating voltage for 10^7 gain
		double operatingHV = pow(1.0/10.,(1./f14->GetParameter(1)))/(f14->GetParameter(0)); //inverse of fit function with y=1 (ie y= 1e7 gain)
		double operatingHVError = abs(pow(1.0/10.,(1./f14->GetParameter(1)))/(f14->GetParameter(0)) - pow(1.0/10.,(1./(f14->GetParameter(1)+f14->GetParError(1))))/(f14->GetParameter(0)+f14->GetParError(0)));
		printf("\n\n\n\n\n Operating voltage for 10^7 Gain for PMT %d: %f  +/- %f \n\n\n\n\n", PMT[i],operatingHV, operatingHVError );
		//=============================================================================================================================
		
		//ta->Run("false");
		TString plotName  = "./Plots/Gain_PMT_%d.png";
		plotName.Form("./Plots/Gain_PMT_%d.png",PMT[i]);
		cout << " plotName = " << plotName << endl;
		tc->SaveAs(plotName);
		
	}
//	mg->Draw("AP");
//	mg->GetYaxis()->SetTitle("Gain (10^7)");
//	mg->GetXaxis()->SetTitle("Applied Voltage (V)");

  //ta->Run("false");
  
  return 0;
}





