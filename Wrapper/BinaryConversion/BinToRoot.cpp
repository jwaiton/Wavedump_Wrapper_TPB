/***************************************************
 * A program to process wavedump output files
 *
 * Author 
 *  gary.smith@ed.ac.uk
 *  28 11 18
 *
 * Adapted from
 * SPE_Gen.cpp 
 *  Tomi Akindele
 *  https://github.com/akindele
 *  04 10 2018
 *
 * Purpose
 *  This program reads a wavedump binary 
 *  file and writes a TTree
 *  and histograms to a root file. 
 *
 * How to build
 *  $ make BinToRoot
 *
 * How to run
 *  Option 1: 
 *  $ ./BinToRoot run pmt loc test  
 * 
 *
 * Dependencies
 *  root.cern
 *
 * Modified 
 *  gary.smith@ed.ac.uk
 *  15 02 19
 *
 *  Pulse/pulse -> Waveform/waveform - previous name was incorrect
 *  Note that this must be propagated to output file anaysis code.
 *
 *  third digitiser option 'd' for desktop version @ 1 GHz 
 *
 *  added negPulsePol as optional argument 
 *  added samplingSetting  as optional argument 
 *
 *  removed unused variables qFixed and qPeak
 *
 *  New function ProcessBinaryFile() acts as 
 *  intermediate step before ProcessBinaryFile()
 *  accommodating a cleaner main() function
 */ 

#include <cstdlib>
#include <fstream>
#include <iostream>

#include "TFile.h"
#include "TTree.h"

#include "TCanvas.h"
#include "TH2.h"
#include "TLine.h"

using namespace std;

int  GetNSamples(char digitiser,
		 char test){
  
  switch(digitiser){     
  case ('V'):
    if (test=='A')
      return 5100;
    else
      return 110;
  case ('D'):
    return 1024;
  default:
    cerr << "Error: Unknown digitiser " << endl;
    return 0;
  }
  
}

int  GetNVDCBins(char digitiser){

  if     ( digitiser == 'V' )
    return 16384;
  else if( digitiser == 'D' )
    return 4096;
  else{
    cerr << "Error: Unknown digitiser " << endl;
    return 0;
  }
  
}

float GetVoltageRange(char digitiser){
  
  if     ( digitiser == 'V' )
    return 2.0;
  else if( digitiser == 'D' )
    return 1.0;
  else{
    cerr << "Error: Unknown digitiser " << endl;
    return 0;
  }


}

int GetSampleRateInMHz(char digitiser,
		       char samplingSetting){

  if     ( digitiser == 'V' )
    return 500;
  else if( digitiser == 'D' ){
    if     (samplingSetting == 'S')
      return 5000;
    else if(samplingSetting == 'L') 
      return 1000;
    else{
      cerr << "Error: Unknown sampling setting " << endl;
      return 0;
    }
  }
  else{
    cerr << "Error: Unknown digitiser " << endl;
    return 0;
  }

}

void printDAQConfig(char digitiser,
		    char samplingSetting,
		    bool negPulsePol){
  

  TString strDigitiser = "VME";
  
  if (digitiser=='d')
    strDigitiser = "Desktop";

  cout << endl;
  cout << "   " 
       << strDigitiser
       << " Digitiser "    << endl;
  cout << "   " 
       << GetNVDCBins(digitiser)        
       << " VDC Bins "    << endl;
  cout << "   " 
       << GetVoltageRange(digitiser)    
       << " Volts Range "  << endl;
  cout << "   " 
       << GetSampleRateInMHz(digitiser, 
			     samplingSetting) 
       << " MHz sample rate " << endl;
  
  if ( negPulsePol )
    cout << "   Negative Pulse Polarity" 
	 << endl;
  else
    cout << "   Positive Pulse Polarity" 
	 << endl;
  
}

float GetnsPerSample(char digitiser,
		     char samplingSetting){
  
  return (1.0e3 / (float)GetSampleRateInMHz(digitiser,
					    samplingSetting));

}

