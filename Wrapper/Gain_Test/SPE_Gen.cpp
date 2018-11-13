/****************************************************
* Program to read wavedump output data
*
* Author 
*  Tomi Akindele
*  https://github.com/akindele
*  04 10 2018
*
* Purpose
*  DAQ reads out to binary file.
*  This program de-codes the file
*  and reads the relevant data in 
*  to a root histogram/s.
*
* How to build
*  $ make SPE_Gen
*
* How to run
*  $ ./SPE_Gen  
*
* Dependencies
*  root.cern
*  Makefile
*
* Modified
*   gary.smith@ed.ac.uk
*   03 11 2018
*
* Modifications:
*   (Completed)
*   Added comments
*   Removed dependency on non-standard libraries
*   namely ns.h.
*   Commented out randomSeedTime() as this is not
*   used.
*   Added the header for TH1D
*   Added nChs variable to accommodate generalising 
*   the code to any number of channels/PMTs
*   Array lengths are initialised to nChs where appropriate
*   Renamed variables: 
*      variables read in from HVscan now end in  _file
*      Gain array was changed to HVs
*   In loops the iterators are now more descriptive 
*      i or w were replaced with iCh or iPMT or iRow for example
*   New paths for binary input files and root output files
*   to allow SPE_Gen to run independently of Data Aquistion process.
*   doComment bool added - to switch on/off commenting to aid debugging
*   Generalised to VME or desktop digitiser
*   
*   (To Do)
*    Charge calculation used numbers that are 
*    not described well.  Change to the use of 
*    variables with names that give 
*    explainations...  
*    Separate code into functions so that it can 
*    be generalised for use for any test...
*    
*/ 

// Presumably "non-standard.h"
//#include "ns.h"

//Standard library include files.
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include "TApplication.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TObject.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TLegend.h"
#include "TGraph.h"

using namespace std;

