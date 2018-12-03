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


float getnsPerSample(char digitiser){
  
  return (1.0e3 / (float)getSampleRateInMHz(digitiser));

}

  

float getCharge(int intVDC, char digitiser = 'V'){
  
  float nYBins      = (float)getNVDCBins(digitiser);
  float mVRange     = 1.0e3 * getVoltageRange(digitiser);
  float nsPerSample = getnsPerSample(digitiser);

  float charge = -1. * (float)intVDC * nsPerSample * mVRange / nYBins;    
  
  if( digitiser == 'V' || digitiser == 'D' )
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
float Accumulate_V1(short VDC, short sample){
  
    if      ( sample >= 6 && 
	    sample < 26 )
    return((int)-VDC);
  else if ( sample >= 27 &&
	    sample <  47 )
    return((int)VDC);
  else
    return 0.;
  
//   if      ( sample >= 60 && 
// 	    sample < 260 )
//     return((int)-VDC);
//   else if ( sample >= 270 &&
// 	    sample <  470 )
//     return((int)VDC);
//   else
//     return 0.;
  
}

// integration window fixed wrt trigger
// pedestal windows before and after signal window
float Accumulate_V2(short VDC, short sample){
  
  if      ( sample >= 6 && 
	    sample < 16 )
    return((float)-VDC);
  else if ( sample >= 17 &&
	    sample <  57 )
    return((float)VDC);
  else if ( sample >= 57 &&
	    sample <  87 )
    return((float)-VDC);
  else
    return 0.;
   
}

// integration windows wrt pulse peak
// pedestal windows before and after signal window
float Accumulate_V3(short VDC, short sample, short minT){

//   cout << " minT   = " << minT   << endl;
//   cout << " sample = " << sample << endl;
  
  if      ( sample >= (minT-30) && 
	    sample <  (minT-10) )
    return((float)-VDC)*3./4.;
  else if ( sample >= (minT-10) &&
	    sample <  (minT+20) )
    return((float)VDC);
  else if ( sample >= (minT+20) &&
	    sample <  (minT+40) )
    return((float)-VDC)*3/4.;
  else
    return 0.;

}

// integration window wrt pulse peak
// pedestal windows before and after signal window
// ( double counting bug as in SPE_Gen )
//float Accumulate_V4(short VDC, float sample, float fMinTns){
float Accumulate_V4(short VDC, int sample, int minT){
  
  float result = 0;

  if ( sample >= (minT - 30) && 
       sample <= (minT - 20) )
    result += ((float)-1*VDC)*3/4.;
  
  if ( sample >= (minT - 20) && 
       sample <= (minT - 10) )
    result += ((float)-1*VDC)*3/4.;
  
  if ( sample >= (minT - 10) &&
       sample <= (minT) )
    result += (float)VDC;

  if ( sample >= (minT) &&
       sample <= (minT + 10) )
    result += (float)VDC;
      
  if ( sample >= (minT + 10) &&
       sample <= (minT + 20) )
    result += (float)VDC;
  
  if ( sample >= (minT + 20) &&
       sample <= (minT + 30) )
    result += ((float)-1*VDC)*3/4.;

  if ( sample >= (minT + 30) &&
       sample <= (minT + 40) )
    result += ((float)-1*VDC)*3/4.;
  
  return result;

//   if ( sample >= (minT - 300) && 
//        sample <= (minT - 200) )
//     result += ((float)-1*VDC)*3/4.;
  
//   if ( sample >= (minT - 200) && 
//        sample <= (minT - 100) )
//     result += ((float)-1*VDC)*3/4.;
  
//   if ( sample >= (minT - 100) &&
//        sample <= (minT) )
//     result += (float)VDC;

//   if ( sample >= (minT) &&
//        sample <= (minT + 100) )
//     result += (float)VDC;
      
//   if ( sample >= (minT + 100) &&
//        sample <= (minT + 200) )
//     result += (float)VDC;
  
//   if ( sample >= (minT + 200) &&
//        sample <= (minT + 300) )
//     result += ((float)-1*VDC)*3/4.;

//   if ( sample >= (minT + 300) &&
//        sample <= (minT + 400) )
//     result += ((float)-1*VDC)*3/4.;
  
  return result;

}

int ProcessBinaryFile(string filePath,
		      int    verbosity = 0,
		      char   digitiser = 'V'
		      ){

  cout << " Processing  " << filePath << endl;

  bool  testMode  = true;
  bool  keepGoing = true;
  int   maxEvents = 50000;
 
  if     ( verbosity == 1 )
    maxEvents = 10;
  else if( verbosity == 2 )
    maxEvents = 1;

  // Read from here
  ifstream fileStream(filePath);

  if(!fileStream.good()){
    cerr << endl;
    cerr << " check filename " << endl;
    return -1;
  }


  if( verbosity > 0 ){
    cout << endl;
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

  
  
  TH2F * hPulses = new TH2F("hPulses",
			    "hPulses;Sample;VDC",
			    getNSamples(digitiser),0,(getNSamples(digitiser)-1),
			    getNVDCBins(digitiser),0.,(getNVDCBins(digitiser)-1));
      
  int   event  = -1;
  
  short minVDC = 32767, maxVDC = -32768;  
  short minT   = 32767, maxT   = -32768;  
  float fVDC1   = 0,  fVDC2   = 0,  fVDC3   = 0,  fVDC4   = 0;
  float charge1 = 0., charge2 = 0., charge3 = 0., charge4 = 0.;  

  // read in samples per event
  // vector maybe better
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
    fVDC1 = 0, fVDC2 = 0, fVDC3 = 0, fVDC4 = 0;
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
      fVDC1 += Accumulate_V1(VDC,iSample);
      fVDC2 += Accumulate_V2(VDC,iSample);
      
      //--------------------------------
      // Sample by sample data here
      
      if(verbosity > 1)
	cout << " VDC(" << iSample << ") = " << VDC << endl;
      
    } // end: for (short iSample = 0; iSa


    float fMinTns = 0;
    
    // Loop over pulse again
    for (short iSample = 0; iSample < getNSamples(digitiser); iSample++){

      fMinTns     = (float)minT*getnsPerSample(digitiser);      
      
      //if(fMinTns > 60. && fMinTns < 124.8 ){
      
      if( minT > 300 && minT < 624  ){

	// accumulations wrt pulse peak (minimum)
	fVDC3 += Accumulate_V3(pulse[iSample],iSample,minT);
	
// 	fVDC4 += Accumulate_V4(pulse[iSample],
// 			       (float)iSample,
// 			       fMinTns);

	fVDC4 += Accumulate_V4(pulse[iSample],
			       iSample,
			       minT);
	
	
// 	cout << endl;
// 	cout << " fMinTns = " << fMinTns << endl;
// 	cout << " fVDC4   = " << fVDC4   << endl;
	
      }
      
      if( !( event%10) )
	hPulses->Fill(iSample,pulse[iSample]);
      
    } // end: for (short iSample = 0; iS...
    
    if( minVDC < 0 && maxVDC > 0 )
      cout << " Warning: pulse is zero crossing " << endl;
    
    // convert accumulated VDC to charge
    charge1 = getCharge(fVDC1,digitiser);
    charge2 = getCharge(fVDC2,digitiser);
    
    if( verbosity > 0 ){
      cout << endl;
      cout << " charge1 = " << charge1 << endl;
      cout << " charge2 = " << charge2 << endl;
    }
    
    hCharge1->Fill(charge1);
    hCharge2->Fill(charge2);
    
    fMinTns     = (float)minT*getnsPerSample(digitiser);      
    
    //if(fMinTns > 60. && fMinTns < 124.8 ){
    if( minT > 300 && minT < 624  ){
      
      charge3 = getCharge(fVDC3,digitiser);
      charge4 = getCharge(fVDC4,digitiser);
      
      if( verbosity > 0 ){ 
	cout << " charge3 = " << charge3 << endl;
	cout << " charge4 = " << charge4 << endl;
      }
	
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


string getFilePath(){
  return  "../../Data/wave_0.dat";
}

string getFilePathBine(){

  return "../../Data/wave_0_Bine.dat";
}

string getEdFileNPath(char digitiser){
  
  if     ( digitiser == 'V')
    return  "/Disk/ds-sopa-group/PPE/Watchman/BinaryData/RUN000001/PMT0001/SPEtest/wave_0.dat";
  else if( digitiser == 'D')
    return  "../../Data/wave_0_Desk.dat";
  else 
    return "../../";
  
}

int main(int argc, char **argv)
{
  
  // ------------------
  // Optional variables
  // - see ProcessBinaryFile()
  
  // 0 - silence (default) 
  // 1 - event-by-event
  // 2 - sample-by-sample
  int  verbosity   = 0;
  
  // Printing
  // 'V' - VME (default)
  // 'D' - Desktop
  char   digitiser = 'V';
  //digitiser = 'D';
  
  //-------------------
  
  string filePath = getFilePath();
  //string filePath = getFilePathBine();
  
  int nEvents = ProcessBinaryFile(filePath,
				  verbosity,
				  digitiser);
  
  cout << " Output file contains " << nEvents << " events " << endl; 
}