float GetWaveformLength(char digitiser,
			char test,
			char samplingSetting){
  
  return (float)GetNSamples(digitiser,
			    test)*GetnsPerSample(digitiser,
						 samplingSetting);
  
}

float GetDelay(int run = 0){
  
  if      ( run == 0 ) // 12th October
    return 60.; 
  else if ( run < 10 ) // 16th October
    return 50.;
  else if ( run < 20 ) // Underground
    return 90.;
  else                 // Surface
    return 60.;
}

float GetmVPerBin(char digitiser){
  return ( 1.0e3 * GetVoltageRange(digitiser) / 
	   GetNVDCBins(digitiser) );
}

float GetCharge(int intVDC, char digitiser,
		char samplingSetting, bool negPulsePol){
  
  float nsPerSample = GetnsPerSample(digitiser,
				     samplingSetting);
  
  float mVolts = (float)intVDC * GetmVPerBin(digitiser);

  float charge = 0.;
  
  if(negPulsePol)
    charge = -1. * nsPerSample * mVolts;
  else
    charge = nsPerSample * mVolts;
  
  return charge; 
  
}

bool isCorrectDigitiser(int header,
			char digitiser,
			int test){
  
  switch( test ){
  case ('A'): 
    if( header == 10224 && digitiser == 'V')
      return true;
    else{
      cerr << " Error: digitiser choice does not match header info " << endl;
      return false;
    }
  default:
    if( ( header == 244  &&  digitiser == 'V' ) || 
	( header == 4120 &&  digitiser == 'D' ) )
      return true;
    else{
      cerr << " Error: digitiser choice does not match header info " << endl;
      return false;
    }
  }
}

// Must leave room for pedestal window
bool isPeakInRange(float minTime, char digitiser,
		   char test, char samplingSetting){
  
  float waveformLength = GetWaveformLength(digitiser,test,samplingSetting);
  
  if( minTime >= 50.   && 
      minTime <  ( waveformLength - 30.0 ) )
    return true;
  else
    return false;
}

float gateWidth(){
  return 50;
}

// integration windows fixed wrt trigger
// baseline window before signal window only
int Accumulate_Fixed(short VDC, float time){

  // Integrate baseline using 
  // 50 ns window before signal
  // (max size given run 1 delay)
  // And signal in 50 ns window
  // NB using factors of two
  // given VME sampling rate.
  // Using greater than 40 to 
  // allow for trigger jitter
  if      ( time >= -gateWidth()  && 
	    time <    0 )
    return((int)-VDC);
  else if ( time >= 0 &&
	    time <  gateWidth() ){
    return((int)VDC);
  }
  else
    return 0.;
}

// Integration windows wrt waveform peak
// -10 ns before to 30 ns after
// timeRel is time relative to minT (in ns)
// baseline window before
int Accumulate_Peak(short VDC, float timeRel){
  
  if      ( timeRel >= -50 && 
	    timeRel <  -10 )
    return((int)-VDC);
  else if ( timeRel >= -10 &&
	    timeRel <   30 ){
    return((int)VDC);
  }
  else
    return 0.;
}


TString GetRunFolderName(int run){

  if     ( run < 10  )
    return "RUN00000%d/";
  else if( run < 100 )
    return "RUN0000%d/";
  else if( run < 1000 )
    return "RUN000%d/";
  else if( run < 10000 )
    return "RUN00%d/";
  else if( run < 100000 )
    return "RUN0%d/";
  else 
    return "RUN%d/";
}

TString GetPMTFolderName(int pmt){

  if     ( pmt < 10  )
    return "PMT000%d/";
  else if( pmt < 100 )
    return "PMT00%d/";
  else if( pmt < 1000 )
    return "PMT0%d/";
  else
    return "PMT%d/";
}

TString GetTestFolderName(char test){
  
  switch(test){
  case('S'):
    return "SPEtest/";
  case('N'):
    return "Nominal/";
  case('D'):
    return "DarkRateTest/";
  case('A'):
    return "APTest/";
  case('G'):
    return "GainTest/";
  default:
    cerr << " Error: invalid test type " << endl;
    return "";
  }
}


