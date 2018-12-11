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
 *  $ ./BinToRoot  
 *
 * Dependencies
 *  root.cern
 *  Makefile
 *
 * Modified 
 *  gary.smith@ed.ac.uk
 *  11 12 18
 *
 *
 *
 */ 

#include <cstdlib>
#include <fstream>
#include <iostream>

#include "TFile.h"
#include "TTree.h"

#include "TH2.h"

using namespace std;

int  getNSamples(char digitiser){

  if     ( digitiser == 'V' )
    return 110;
  else if( digitiser == 'D' )
    return 1024;
  else{
    cerr << "Error: Unknown digitiser " << endl;
    return 0;
  }
  
}

int  getNVDCBins(char digitiser){

  if     ( digitiser == 'V' )
    return 16384;
  else if( digitiser == 'D' )
    return 4096;
  else{
    cerr << "Error: Unknown digitiser " << endl;
    return 0;
  }
  
}

float getVoltageRange(char digitiser){
  
  if     ( digitiser == 'V' )
    return 2.0;
  else if( digitiser == 'D' )
    return 1.0;
  else{
    cerr << "Error: Unknown digitiser " << endl;
    return 0;
  }


}

int getSampleRateInMHz(char digitiser){

  if     ( digitiser == 'V' )
    return 500;
  else if( digitiser == 'D' )
    return 5000;
  else{
    cerr << "Error: Unknown digitiser " << endl;
    return 0;
  }

}

void printDAQConfig(char digitiser){
  

  TString strDigitiser = "VME";
  
  if (digitiser=='d')
    strDigitiser = "Desktop";

  cout << endl;
  cout << "   " 
       << strDigitiser
       << " Digitiser "    << endl;
  cout << "   " 
       << getNVDCBins(digitiser)        
       << " VDC Bins "    << endl;
  cout << "   " 
       << getVoltageRange(digitiser)    
       << " Volts Range "  << endl;
  cout << "   " 
	 << getSampleRateInMHz(digitiser) 
       << " MHz sample rate " << endl;
  cout << endl;
}


float getnsPerSample(char digitiser){
  
  return (1.0e3 / (float)getSampleRateInMHz(digitiser));

}

float getDelay(char digitiser){
  
  if      ( digitiser == 'V' )
    return 54.;
  else if ( digitiser == 'D' )
    return 105.;
  else 
    return 0.;
}

float getmVPerBin(char digitiser){
  return ( 1.0e3 * getVoltageRange(digitiser) / 
	   getNVDCBins(digitiser) );
}

float getCharge(int intVDC, char digitiser = 'V'){
  
  float nsPerSample = getnsPerSample(digitiser);
  
  float mVolts = (float)intVDC * getmVPerBin(digitiser);
  
  float charge = -1. * nsPerSample * mVolts;
  
  if( digitiser == 'V' || 
      digitiser == 'D' )
    return charge; 
  else
    return 0.;

}


bool isCorrectDigitiser(int header, char digitiser){
  
  if( ( header == 244  &&  digitiser == 'V' ) || 
      ( header == 4120 &&  digitiser == 'D' ) )
    return true;
  else{
    cerr << " Error: digitiser choice does not match header info " << endl;
    return false;
  }
}

// integration windows fixed wrt trigger
// pedestal window before signal window only
float Accumulate_Fixed(short VDC, float time){

  // Integrate pedestal using 
  // 40 ns window before signal
  // And signal in 40 ns window
  if      ( time >= -40 && 
	    time <    0 )
    return((int)-VDC);
  else if ( time >= 0 &&
	    time <  40 ){
    return((int)VDC);
  }
  else
    return 0.;
  
}

// integration windows wrt pulse peak
// -10 ns before to 20 ns after
// (timeRel is time relative to minT)
// pedestal window before
float Accumulate_Peak(short VDC, float timeRel){
  
  if      ( timeRel >= -50 && 
	    timeRel <  -10 )
    return((float)-VDC);
  else if ( timeRel >= -10 &&
	    timeRel <   30 ){
    return((float)VDC);
  }
  else
    return 0.;

}

