/*****************************************************
 * A program to process raw root files produced  
 * using dat_to_root
 *
 * Author 
 *  gary.smith@ed.ac.uk
 *  03 04 2020 (last modified)
 *
 * Purpose
 *  This program reads in a TTree
 *  of raw variables (ADC and HEAD)
 *  and creates an output TTree of
 *  'cooked' variables, storing 
 *  it in another root file.
 *
 * Setting Up
 *   The environment should first be set using 
 *   the WM.sh script - located in ../../
 *    
 * How to build 
 *  $ make 
 * 
 *  or
 * make -f ./Build_Options/Makefile_clang++
 *
 * How to run 
 *
 *   [ NB some test parameters will be extracted from the file path
 *     which requires the file path name to follow some conventions,
 *     specifically it must contain:
 *     /RUN000XXX    (XXX is the run number, zeros are optional) 
 *     /PMT0YYY      (YYY pmt number, leading zero is required)
 *     /TestName     (TestName must start with D - Dark Count, N - Nominal, S - SPE, A - Afterpulsing, G - Gain)
 *     ( full details in $WM_COMMON/FileNameParser.C and $WM_COMMON/testFileNameParser.C ) ]
 * 
 * $ cook_raw /my/path/to/RUN000001/PMT0130/Nominal/wave_0.dat.root
 * 
 * Input
 *  A .root file that was created using dat_to_root 
 *  (or desktop_dat_to_root)
 *
 * Output
 *  A root file containing: 
 *      a cooked variables TTree  
 *      a meta data TTree 
 *  Monitoring plots in 
 *     ./Plots/DAQ 
 * 
 * Dependencies
 *  root.cern - a working version of root is required
 *
 *  WATCHMAN common tools (see $WM_COMMON folder)
 *    wmStyle.C - TStyle class settings for WATCHMAN visualisation
 *    FileNameParser.C - class for extracting file ID variables 
 * 
 */ 

#include <iostream>

#include <string>
#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"

#include "TCooker.h"

#include "FileNameParser.h"

bool Welcome(int argc);
void PrintUsage();
bool IsFileReady(TFile *, char *);

int main(int argc, char * argv[]){
  
  if( !Welcome(argc) )
    return -1;
  
  // default to VME digitiser
  char digitiser = 'V';  
  // CAENs frequency setting system 
  // only used for digitiser = 'D' 
  // default '2' = 1 GHz (for desktop digi)
  char sampling  = '2';  
  
  // pulse polarity
  // 'N' for non-inverting amp
  char polarity  = 'N';

  float amp_gain = 10.;

  for ( int i = 2; i < argc ; i = i+2 ) {
    if     ( string(argv[i]) == "-d" ) digitiser = *argv[i+1];
    else if( string(argv[i]) == "-s" ) sampling  = *argv[i+1];
    else if( string(argv[i]) == "-p" ) polarity  = *argv[i+1];
    else if( string(argv[i]) == "-g" ) amp_gain  = stoi(argv[i+1]);
    else {
      PrintUsage();
      return 1;
    }
  }

  TFile * inFile  = nullptr;
  TTree * tree    = nullptr;

  gSystem->Exec("mkdir -p ./Plots/");

  // object used to cook 
  // raw (root) data
  TCooker * cooker = nullptr;
  
  // object used for extracting file ID info 
  // from argv[], namely:
  //  Run, PMT, Test, Location, 
  FileNameParser * fNP =  nullptr;
  
  for( int iFile = 1 ; iFile < argc ; iFile++){
    
    if(string(argv[iFile]) == "-d" ||
       string(argv[iFile]) == "-s") {
      iFile = iFile + 2;
      continue;
    }
       
    //-------------------
    //-------------------
    // Setting Up

    // Check root file
    inFile = new TFile(argv[iFile],"READ");
    if( !IsFileReady(inFile,argv[iFile]) )
      continue;
    
    // argv should be full path to data file
    // in standard WATCHMAN PMT Testing format
    // (option 1 is for use with this format)
    fNP = new FileNameParser(argv[iFile],1);
    
    // Get raw data tree, which is always called 'T'
    inFile->GetObject("T",tree); 
    
    // initalise TCooker object using 
    // tree from input file
    cooker = new TCooker(tree,
			 digitiser,sampling,polarity); // optional
    
    // set the cooker object FileID using the
    // FileNameParser object member function
    cooker->SetFileID(fNP->GetFileID());
    
    //
    cooker->SetRun(fNP->GetRun());
    cooker->SetPMT(fNP->GetPMT());
    cooker->SetLoc(fNP->GetLoc());
    cooker->SetTest(fNP->GetTest());
    cooker->SetHVStep(fNP->GetHVStep());
    //

    // Set output file directory 
    // to same as input file directory
    cooker->SetDir(fNP->GetDir());

    // Optional method:
    // reduce event loop for faster code testing
    // NB no check that this is lower that nentries
    // int user_nentries = 100000; 
    // cooker->SetTestMode(user_nentries);

    // Apply Equipment 
    // Specific Settings


    //amp_gain = 1.;
    short firstMaskBin = -1; // -1 means no mask
    //firstMaskBin = 1000;
    //firstMaskBin = 988;
    
    // scale amplitudes to 
    // match 10x preamp gain   
    cooker->SetAmpGain(amp_gain);

    // set known bad ADC channels
    // to event-by-event baseline values
    // arg is first (lowest) bin masked 
    cooker->SetFirstMaskBin(firstMaskBin);
    
    cooker->PrintConstants();

    //-------------------
    // DAQ info
    //  Print mean trigger rate
    //  Save: rate,timing and event plots
    //  (desktop digitiser not yet implemented)
    if(digitiser=='V'){
      gSystem->Exec("mkdir -p ./Plots/DAQ");
      cooker->DAQ();
    }
    
    //-------------------
    //-------------------
    // Cook Data
    
    
    // Calculate basic variables
    // NB: ADC pulse is flipped for negative pulse polarity data     
    
    // Save meta data tree
    // Save cooked data tree
    cooker->Cook();
    
    inFile->Delete();
    
    delete fNP;
  }
  
  return 1;
}


bool Welcome(int argc){
  
  printf("\n      --------------------    ");
  printf("\n      --------------------  \n");
  printf("\n            cook_raw        \n");
  printf("\n      --------------------    ");
  printf("\n      --------------------  \n");

  printf("\n ------------------------------ \n");
  
  if(argc > 1){
    printf("\n  Processing file/s \n");
    printf("\n ------------------------------ \n");
    return true;
  }
  else{
    printf("\n  enter file as argument \n");
    printf("\n  e.g. \n");
    printf("\n  ./cook_raw /path/to/wave_0.dat.root \n\n");
    printf("\n ------------------------------ \n");
    return false;
  }
}

void PrintUsage() {
  cerr << " Usage: " << endl;
  cerr << " cook_raw /path/to/file.dat.root [-d desktop character ] [-s sample setting]  "
       << endl;
  cerr << " -d options for digitiser: 'V' VME digitiser (default), 'D' Desktop digitiser " 
       << endl;
  cerr << " -s options for sample setting (desktop digitiser only): 0 - 5 GHz, 1 - 2.5 GHz, 2 - 1 GHz (default), 3 - .75 GHz "
       << endl;
}



bool IsFileReady(TFile * inFile, char * arg){
  
  if ( !inFile || !inFile->IsOpen()) {
    fprintf(stderr,"\n Error, Check File: %s \n",arg);
    return false;
  }
  else {
    printf("\n ------------------------------ \n");
    printf("\n  Input File:       ");
    printf("\n    %s  \n",arg);

    return true;
  }
}