TString GetBinaryFilePath(TString filePath = "../../Data/",
			  int  run  = 1, 
			  int  pmt  = 1, 
			  int  loc  = 0,
			  char test = 'S',
			  int  hvStep = 0){
  
  TString rtnFilePath = "";
  
  // append file path
  filePath += GetRunFolderName(run);
  filePath += GetPMTFolderName(pmt);
  filePath += GetTestFolderName(test);
  
  // append with filename
  switch(test){
  case('G'):
    filePath += "wave_%d_hv%d.dat";
    rtnFilePath.Form(filePath,run,pmt,loc,hvStep);
    break;
  default:
    filePath += "wave_%d.dat";
    rtnFilePath.Form(filePath,run,pmt,loc);
  }

  return rtnFilePath;
}

TString GetRawRootFilePath(TString filePath = "./",
			   int  run  = 0, 
			   int  pmt  = 1, 
			   int  loc  = 0,
			   char test = 'S',
			   int  hvStep = 0){
  
  // default option
  if( run == 0 )
    return "outputFile.root";
  
  // Use same folder structure as
  // binary files ( folder creation
  // not implemented )
  // or write to single folder
  bool singleOutputFolder = true;

  TString rtnFilePath = "";
  
  if ( singleOutputFolder ){
    // append with filename
    // containing info
    switch(test){
    case('G'):
      filePath += "Run_%d_PMT_%d_Loc_%d_HV_%d";
      rtnFilePath.Form(filePath,run,pmt,loc,hvStep);
      break;
    default:
      filePath += "Run_%d_PMT_%d_Loc_%d_Test_%c";
      rtnFilePath.Form(filePath,run,pmt,loc,test);
    }
  }
  else {
    
    filePath += GetRunFolderName(run);
    filePath += GetPMTFolderName(pmt);
    filePath += GetTestFolderName(test);
    
    // append with filename
    // containing info
    switch(test){
    case('G'):
      filePath += "Run_%d_PMT_%d_Loc_%d_HV_%d";
      rtnFilePath.Form(filePath,run,pmt,run,pmt,loc,hvStep);
      break;
    default:
      filePath += "Run_%d_PMT_%d_Loc_%d_Test_%c";
      rtnFilePath.Form(filePath,run,pmt,run,pmt,loc,test);
    }
  }

  return rtnFilePath;
  
}