// Read in binary file and 
// write to root file
int ProcessBinaryFile(TString inFilePath,
		      TString outFilePath,
		      int    verbosity = 0,
		      char   digitiser = 'V'
		      ){
  cout << endl;
  cout << " Processing  " << inFilePath << endl;

  bool  testMode  = false;
  bool  keepGoing = true;
  int   maxEvents = 50000;
  
  if     ( verbosity == 1 )
    maxEvents = 10;
  else if( verbosity == 2 )
    maxEvents = 1;

  // Read from here
  ifstream fileStream(inFilePath);

  if(!fileStream.good()){
    cerr << endl;
    cerr << " check filename " << endl;
    return -1;
  }
  
  printDAQConfig(digitiser);
    
  // Write to here
  TFile * outFile    = new TFile(outFilePath,
				 "RECREATE",
				 "Wavedump Data");
  //---------------------
  // Event Level Data
  TTree * eventTree  = new TTree("eventTree",
				 "event-level variables");
  
  //TCanvas
  // accumulated and coverted data
  TH1F * hQ_Fixed = new TH1F("hQ_Fixed","Fixed windows;Charge (mV nS);Counts",
			     128,-560.,2000.);
  
  TH1F * hQ_Peak = new TH1F("hQ_Peak","Windows around peak;Charge (mV nS);Counts",
			    128,-560.,2000.);
  
  TH2F * hPulses = new TH2F("hPulses",
			    "hPulses;Sample;VDC",
			    getNSamples(digitiser),0,(getNSamples(digitiser)-1),
			    getNVDCBins(digitiser),0.,(getNVDCBins(digitiser)-1));
  
  TH2F * hTV = new TH2F("hTV",
			"hTV;Time (ns);Voltage (mV)",
			getNSamples(digitiser),
			0.,(getNSamples(digitiser)-1)*getnsPerSample(digitiser),
			getNVDCBins(digitiser),
			0.,getVoltageRange(digitiser)*1.0e3);

  int   event  = -1;
  
  // Note that pulses are negative polarity and
  // the sign is preserved here, therefore 
  // minima will be where the signal was largest
  short minVDC = 32767, maxVDC = -32768;  
  short minT   = 32767, maxT   = -32768;  
  int   intVDCfixed = 0, intVDCpeak = 0;
  float qFixed = 0., qPeak = 0.;

  // read in samples per event
  // vector may be better
  short pulse[getNSamples(digitiser)];
  
  eventTree->Branch("event",&event,"event/I");
  eventTree->Branch("minVDC",&minVDC,"minVDC/S");
  eventTree->Branch("maxVDC",&maxVDC,"maxVDC/S");
  eventTree->Branch("minT",&minT,"minT/S");
  eventTree->Branch("maxT",&maxT,"maxT/S");
  
  TString arrayString = "";
  arrayString.Form("pulse[%d]/S",getNSamples(digitiser));
  
  eventTree->Branch("pulse",pulse,arrayString);
  
  //---------------------
  // Sample Level Data

  short VDC = 0, sample = 0;
  
  int   fileHeader = 0;
  float floatVDC   = 0.;
  
  // Time (delay subtracted)
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
    
    //-------------------
    // event-level data
    intVDCfixed = 0, intVDCpeak = 0;
    qFixed = 0.,qPeak = 0.;
    
    // VDC range
    minVDC =  32767;
    maxVDC = -32768;  

    // time walk?
    minT   =  32767;
    // random?
    maxT   = -32768;

    //-------------------
    // pulse-level data
    sample = 0;
    VDC    = 0;    
    
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
			       digitiser)  ) {
	
	return -1;
      }
    } // end: for (int i = 0 ; i < intsPerHeader
    
    // read in pulse which comes 
    // in 2 (VME) or 4 (Desktop) bit chunks
    for (short iSample = 0; iSample < getNSamples(digitiser); iSample++){
      
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
      pulse[sample] = VDC;
      
      if     ( VDC < minVDC ){
	minVDC = VDC;
	minT   = sample;
      }
      else if( VDC > maxVDC ) {
	maxVDC = VDC;
	maxT   = sample;
      }
      
      // use time with delay subtracted so that 
      // any run can use same windows
      time = sample * getnsPerSample(digitiser) - getDelay(digitiser);
      
      // fixed window accumulations
      // add / subtract / skip sample VDC value
      intVDCfixed += Accumulate_Fixed(VDC,time);

      if(verbosity > 1)
	cout << " VDC(" << iSample << ") = " << VDC << endl;
      
    } // end: for (short iSample = 0; iSa
    
    float minTime = (float)minT*getnsPerSample(digitiser);
    float timeRelPeak = 0;
    
    // Loop over pulse again
    // necessary for when event-level variables are used
    // such as pulse minimum
    for (short iSample = 0; iSample < getNSamples(digitiser); iSample++){
      
      time = (float)iSample*getnsPerSample(digitiser) - getDelay(digitiser);

      timeRelPeak = time - minTime + getDelay(digitiser);;
	
      if(verbosity > 0){
	cout << " minTime = " << minTime << endl;
	cout << " time    = " << time << endl;
      }
      
      if( minTime > (getDelay(digitiser) - 20.)  && 
	  minTime < (getDelay(digitiser) + 20.) ){
	
	// accumulations wrt pulse peak (minimum)
	intVDCpeak += Accumulate_Peak(pulse[iSample],
				      timeRelPeak);
      }
      
      // sample vs VDC and time vs voltage plots
      // for checking signals (delay etc) 
      if( !(event%1000) && event < 100000 ){
	hPulses->Fill(iSample,pulse[iSample]);
	hTV->Fill(time + getDelay(digitiser),
		  pulse[iSample]*getmVPerBin(digitiser));
      }
      
    } // end: for (short iSample = 0; iS...
    
    if( minVDC < 0 && maxVDC > 0 )
      cout << " Warning: pulse is zero crossing " << endl;
    
    // convert accumulated VDC to charge
    qFixed = getCharge(intVDCfixed,digitiser);
    
    if( verbosity > 0 ){
      cout << endl;
      cout << " qFixed = " << qFixed << endl;
    }
    
    hQ_Fixed->Fill(qFixed);
    
    if( minTime > (getDelay(digitiser) - 20.) && 
	minTime < (getDelay(digitiser) + 20.) ){
      
      qPeak = getCharge(intVDCpeak,digitiser);
    
      hQ_Peak->Fill(qPeak);
      
      if( verbosity > 0 )
	cout << " qPeak = " << qPeak << endl;
      
    }
    
    //--------------------------------
    // Write event by event data here
    
    eventTree->Fill();
    
    if(verbosity > 0){
      
      cout << endl;
      cout << " Event " << event << endl;
      cout << " minVDC(" << minT << ") = " << minVDC << endl;
      cout << " maxVDC(" << maxT << ") = " << maxVDC << endl;
      cout << endl;
      
      if(verbosity > 1){
	
	cout << endl;
      }
    
    }
    
    if( testMode &&
	(event+1) == maxEvents )
      keepGoing = false;
    
  } // end: while loop

  // close wavedump file
  fileStream.close();	

  //--------------------------------
  // Write file info here

  eventTree->Write();
  eventTree->Delete();

  outFile->Write();
  outFile->Close();
  
  return (event+1);
}


