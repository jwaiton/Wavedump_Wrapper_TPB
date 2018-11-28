/***************************************************
 * A program to process wavedump output files
 *
 * Author 
 *  Gary Smith
 *  https://github.com/gsmith23
 *  28 11 18
 *
 * Adapted from
 * SPE_Gen.cpp 
 *  Tomi Akindele
 *  https://github.com/akindele
 *  04 10 2018
 *
 * Purpose
 *  This program read the binary 
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
 */ 

#include <cstdlib>
#include <fstream>
#include <iostream>

#include "TFile.h"
#include "TTree.h"

#include "TH1.h"

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

bool isCorrectDigitiser(int header, char digitiser){
  
  if( ( header == 244  &&  digitiser == 'V' ) || 
      ( header == 4120 &&  digitiser == 'D' ) )
    return true;
  else{
    cerr << " Error: digitiser choice does not match header info " << endl;
    return false;
  }
}

float getCharge(short VDC, char digitiser = 'V'){

  // To do: substitute with parameters
  // VME   (Desktop)
  // 2.0   (0.2)       ns per time bin
  // 16384 (4096)      14 or 12 bit
  // 2.0   (1.0)*1.0e3 mV range 
  
  if     ( digitiser == 'V')
    return (-1.*(float)VDC*2.0/16384.0*2.0e3);
  else if( digitiser == 'D')
    return (-1.*(float)VDC*.2/4096.0*1.0e3);
  else 
    return 99999;
}

// integration windows fixed wrt trigger
// pedestal window before signal window only
float Accumulate_V1(short VDC, short sample){
  
  if      ( sample >= 6 && 
	    sample < 26 )
    return((int)-VDC);
  else if ( sample >= 27 &&
	    sample <  47 )
    return((int)VDC);
  else
    return 0.;
  
}

// integration window fixed wrt trigger
// pedestal windows before and after signal window
float Accumulate_V2(short VDC, short sample){
  
  if      ( sample >= 6 && 
	    sample < 16 )
    return((int)-VDC);
  else if ( sample >= 17 &&
	    sample <  57 )
    return((int)VDC);
  else if ( sample >= 57 &&
	    sample <  87 )
    return((int)-VDC);
  else
    return 0.;
   
}

// integration windows wrt pulse peak
// pedestal windows before and after signal window
float Accumulate_V3(short VDC, short sample, short minT){
  
  if      ( sample >= (minT-30) && 
	    sample <  (minT-10) )
    return((int)-VDC)*3/4.;
  else if ( sample >= (minT-10) &&
	    sample <  (minT+20) )
    return((int)VDC);
  else if ( sample >= (minT+20) &&
	    sample <  (minT+40) )
    return((int)-VDC)*3/4.;
  else
    return 0.;

}

// integration window wrt pulse peak
// pedestal windows before and after signal window
// ( double counting bug as in SPE_Gen )
float Accumulate_V4(short VDC, short sample, short minT){

  float result = 0;
  
  if ( sample >= (minT-30) && 
       sample <= (minT-20) )
    result += ((int)-1*VDC)*3/4.;
  
  if ( sample >= (minT-20) && 
       sample <= (minT-10) )
    result += ((int)-1*VDC)*3/4.;
  
  if ( sample >= (minT-10) &&
       sample <= (minT) )
    result += (int)VDC;

  if ( sample >= (minT) &&
       sample <= (minT+10) )
    result += (int)VDC;
      
  if ( sample >= (minT+10) &&
       sample <= (minT+20) )
    result += (int)VDC;
  
  if ( sample >= (minT+20) &&
       sample <= (minT+30) )
    result += ((int)-1*VDC)*3/4.;

  if ( sample >= (minT+30) &&
       sample <= (minT+40) )
    result += ((int)-1*VDC)*3/4.;
  
  return result;

}