// Read in binary file and 
// write to root file
int ProcessBinaryFile(TString inFilePath,
		      TString outFilePath,
		      int run, int loc,
		      int pmt, int hvStep,
		      char test,
		      int  verbosity,
		      char digitiser,
		      bool negPulsePol = true,
		      char samplingSetting = 'S'
		      ){

  inFilePath = GetBinaryFilePath(inFilePath,
				 run, pmt, loc, test, hvStep);
  
  cout << endl;
  cout << " Processing  " << inFilePath << endl;

  //----------------------
  // Variables for testing
  bool  testMode  = false;
  bool  keepGoing = true;
  int   maxEvents = 100;
  
  if ( test == 'A' )
    maxEvents = 2;
  
  if     ( verbosity == 1 )
    maxEvents = 10;
  else if( verbosity == 2 )
    maxEvents = 1;
  //----------------------


  // Read from here
  ifstream fileStream(inFilePath);

  if(!fileStream.good()){
    cerr << endl;
    cerr << " Error: check filename " << endl;
    return -1;
  }
  
  if( verbosity > 0 ) 
    printDAQConfig(digitiser,
		   samplingSetting,
		   negPulsePol);
  
  outFilePath = GetRawRootFilePath(outFilePath,
				   run, pmt, loc, test, hvStep);

  
  TString outFileName = outFilePath;
  outFilePath += ".root";
  
  cout << endl;
  cout << " Writing     " << outFilePath << endl;
  
  // Write to here
  TFile * outFile    = new TFile(outFilePath,
				 "RECREATE",
				 outFileName);

  TString eventTreeNameTemp = "", eventTreeName = "";;
  TString canvasNameTemp    = "", canvasName    = "";
  
  TString hQ_FixedNameTemp = "", hQ_FixedName = "";
  TString hQ_PeakNameTemp  = "", hQ_PeakName  = "";
  
  if(hvStep == 0){
    eventTreeNameTemp = "Events_Run_%d_PMT_%d_Loc_%d_Test_%c";
    eventTreeName.Form(eventTreeNameTemp,run,pmt,loc,test);
    canvasNameTemp = "Canvas_Run_%d_PMT_%d_Loc_%d_Test_%c";
    canvasName.Form(canvasNameTemp,run,pmt,loc,test);
    
    hQ_FixedNameTemp = "hQ_Fixed_Run_%d_PMT_%d_Loc_%d_Test_%c";
    hQ_FixedName.Form(hQ_FixedNameTemp,run,pmt,loc,test);
    
    hQ_PeakNameTemp = "hQ_Peak_Run_%d_PMT_%d_Loc_%d_Test_%c";
    hQ_PeakName.Form(hQ_PeakNameTemp,run,pmt,loc,test);
    
  }
  else{
    eventTreeNameTemp = "Events_Run_%d_PMT_%d_Loc_%d_HV_%d";
    eventTreeName.Form(eventTreeNameTemp,run,pmt,loc,hvStep);
    canvasNameTemp = "Canvas_Run_%d_PMT_%d_Loc_%d_HV_%d";
    canvasName.Form(canvasNameTemp,run,pmt,loc,hvStep);

    hQ_FixedNameTemp = "hQ_Fixed_Run_%d_PMT_%d_Loc_%d_HV_%d";
    hQ_FixedName.Form(hQ_FixedNameTemp,run,pmt,loc,hvStep);
    
    hQ_PeakNameTemp = "hQ_Peak_Run_%d_PMT_%d_Loc_%d_HV_%d";
    hQ_PeakName.Form(hQ_PeakNameTemp,run,pmt,loc,hvStep);

  }
  //---------------------
  // Event Level Data
  TTree * eventTree  = new TTree(eventTreeName,
				 eventTreeName);
  
  TCanvas * canvas = new TCanvas(canvasName,
				 canvasName);
  
  Int_t nBinsX  = 512;
  Float_t rangeX[2] = {-500.,2000.};
  
  TH1F * hQ_Fixed = new TH1F(hQ_FixedName,
			     "Fixed gate;Charge (mV nS);Counts",
			     nBinsX,rangeX[0],rangeX[1]);
  
  TH1F * hQ_Peak = new TH1F(hQ_PeakName,
			    "Gate around peak;Charge (mV nS);Counts",
			    nBinsX,rangeX[0],rangeX[1]);
  
  TH2F * hWaveforms = new TH2F("hWaveforms",
			       "Subset of Raw Waveforms;Sample;VDC",
			       GetNSamples(digitiser,test),0,
			       (GetNSamples(digitiser,test)-1),
			       GetNVDCBins(digitiser),0.,
			       (GetNVDCBins(digitiser)-1));
  
  TString labels = "Subset of calibrated waveforms;Time (ns);Voltage (mV)";
  
  rangeX[0] = 0;
  
  rangeX[1] = GetNSamples(digitiser,test)-1;
  rangeX[1] = rangeX[1] * GetnsPerSample(digitiser,
					 samplingSetting);
  
  TH2F * hTV = new TH2F("hTV",labels,
			GetNSamples(digitiser,test),
			rangeX[0],rangeX[1],
			GetNVDCBins(digitiser),
			0.,GetVoltageRange(digitiser)*1.0e3);
  int   event  = -1;
  
  // Note that the sign is preserved for VDC, therefore 
  // minima (maxima) will be where the signal peaked 
  // for negative (positive) signal pulses
  short minVDC = 32767, maxVDC = -32768;  
  // sample numbers (indices) corresponding to minVDC and maxVDC
  short minT   = 32767, maxT   = -32768;  
  
  // accumulators for integrating the sample values
  int   intVDCfixed = 0, intVDCpeak = 0;

    // read in samples per event
  // vector may be better
  short waveform[GetNSamples(digitiser,test)];
  
  eventTree->Branch("event",&event,"event/I");
  eventTree->Branch("minVDC",&minVDC,"minVDC/S");
  eventTree->Branch("maxVDC",&maxVDC,"maxVDC/S");
  eventTree->Branch("minT",&minT,"minT/S");
  eventTree->Branch("maxT",&maxT,"maxT/S");
  
  TString arrayString = "";
  arrayString.Form("waveform[%d]/S",GetNSamples(digitiser,test));
  
  eventTree->Branch("waveform",waveform,arrayString);
  
  //---------------------
  // Sample Level Data

  short VDC = 0, sample = 0;
  int   fileHeader = 0;
  float floatVDC   = 0.;
  
  // waveform time in ns
  float waveTime = 0.;
  
  // waveform time with delay subtracted
  float time = 0.;
  
  // read in data from streamer object
  // until the end of the file
  while ( fileStream.is_open() && 
	  fileStream.good()    && 
	  !fileStream.eof()    &&
	  keepGoing 
	  ){
    
    //-------------------
    // file-level data
    event++;
    
    if( (event > 0) && 
	((event % 1000 == 0 && event < 5000) ||
	 (event % 1000000 == 0))              ){
      cout << endl;
      cout << " event count " << event << endl;;
    }

    //-------------------
    // event-level data
    intVDCfixed = 0, intVDCpeak = 0;
    
    // VDC range
    minVDC =  32767;
    maxVDC = -32768;  

    // time walk?
    minT   =  32767;
    // random?
    maxT   = -32768;

    //-------------------
    // sample-level data
    sample = 0;
    VDC    = 0;    
    
    if(event < 10 && 
       verbosity > 0){
      cout << endl;
      cout << " event      = " << event << endl;
    }
    
    // read in header info which comes 
    // as six four bit sized chunks
    for (int iHeader = 0 ; iHeader < 6 ; iHeader++ ){
      
      fileHeader = 0;
      
      // read in 4 bit segment
      fileStream.read( (char*)&fileHeader,
		       sizeof(int)); // read 4 bits
      
      // check first header value matches expectations
      // NB other values may be acceptable so modify
      // isCorrectDigitiser() as appropriate
      if ( iHeader == 0 && 
	   event   == 0 &&
	   !isCorrectDigitiser(fileHeader,
			       digitiser,test)  
	   ) {
	
	return -1;
      }
      
      if(event < 10 && 
	 verbosity > 0){
	cout << " fileHeader = " << fileHeader << endl;
      }
      
    } // end: for (int i = 0 ; i < intsPerHeader
    
    // read in waveform which comes 
    // in 2 (VME) or 4 (Desktop) bit chunks
    for (short iSample = 0; iSample < GetNSamples(digitiser,test); iSample++){
      
      VDC = 0;
      
      // read 2 bits
      if     ( digitiser == 'V' ){
	fileStream.read((char*)&VDC,2); 
      }
      // read 4 bits
      else if( digitiser == 'D' ){
	fileStream.read((char*)&floatVDC,sizeof(float));
	VDC = (short)floatVDC;
      }
      else {
	
	cerr << " Error:  incorrect digitiser " << endl;
	return -1;
	
      }
      
      // for writing
      sample = iSample;
      waveform[sample] = VDC;
      
      if     ( VDC < minVDC ){
	minVDC = VDC;
	minT   = sample;
      }
      else if( VDC > maxVDC ) {
	maxVDC = VDC;
	maxT   = sample;
      }

      waveTime = sample * GetnsPerSample(digitiser,
					 samplingSetting);
      
      // time with delay subtracted 
      // too allow common fixed time windows per run
      time = waveTime - GetDelay(run);
      
      // fixed window accumulations
      // add / subtract / skip sample VDC value
      intVDCfixed += Accumulate_Fixed(VDC,time);

      if(verbosity > 1)
	cout << " VDC(" << iSample << ") = " << VDC << endl;
      
    } // end: for (short iSample = 0; iSa
    
    // time of the signal peak
    float minTime = minT*GetnsPerSample(digitiser,
					samplingSetting);
    
    float timeRelPeak = 0;
    
    // Loop over waveform again
    // necessary for when event-level variables are used
    // such as waveform minimum
    for (short iSample = 0; iSample < GetNSamples(digitiser,test); iSample++){
      
      waveTime = iSample*GetnsPerSample(digitiser,
					samplingSetting);
      
      time = waveTime - GetDelay(run);

      timeRelPeak = waveTime - minTime;
	
      // accumulations wrt waveform peak (minimum)      
      if( isPeakInRange(minTime,digitiser,test,samplingSetting) )
	intVDCpeak += Accumulate_Peak(waveform[iSample],
				      timeRelPeak);
      
      // sample vs VDC and time vs voltage plots
      // for checking signals (delay etc) 
      if( event < 100000   &&
	  !( event % 1000 )){
	
	hWaveforms->Fill(iSample,waveform[iSample]);
	
	hTV->Fill(time + GetDelay(run),
		  waveform[iSample]*GetmVPerBin(digitiser));
	
      }
      
    } // end: for (short iSample = 0; iS...
    
    hQ_Fixed->Fill(GetCharge(intVDCfixed,digitiser,
			     samplingSetting,negPulsePol));
    
    if( isPeakInRange(minTime,digitiser,test,samplingSetting) )
      hQ_Peak->Fill(GetCharge(intVDCpeak,digitiser,
			      samplingSetting,negPulsePol));
    
    
    //--------------------------------
    // Write event by event data here
    
    eventTree->Fill();
    
    if(verbosity > 0){
      cout << endl;
      cout << " Event " << event << endl;
      cout << " minVDC(" << minT << ") = " << minVDC << endl;
      cout << " maxVDC(" << maxT << ") = " << maxVDC << endl;
      cout << endl;
    }
    
    if( testMode &&
	(event+1) == maxEvents )
      keepGoing = false;
    
  } // end: while loop

  // close wavedump file
  fileStream.close();	

  canvas->Divide(2,2);

  canvas->cd(1);
  
  hQ_Fixed->SetAxisRange(-500., 2500.,"X");

  gPad->SetLogy(1);
  hQ_Fixed->Draw();
  
  canvas->cd(2);
  gPad->SetLogy(1);
  
  hQ_Peak->SetAxisRange(-500., 2500.,"X");
  hQ_Peak->Draw();
  
  canvas->cd(3);
  
  float minY = GetVoltageRange(digitiser)*(16 - 2)/32*1.0e3;
  float maxY = GetVoltageRange(digitiser)*(16 + 1)/32*1.0e3 ;

  hTV->SetAxisRange(minY,maxY,"Y");

  float minX = 0.;
  float maxX = 1000.; // 220.
  
  if(test=='A'){
    hTV->SetAxisRange(minX,maxX,"X");
  }
  
  hTV->Draw("colz");
  
  float lineYMin = minY * (16 - 1)/(16 - 2); 
  float lineYMax = maxY * (16 + 0.25)/(16 + 1); 
  
  float lineXMin = GetDelay(run) - gateWidth();
  float lineXMax = GetDelay(run) + gateWidth();
  
  TLine *lPedMin = new TLine(lineXMin,lineYMin,
			     lineXMin,lineYMax); 
  lPedMin->SetLineColor(kRed);
  
  TLine *lSigMin = new TLine(GetDelay(run),lineYMin,
			     GetDelay(run),lineYMax); 
  lSigMin->SetLineColor(kBlue);
  
  TLine *lSigMax = new TLine(lineXMax,lineYMin,
			     lineXMax,lineYMax); 
  lSigMax->SetLineColor(kBlue);
  
  lPedMin->Draw("same");
  lSigMin->Draw("same");
  lSigMax->Draw("same");
  
  canvas->cd(4);
  
  minY = GetNVDCBins(digitiser)*10/16 ;
  maxY = GetNVDCBins(digitiser)*6/16 ;
  
  hWaveforms->SetAxisRange(minY,maxY,"Y");
    
  
  hWaveforms->Draw("colz");
  
  canvasName = "./Plots/";
  canvasName += canvas->GetName();
  canvasName += ".png";

  canvas->SaveAs(canvasName);
  
  //--------------------------------
  // Write file info here

  hWaveforms->Delete();
  hTV->Delete();

  eventTree->Write();
  eventTree->Delete();

  outFile->Write();
  outFile->Close();
  
  return (event+1);
}