int main(int argc, char **argv)
{
    
  // 'D' - Desktop
  // 'V' - VME
  char   digitiser = 'D';
  
  // Number of channels of recorded data to read.
  static const int nChs = 1;

  cout << " DATA_DIR = " << getenv("DATA_DIR") << endl;
  TString inputFileName = getenv("DATA_DIR");
  
  // switch on/off debugging messages
  bool doComment = false;
    
  int PMTs[nChs];
  int HVs[nChs];
  
  for (int iCh = 0 ; iCh < nChs ; iCh++){
    PMTs[iCh] = 0;
    HVs[iCh]  = 0;
  }
  
  //======= Read in HV data  ======= 
  string   hvfile = "../HVScan.txt";
  ifstream file(hvfile.c_str());
  string   hvdat;
  
  vector<int> PMT_file(125,0), HV_Nom_file(125,0);
  vector<vector<int>> HV_steps_file;
  
  // HV steps
  vector<int> step(5,0);
  
   // Read in 5 values for all PMT 
   for (int i = 0; i < 125; i++)
     HV_steps_file.push_back(step);
   
   int pmt_info;
   
   for (int iRow = 0; iRow < 125; iRow++){
     for (int iCol = 0; iCol < 7  ; iCol++){
       
       file >> hvdat;
       pmt_info = atof(hvdat.c_str());
       
       if (iCol == 0){
	 PMT_file[iRow] = pmt_info;
       }
       else if (iCol != 0 && iCol != 6){
	 HV_steps_file[iRow][iCol-1] = pmt_info;
       }
       else if (iCol == 6){
	 HV_Nom_file[iRow] = pmt_info;
       }
       
       if(doComment)
	 printf("column %d, value %d \n",iCol,pmt_info);
     }
   }

   //==================================================================

   char    answer = 'N';
   int     iHVStep;
   char    histname[200]= "";
   TString aQuestion = "";
   
   while(answer!='y'){
     
     for ( int iCh = 0 ; iCh < nChs ; iCh++ ){
       cout << "Input the PMT number in Channel " << iCh << "  \n" ;
       cout << "Note: please neglect the NB and the zeros before the number \n" <<endl;
       cin  >> PMTs[iCh]; 
       cout << endl;
       
       if ( PMTs[iCh] < 0 || 
	    PMTs[iCh] > 200 ){
	 cerr << "Error: PMT number step  " << endl;
	 return -1;
       }
       
     }
     
     cout << "Please Specify which HV Step (1, 2, 3, 4, or 5)" << endl;
     cin  >> iHVStep;
     
     if ( iHVStep < 1 || 
	  iHVStep > 5 ){
       cerr << "Error: invalid HV step  " << endl;
       return -1;
     }
          
     iHVStep--;
     
     for (int iPMT = 0; iPMT < 125 ; iPMT++){
       for(int iCh = 0; iCh < nChs; iCh++){
	 
	 if (PMTs[iCh] == PMT_file[iPMT]){
	   
	   HVs[iCh] = HV_steps_file[iPMT][iHVStep];
	   
	   if(doComment)
	     printf("HVs %d \n", 
		    HV_steps_file[iPMT][iHVStep]);
	   
	 }
       }
     }
     
    cout << " Please verify the following: " << endl;
    
    for (int iCh = 0 ; iCh < nChs ; iCh++){
      
      aQuestion = "NB0";
      
      if (PMTs[iCh] < 100){
	aQuestion += "0";
	if (PMTs[iCh] < 10)
	  aQuestion += "0";
      }
      
      aQuestion.Form(aQuestion + 
		     "%d is in Channel %d Biased at %d Volts \n",
		     PMTs[iCh], iCh, HVs[iCh]);
      cout << aQuestion;
     
    }

    cout << " Is this correct? (y/n)  "<<endl;
    cin >> answer;
    
    if( answer == 'Y')
      answer = 'y';
    
    cout << answer << endl;
    
   }// end: while(answer!='Y'
   //==============================
   
   //==============================
   //  Digitiser specific settings
   
   int    nBins = 102;
   double xMin  = 0.;
   double xMax  = 204.;
  
   int    intsPerHeader  = 6;
   int    intsPerEvent = 1030;
   int    intsPerPulse   = 1024;
   
   double aoff        = 2700;
   
   if     ( digitiser == 'V' ){
     nBins = 102;
     xMax  = 204.;
     
     intsPerEvent = 122;
     intsPerPulse = 110;
     
     aoff         = 8700;
     
   }
   else if( digitiser == 'D' ){
     nBins = 1024;
     xMax  = 204.8;
     
     intsPerEvent = 1030;
     intsPerPulse = 1024;

     aoff         = 2700;
    
   
   }
   else{
     
     cerr << "Error: Invalid digitiser " << endl;
     return -1;
   }

   // Store waveform for processing
  TH1D* Wave = new TH1D("Wave","Waveform; Time (ns); ADC Counts",nBins,xMin,xMax);

  // Single Photoelectron Spectra with averaged accumulators
  TH1D **SPE = new TH1D*[nChs];	
  for (int iCh = 0 ; iCh < nChs ; iCh++){
    sprintf(histname, "SPE%d",iCh);
    SPE[iCh] = new TH1D(histname,"Single Photo-Electron; Charge (mV-ns); Counts",1500.0,-500.0,2000.0);
  }
  
  
  //TH1D * hBinMax = TH1D("hBinMax","hBinMax",);
  
  int totalwaves[nChs];
  for( int iCh = 0 ; iCh < nChs ; iCh++ )
    totalwaves[iCh] = 0;
  
  
  //================= Read in the headers and assigns values for things=============
  
  
  //================= Read in waveforms of length 102 or 1024==================

  iHVStep++;
  
  // counter: for outputing progress
  int counter = 0;

  
  TH2F * hTV = new TH2F("hTV","hTV",
			128, 0., 120.,
			128, 0., 1000);
  
  for (int iCh = 0 ; iCh < nChs ; iCh++){

    if     ( digitiser == 'D'){
      inputFileName.Form(inputFileName +
			 "/wave_%d.dat", 
			 iCh); 
    }
    else if( digitiser == 'V' ){
      inputFileName.Form(inputFileName +
			 "/wave_%d_VME.dat", 
			 iCh); 
    }
    
    cout << endl;
    cout << " inputFileName = " << inputFileName << endl;    
    
    ifstream fin(inputFileName);
    
    int header = 0;
    for (int i = 0 ; i < intsPerHeader; i++ ){
      header = 0.;
      fin.read((char*)&header,sizeof(int));
      cout << " header = " << header << endl;  
      
    }
    counter = 0;
    while ( fin.is_open() && 
	    fin.good()    && 
	    !fin.eof()
	    ){
      counter++;

      if (counter%10000==0)
	printf("Waveform Progress: %d \n", counter);
      
      // Records and ind. waveform into
      
      for (int i = 0; i < intsPerEvent; i++){


	// ----------- VME digitiser ----------
	
	double flip_signal = 0;
	if     ( digitiser == 'V' ){
	  unsigned short result=0.;  //changed from float
	  fin.read((char*)&result,2);  //sizeof(float) 
	  
	  if (i < intsPerPulse ){
	    flip_signal = (float(result)-aoff)*-1.0;
	    
	    
	 }
	  
	}
	// ---------------------------------------
	// ----------- Desktop digitiser ----------
	else if( digitiser == 'D' ){
	  float result=0.;
	  fin.read((char*)&result,sizeof(float));
	
	  if (i < intsPerPulse ){
	    flip_signal = (result-aoff)*-1.0;
	  }
	  
	}
	
	Wave->SetBinContent(i+1,flip_signal);
	
	hTV->Fill(i+1,flip_signal);
      }
    
      
  
      // Determine the location of the peak
      int    binmax  = Wave->GetMaximumBin(); 
      double maxtime = Wave->GetXaxis()->GetBinCenter(binmax);

       if(doComment)
	 printf("maxtime: %f\n",maxtime);

       static const int nGates = 8;

       int gates[nGates];
       
       //!! temporary
       int multiple = 100;
       
       float timeLow  = 60.0;
       float timeHigh = 124.8;

       if( digitiser == 'V' ){
	  multiple = 10;
	  timeHigh = 124.0;
       }
       
       for (int iGate = -3 ; iGate < (nGates - 3) ; iGate++ )
	 gates[iGate+3] = binmax + ( iGate * multiple );

       // Only count events in window
       if (maxtime > timeLow  && 
	   maxtime < timeHigh     ){

	 //Define the accumulators
	 double A0 = 0; double A1=0;
	 double A2 = 0; double A3=0;
	 double A4 = 0; double A5=0;
	 double A6 = 0;
	 	 
	 for (int i = 1; i <= nBins; i++){
	   
	   int time = i;
	   if (time >= gates[0] && time <= gates[1]){
	     A0 += Wave->GetBinContent(i);
	   }	
	   if (time>=gates[1] && time<=gates[2]){

	     A1+=Wave->GetBinContent(i);
	   }
	   if (time>=gates[2] && time<=gates[3]){

	     A2+=Wave->GetBinContent(i);
	   }
	   if (time>=gates[3] && time<=gates[4]){

	     A3+=Wave->GetBinContent(i);
	   }
	   if (time>=gates[4] && time<=gates[5]){

	     A4+=Wave->GetBinContent(i);
	   }
	   if (time>=gates[5] && time<=gates[6]){
	   
	     A5+=Wave->GetBinContent(i);
	   }
	   if (time>=gates[6] && time<=gates[7]){

	     A6+=Wave->GetBinContent(i); 
	   }

	 }

	 // Filling all the SPE
	 double ADC_Counts = A2+A3+A4-(A0+A1+A5+A6)*3.0/4.0;
	 
	 //cout << " ADC_Counts " << ADC_Counts << endl; 
	 
	 double WaveCharge =  ADC_Counts*.2/4096.0*1.0e3;
	 
	 if( digitiser == 'V' )
	   WaveCharge =  ADC_Counts*2.0/16384.0*2.0e3;
	 
	 if( doComment )
	   printf("WaveCharge %f \n",WaveCharge);
	 
	 SPE[iCh]->Fill(WaveCharge);
       }

    }

    totalwaves[iCh]=counter;

    //!!
    //SPE[iCh]->Scale(1./(counter));
    
    // close wavedump file
    fin.close();	
  
  } // end: for (int iCh = 0 ; iCh 
  
  //Print out total number of waves for the relative quantum efficiency
  for (int iCh = 0 ; iCh < nChs ; iCh++)
    printf("Total Triggers from Wave %d: %d \n", iCh, totalwaves[iCh]);
  
  //Create canvas allowing for window close
  TApplication *ta= new TApplication("ta",&argc,argv);
  TCanvas *tc1= new TCanvas("Canvas1","ROOT Canvas",1);
  tc1->Connect("TCanvas1","Closed()","TApplication",gApplication, "Terminate()");
  tc1->SetGrid();
  
  for (int iCh = 0 ; iCh < nChs ; iCh++)
    SPE[iCh]->Draw("HIST SAME");
  
  hTV->Draw("colz");
  
  TString outFileName = "";

  for (int iCh = 0 ; iCh < nChs ; iCh++){
    
    outFileName = "./HV_SPE/PMT_NB0";
    
    if (PMTs[iCh] < 100){
      outFileName += "0";
      
      if (PMTs[iCh] < 10)
	outFileName += "0";
    }
    
    outFileName.Form(outFileName + "%d_HV%d.root", PMTs[iCh], HVs[iCh]);
    
    SPE[iCh]->SaveAs(outFileName);
  }
  
  

  ta->Run();
    
  return 0;
}