string getDefaultFilePath(){
  return  "../../Data/wave_0.dat";
}

TString getRunFolderName(int run){

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

TString getPMTFolderName(int pmt){

  if     ( pmt < 10  )
    return "PMT000%d/";
  else if( pmt < 100 )
    return "PMT00%d/";
  else if( pmt < 1000 )
    return "PMT0%d/";
  else
    return "PMT%d/";
}

TString getTestFolderName(char test){
  
  switch(test){
  case('S'):
    return "SPEtest/";
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

TString getBinaryFilePath(TString filePath = "../../Data/",
			  int  run  = 1, 
			  int  pmt  = 1, 
			  int  loc  = 0,
			  char test = 'S',
			  int  hvstep = 0){

  // append file path
  filePath += getRunFolderName(run);
  filePath += getPMTFolderName(pmt);
  filePath += getTestFolderName(test);
  
  // append with filename
  switch(test){
  case('G'):
    filePath += "wave_%d_hv%d.dat";
    filePath.Form(filePath,run,pmt,loc,hvstep);
    break;
  default:
    filePath += "wave_%d.dat";
    filePath.Form(filePath,run,pmt,loc);
  }

  return filePath;
}

TString getRawRootFilePath(TString filePath = "./",
			   int  run  = 0, 
			   int  pmt  = 1, 
			   int  loc  = 0,
			   char test = 'S',
			   int  hvstep = 0){
  
  // default option
  if( run == 0 )
    return "outputFile.root";

  // append file path
  filePath += getRunFolderName(run);
  filePath += getPMTFolderName(pmt);
  filePath += getTestFolderName(test);
  
  // append with filename
  switch(test){
  case('G'):
    filePath += "Run_%d_PMT_%d_Loc_%d_Test_%c_HV_%d.root";
    filePath.Form(filePath,run,pmt,run,pmt,loc,test,hvstep);
    break;
  default:
    filePath += "Run_%d_PMT_%d_Loc_%d_Test_%c.root";
    filePath.Form(filePath,run,pmt,run,pmt,loc,test);
  }
  
  return filePath;
  
}

int main(int argc, char **argv)
{
  
  // ------------------
  // Optional variables
  // - see ProcessBinaryFile()

  // Printing  
  // 0 - silence (default) 
  // 1 - event-by-event
  // 2 - sample-by-sample
  int  verbosity   = 0;
  
  // 'V' - VME (default)
  // 'D' - Desktop
  char   digitiser = 'V';
  //digitiser = 'D';
  
  //-------------------
  
  //string filePath = getDefaultFilePath();
  
  int  run = 20; // 11 (underground), 20
  int  pmt = 90;
  int  loc = 0;
  char test = 'S';
  int  hvstep = 0;
  
  TString garyExternalBinary = "/Volumes/G-DRIVE/BinaryData/";
  TString garyExternalROOT   = "/Volumes/G-DRIVE/RawRootData/";
  
  // TString edinburghBinary = "";
  // edinburghBinary = "/Disk/ds-sopa-group/PPE/Watchman/BinaryData/";
  
  TString inputFilePath  = "";
  TString outputFilePath = "";
    
  //  for (int hvstep = 1 ; hvstep < 6 ; hvstep++){

  inputFilePath = getBinaryFilePath(garyExternalBinary,
				    run, pmt, loc, test, hvstep);
  
  outputFilePath = getRawRootFilePath(garyExternalBinary,
				      run, pmt, loc, test, hvstep);
  
  cout << endl;
  cout << " Binary File = " << inputFilePath  << endl;
  cout << " Root File   = " << outputFilePath << endl;
  //}

  int nEvents = ProcessBinaryFile(inputFilePath,
				  outputFilePath, 
				  verbosity, 
				  digitiser);
  
  cout << endl;
  cout << " Output file contains " << nEvents << " events " << endl; 
}