string GetDefaultFilePath(){
  return  "../../Data/wave_0.dat";
}

bool GetNegPulsePol(char digitiser){
  
  bool negPulsePol = true;
  
  if( digitiser != 'V' ){
    cout << " Enter negative pulse polarity: ";
    cout << endl;
    cout << " true  - negative  "; 
    cout << endl;
    cout << " false - positive  ";
    cin >> negPulsePol;
    cout << endl;
  }
  
  return negPulsePol;
}

char GetSamplingSetting(char digitiser){

  if  (digitiser == 'V')
    return 'S'; 
  
  char samplingSetting = 'S';

  cout << " Enter sampling frequency: " << endl;
  cout << " 'S' (Standard)  5 GHZ "    << endl;
  cout << " 'L' (Low)       1 GHZ "    << endl;
  cin  >> samplingSetting;
  cout << endl;
  
  return samplingSetting;
}  


int GetRunUser(){
  int run;
  cout << " Enter run number: ";
  cin >> run;
  cout << endl;
  return run;
}

int GetPMTUser(){
  int pmt;
  cout << " Enter pmt number: ";
  cin >> pmt;
  cout << endl;
  return pmt;
}

int GetLocUser(){
  int loc;
  cout << " Enter rig location: ";
  cin >> loc;
  cout << endl;
  return loc;
}