int ProcessBinaryFile(string fileName,
		      char digitiser = 'D',
		      int  verbosity = 0
		      ){

  bool  testMode  = false;
  bool  keepGoing = true;
  int   maxEvents = 50000;

  // Read from here
  ifstream fileStream(fileName);

  if(!fileStream.good()){
    cerr << endl;
    cerr << " check filename " << endl;
    return -1;
  }

  // Write to here
  TFile * outFile    = new TFile("outputFile.root",
				 "RECREATE",
				 "Wavedump Data");
  //---------------------
  // Event Level Data
  TTree * eventTree  = new TTree("eventTree",
				 "event-level variables");
  
  // accumulated and coverted data
  TH1F * hCharge1 = new TH1F("hCharge1","hCharge1;Charge (mV nS);Counts",
			     128,-560.,2000.);
  
  TH1F * hCharge2 = new TH1F("hCharge2","hCharge2;Charge (mV nS);Counts",
			     128,-560.,2000.);
  
  TH1F * hCharge3 = new TH1F("hCharge3","hCharge3;Charge (mV nS);Counts",
			     128,-560.,2000.);

  TH1F * hCharge4 = new TH1F("hCharge4","hCharge4;Charge (mV nS);Counts",
			     128,-560.,2000.);
  
    
  int   event  = -1;
  
  short minVDC = 32767, maxVDC = -32768;  
  short minT   = 32767, maxT   = -32768;  
  int   intVDC1 = 0,  intVDC2 = 0,  intVDC3 = 0,  intVDC4 = 0;
  float charge1 = 0., charge2 = 0., charge3 = 0., charge4 = 0.;  

  // read in samples per event
  short pulse[110];
  
  eventTree->Branch("event",&event,"event/I");
  eventTree->Branch("minVDC",&minVDC,"minVDC/S");
  eventTree->Branch("maxVDC",&maxVDC,"maxVDC/S");
  eventTree->Branch("minT",&minT,"minT/S");
  eventTree->Branch("maxT",&maxT,"maxT/S");
  eventTree->Branch("pulse",pulse,"pulse[110]/S");


  //---------------------
  // Sample Level Data

  short VDC = 0, sample = 0;
  
  int   fileHeader = 0;
  float floatVDC   = 0.;
  

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
    intVDC1 = 0, intVDC2 = 0, intVDC3 = 0, intVDC4 = 0;
    charge1 = 0.,charge2 = 0.,charge3 = 0.,charge4 = 0.;
    
    // VDC range (check zero crossing)
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
      
      if     ( digitiser == 'V' ){
	fileStream.read((char*)&VDC,2); //read 2 bits
      }
      
      else if( digitiser == 'D' ){
	fileStream.read((char*)&floatVDC,sizeof(float));// read 4 bits
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
      
      // fixed window accumulations
      // add or subtract
      intVDC1 += Accumulate_V1(VDC,iSample);
      intVDC2 += Accumulate_V2(VDC,iSample);
      
      //--------------------------------
      // Sample by sample data here
      
      if(verbosity > 1)
	cout << " VDC(" << iSample << ") = " << VDC << endl;
      
    } // end: for (short iSample = 0; iSa

    // Loop over pulse again
    for (short iSample = 0; iSample < getNSamples(digitiser); iSample++){
      
      if(minT > 30 && minT < 62.4 ){
	// accumulations wrt pulse peak (minimum)
	intVDC3 += Accumulate_V3(pulse[iSample],iSample,minT);
	intVDC4 += Accumulate_V4(pulse[iSample],iSample,minT);
      }
    }

    
    if( minVDC < 0 && maxVDC > 0 )
      cout << " Warning: pulse is zero crossing " << endl;
    
    // convert accumulated VDC to charge
    charge1 = getCharge(intVDC1,digitiser);
    charge2 = getCharge(intVDC2,digitiser);
    charge3 = getCharge(intVDC3,digitiser);
    charge4 = getCharge(intVDC4,digitiser);
    
    hCharge1->Fill(charge1);
    hCharge2->Fill(charge2);
    
    if(minT > 30 && minT < 62.4 ){
      hCharge3->Fill(charge3);
      hCharge4->Fill(charge4);      
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
      
      if(verbosity > 2)
	cout << endl;
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


string getFilename(char digitiser){
  
  if     ( digitiser == 'V')
    return  "../../Data/wave_0.dat";
  else if( digitiser == 'V')
    return  "../../Data/wave_0_Desk.dat";
  else 
    return "../../";
  
}

int main(int argc, char **argv)
{
  
  // 'D' - Desktop
  // 'V' - VME
  char   digitiser = 'V';
  
  string fileName = getFilename(digitiser);
  
  // 0 - silence, 1 - event-by-event, 2 - sample-by-sample
  int  verbosity   = 0;
  
  cout << " The binary file is called  " << fileName << endl;
  
  int nEvents;
  
  nEvents = ProcessBinaryFile(fileName,digitiser,verbosity);
  
  cout << " This file contains " << nEvents << " events " << endl; 
}
