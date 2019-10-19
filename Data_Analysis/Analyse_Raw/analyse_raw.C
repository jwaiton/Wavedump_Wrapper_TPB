/*****************************************************
 * A program to analyse raw root files produced  
 * using dat_to_root
 *
 * Author 
 *  gary.smith@ed.ac.uk
 *  15 10 2019
 *
 * Purpose
 *  This program reads in a TTree
 *  of raw variables (ADC and HEAD)
 *
 * Setting Up
 *   The environment should first be set using 
 *   the WM.sh script - located in ../../
 * 
 * How to build
 *  $ make 
 *
 * How to run 
 *  e.g.
 * $ ./analyse_raw /path/to/RUN000001/PMT0130/SPEtest/wave_0.dat.root
 * 
 * Input
 *  .root file created with dat_to_root
 * 
 * Output
 *  Monitoring Plots can be saved in directory structure
 *     ./Plots/DAQ 
 *     ./Plots/Waveforms
 *   which must be created prior to running.
 *    (the script make_plot_directories.sh is 
 *       provided to automate this) 
 * 
 * Dependencies
 *  root.cern - a working version of root is required
 *
 *  wmStyle.C - TStyle class settings for WATCHMAN visualisation
 *   This file is included in the distribution in
 *   /path/to/Wavedump_Wrapper/Common_Tools/
 * 
 */ 

#include <iostream>

#include <string>
#include "TFile.h"
#include "TTree.h"

#include "TRawAnalyser.h"

bool Welcome(int argc);
bool IsFileReady(TFile *, char *);

int main(int argc, char * argv[]){
  
  if( !Welcome(argc) )
    return -1;
     
  TFile * inFile  = nullptr;
  TTree * tree    = nullptr;

  // object used to analyse 
  // raw (root) data
  TRawAnalyser * analyser = nullptr;

  for( int iFile = 1 ; iFile < argc ; iFile++){

    //-------------------
    //-------------------
    // Setting Up
    
    // Check root file
    inFile = new TFile(argv[iFile],"READ");
    if( !IsFileReady(inFile,argv[iFile]) )
      continue;
    
    printf("\n ------------------------------ \n");
    printf("\n  Input File:       ");
    printf("\n    %s  \n",argv[iFile]);
    
    // Get tree called 'T'
    inFile->GetObject("T",tree); 
    
    // initalise TRawAnalyser object using 
    // tree from input file
    analyser = new TRawAnalyser(tree);
    //analyser = new TRawAnalyser(tree,'D');// desktop digi version
    
    // Optional method:
    // reduce event loop for faster code testing
    // NB no check that this is lower that nentries
    int user_nentries = 100000; 
    //analyser->SetTestMode(user_nentries);
    
    //-------------------
    //-------------------
    // Monitor Raw Data 
    analyser->PrintConstants();
    
    // DAQ info
    //  Print mean trigger rate
    //  Save: rate,timing and event plots
    analyser->DAQ();
    
    //.....
    // In progress - currently saves one waveform
    //
    // Plot Waveforms, options: 
    //    'w' waveform only
    //    'f' fft only
    //    'b' waveform & fft
    char option = 'b';  
    analyser->Waveform(option);    
    
    inFile->Delete();
  }
  
  return 1;
}


bool Welcome(int argc){
  
  printf("\n      --------------------    ");
  printf("\n      --------------------  \n");
  printf("\n          analyse_raw       \n");
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
    printf("\n  ./analyse_raw ./wave_0.dat.root \n\n");
    printf("\n ----------------------------------- \n");
    return false;
  }
  
  
}

bool IsFileReady(TFile * inFile, char * arg){
  
  if ( !inFile || !inFile->IsOpen()) {
    fprintf(stderr,"\n Error, Check File: %s \n",arg);
    return false;
  }
  else 
    return true;
}