char GetTestUser(){
  char test;
  cout << " Enter test type : ";
  cin >> test;
  cout << endl;
  return test;
}

char * GetInDirUser(){
  char * inDir = new char[128];
  cout << " Enter input path: ";
  cin >> inDir;
  cout << endl;
  return inDir;
}

char * GetOutDirUser(){
  char * outDir = new char[128];
  cout << " Enter output path: ";
  cin >> outDir;
  cout << endl;
  return outDir;
}

char GetDigitiserUser(){
  char digitiser;
  cout << " Enter digitiser type ('V', or 'D'): ";
  cin >> digitiser;
  cout << endl;
  return digitiser;
}

int GetHVStep(char test ){
  if( test == 'G')
    return 1;
  else
    return 0;
}

int GetNSteps(char test ){
  if( test == 'G')
    return 5;
  else
    return 0;
}

void ExecuteProcessing(int run = 0, int pmt = 0,
		       int loc = 0 ,char test = 'A',
		       TString inDir = "./", 
		       TString outDir = "./",
		       char digitiser = 'V',
		       char verbosity = 0
		       ){
  
  bool negPulsePol = GetNegPulsePol(digitiser);
  char samplingSetting = GetSamplingSetting(digitiser);

  int  nEvents = -1;

  for (int hvStep = GetHVStep(test) ; 
       hvStep <= GetNSteps(test)    ; 
       hvStep++ ){
    
    nEvents = ProcessBinaryFile(inDir,
				outDir, 
				run, loc, 
				pmt, hvStep,
				test,
				verbosity, 
				digitiser,
				negPulsePol,
				samplingSetting);
    
    cout << endl;
    cout << " Output file contains " << nEvents << " events " << endl; 
  }  
  
  cout << endl;
  cout << " Completed Processing    " << endl;
  cout << " ----------------------- " << endl;
}

