/*****************************************************
 * A program to process raw root files produced  
 * using dat_to_root
 *
 * Author 
 *  gary.smith@ed.ac.uk
 *  15 10 2019
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
 * How to run 
 *  e.g. (test parameters are extracted from the file path)
 * $ cook_raw /path/to/RUN000001/PMT0130/SPEtest/wave_0.dat.root
 *
 * Input
 *  A .root file that was created using dat_to_root
 * 
 * Output
 *  A root file containing: 
 *      a cooked variables TTree  
 *      a meta data TTree 
 *  Monitoring Plots (optional)
 *  in the following folders:
 *     ./Plots/DAQ 
 *     ./Plots/Baseline
 *     ./Plots/Waveforms
 *   these folders must be created prior to running.
 *    (the script make_plot_directories.sh is 
 *       provided to automate this) 
 *
 * Dependencies
 *  root.cern - a working version of root is required
 *
 *  WATCHMAN common tools
 *    wmStyle.C - TStyle class settings for WATCHMAN visualisation
 *    FileNameParser.h - class for extracting file ID variables 
 * 
 */ 

#include <iostream>

#include <string>
#include "TFile.h"
#include "TTree.h"

#include "TCooker.h"

#include "FileNameParser.h"

bool Welcome(int argc);
bool IsFileReady(TFile *, char *);

int main(int argc, char * argv[]){
  
  if( !Welcome(argc) )
    return -1;
  
  TFile * inFile  = nullptr;
  TTree * tree    = nullptr;

  // object used to cook 
  // raw (root) data
  TCooker * cooker = nullptr;

  // object used for extracting file ID info 
  // from argv[], namely:
  //  Run, PMT, Test, Location, 
  FileNameParser * fNP =  nullptr;
  
  for( int iFile = 1 ; iFile < argc ; iFile++){

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
    cooker = new TCooker(tree);
    // cooker = new TCooker(tree,'D','2','P');// desktop digi, inverting amp
    //cooker = new TCooker(tree,'D','2','N');// desktop digi, no amp
       
    // set the cooker object FileID using the
    // FileNameParser object member function
    cooker->SetFileID(fNP->GetFileID());
    
    // Set output file directory 
    // to same as input file directory
    cooker->SetDir(fNP->GetDir());

    // Optional method:
    // reduce event loop for faster code testing
    // NB no check that this is lower that nentries
    int user_nentries = 100000; 
    //cooker->SetTestMode(user_nentries);

    // Apply Equipment 
    // Specific Settings

    float amp_gain = 10.;
    //amp_gain = 1.;
    short firstMaskBin = -1;
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
    cooker->DAQ();
    
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
  
  if(argc == 2){
    printf("\n  Processing single file \n");
    printf("\n ------------------------------ \n");
    return true;
  }
  else if  (argc > 2){
    printf("\n  Processing %d files \n",argc-1);
    printf("\n ------------------------------ \n");
    return true;
  }
  else{
    printf("\n  enter file as argument \n");
    printf("\n  e.g. \n");
    printf("\n  ./cook_raw ./wave_0.dat.root \n\n");
    printf("\n ------------------------------ \n");
    return false;
  }
  
  
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
