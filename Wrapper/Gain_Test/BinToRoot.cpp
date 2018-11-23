/***************************************************
 * A program to process wavedump output files
 *
 * Author 
 *  Gary Smith
 *  https://github.com/gsmith23
 *  20 11 18
 *
 * Adapted from
 * SPE_Gen.cpp 
 *  Tomi Akindele
 *  https://github.com/akindele
 *  04 10 2018
 *
 * Purpose
 *  DAQ reads out to binary file.
 *  This program de-codes the file
 *  and reads the pulse data in 
 *  to a root TTree.
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

int ProcessBinaryFile(string fileName,
		      char digitiser = 'D',
		      int  verbosity = 0
		      ){

  // Read from here
  ifstream fileStream(fileName);

  // Write to here
  TFile * outFile    = new TFile("outputFile.root",
				 "RECREATE",
				 "Wavedump Data");
  //---------------------
  // Event Level Data
  TTree * eventTree  = new TTree("eventTree",
				 "event-level variables");
  
  
  int   event = -1;
  float minVDC = 0., maxVDC = 0.;  
  
  short sampleArr[10000000];
  
  eventTree->Branch("event",&event,"event/I");
  eventTree->Branch("minVDC",&minVDC,"minVDC/F");
  eventTree->Branch("maxVDC",&maxVDC,"maxVDC/F");

  //---------------------
  // Sample Level Data

  short VDC = 0, sample = 0;
  
  int   fileHeader  = 0;
  int   eventHeader = 0.;    
  float floatVDC = 0.;
  
  TTree * sampleTree = new TTree("sampleTree",
				 "sample-level variables");  
  
  sampleTree->Branch("event",&event,"event/I");
  sampleTree->Branch("sample",&sample,"sample/S");
  sampleTree->Branch("VDC",&VDC,"VDC/S");
  
  // read in data from streamer object
  // until the end of the file
  while ( fileStream.is_open() && 
	  fileStream.good()    && 
	  !fileStream.eof()       ){
    
    event++;
    
    // data to be recorded for each pulse
    sample = 0;
    VDC    = 0;    
    
    eventHeader = 0.;    
    
    // range will be used to check for 
    // zero crossing
    minVDC = 100000.;
    maxVDC = -100000.;  
    
    // read in header info which comes 
    // as six char sized chunks
    for (int iHeader = 0 ; iHeader < 6 ; iHeader++ ){
      
      fileHeader = 0;
      
      // read in 4 bit (char) segment
      fileStream.read( (char*)&fileHeader,
		       sizeof(int)); 
      
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
      
      if     ( VDC < minVDC ){
	minVDC = VDC;
      }
      else if( VDC > maxVDC ) {
	maxVDC = VDC;
      }
      
      //--------------------------------
      // Write sample by sample data here
      
      sample = iSample;

      sampleTree->Fill();
      
      if(verbosity > 1)
	cout << " VDC(" << iSample << ") = " << VDC << endl;
      
    }
    
    if( minVDC < 0 && maxVDC > 0 )
      cout << " Warning: pulse is zero crossing " << endl;
    

    //--------------------------------
    // Write event by event data here
    
    eventTree->Fill();
    
    if(verbosity > 0){
      cout << endl;
      cout << " minVDC(" << event << ") = " << minVDC << endl;
      cout << " maxVDC(" << event << ") = " << maxVDC << endl;
      
      if(verbosity > 2)
	cout << endl;
    }
  
  } // end: while loop

  // close wavedump file
  fileStream.close();	

  //--------------------------------
  // Write file info here

  sampleTree->Write();
  sampleTree->Delete();

  eventTree->Write();
  eventTree->Delete();

  outFile->Write();
  outFile->Close();
  
  
  return (event+1);
}


int main(int argc, char **argv)
{
  
  // 'D' - Desktop
  // 'V' - VME
  char   digitiser = 'V';
  string fileName = "../../Data/wave_0_VME.dat";
  
  // 0 - silence, 1 - event-by-event, 2 - sample-by-sample
  int  verbosity   = 0;
  
  if( digitiser == 'D' )
    fileName = "../../Data/wave_0.dat";
  
  cout << " The binary file is called  " << fileName << endl;
  
  int nEvents = ProcessBinaryFile(fileName,
				  digitiser,
				  verbosity);
  
  cout << " This file contains " << nEvents << " events " << endl; 
}