int GetPMT(int run, int iPMT){
  
  if(run == 1){
    int  pmtList[100] = {83 , 88,108,107, // Tent A
			 73 , 76, 84, 87,
			 66 , 78, 82,103,
			 104,106,112,141,
			 61 , 65, 75,105,
			 74 ,111,140,142,
			 143,145,146,147,
			 63 , 67,158,160,
			 139,161,164,165,
			 90 ,159,166,171,
			 81 ,167,169,170,
			 50 , 53,162,163,
			 55 , 56, 92, 94,
			 57 , 51, 54, 59,
			 96 , 97, 98, 99,
			 153,148,154,157,
			 1  ,  3,  6,  7,
			 34 , 37, 39, 42,
			 26 , 27, 28, 29,
			 130,131,132,133, // Tent A
			 102,149,150,152, // Tent B
			 9  , 10, 12, 14,
			 43 , 47, 48, 49,
			 30 , 31, 32, 33,
			 134,135,136,138};// Tent B	   
    
    return pmtList[iPMT];
  }
  else
    return 1;

}

int GetLoc(int run, int iPMT){
  
  if(run == 1)
    if( iPMT < 80 )
      return (iPMT%4);
    else
      return (iPMT%4 + 4);
  else
    return 0;

}

int GetNPMTs(int run){

  if(run == 1)
    return 100;
  else
    return 1;
}

char GetTest(char choice, int iTest){

  char testList[5] = {'S','N','G','D','A'};
  
  if( choice != 'E' )
    return choice;
  else
    return testList[iTest]; 

}

int GetNTests(char choice){
  if( choice != 'E' )
    return 1;
  else
    return 5;
}

void ExecuteProcessingRun(int run){
    
  char choice;
  cout << " Which test/s to process? " << endl;
  cout << " 'S' - SPEtest "           << endl;
  cout << " 'N' - Nominal "           << endl;
  cout << " 'A' - APTest "            << endl;
  cout << " 'D' - DarkRateTest "      << endl;
  cout << " 'G' - GainTest     "      << endl;
  cout << " 'E' - Every Test     "    << endl;
  cin  >> choice;

  for ( int iPMT = 0 ; iPMT < GetNPMTs(run) ; iPMT++)
    for( int iTest = 0 ; iTest < GetNTests(choice) ; iTest++)
      ExecuteProcessing(run,
			GetPMT(run,iPMT),
			GetLoc(run,iPMT),
			GetTest(choice,iTest),
			GetInDirUser(),
			GetOutDirUser());
  

}

int main(int argc, char **argv)
{
  
  cout << endl;
  cout << " BinToRoot " << endl;
  cout << endl;
  
  //-------------------
  int  run    = 160001; 
  int  pmt    = 16;
  int  loc    = 0;
  char test   = 'S';
  
//   char inDir[128];
//   char outDir[128];
  
//   char digitiser = 'V';
  //---------------
  // User Options
  
  switch ( argc ){
    // No arguments passed to exectutable
    // Normal use for single file processing
    // Full user input required
  case ( 1 ):
    
    ExecuteProcessing(GetRunUser(),
		      GetPMTUser(),
		      GetLocUser(),
		      GetTestUser(),
		      GetInDirUser(),
		      GetOutDirUser(),
		      GetDigitiserUser());
    break;
  case ( 2 ):
    // One argument (option) passed to executable
    // Option '-1' - debugging mode
    // Option '1' - process run 1
    // Option 'N' - process run N (N is integer)
    char * endPtr;
    ExecuteProcessingRun(strtol(argv[1], &endPtr, 10));
    break;
  case ( 5 ):
    run = strtol(argv[1], &endPtr, 10);
    pmt = strtol(argv[2], &endPtr, 10);
    loc = strtol(argv[3], &endPtr, 10);
    test = *argv[4];
    
    ExecuteProcessing(run,pmt,loc,test);
    
    break;
  default:
    cerr << " Invalid option " << endl;
    return 0;
  }
    
  //
  //---------------
  
    

    
  return 1;  
}
