/*****************************************************
 * A program to process raw root files produced  
 * using dat_to_root
 *
 * Author 
 *  gary.smith@ed.ac.uk
 *  13 10 2019
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
 *  e.g.
 * $ ./cook_raw /path/to/RUN000001/PMT0130/SPEtest/wave_0.dat.root
 * 
 * Input
 *  .root file created with dat_to_root
 * 
 * Output
 *  Monitoring Plots can be saved in directory structure
 *     ./Plots/DAQ 
 *     ./Plots/Noise
 *     ./Plots/Baseline
 *     ./Plots/Waveforms
 *   which must be created prior to running.
 *    (the script make_plot_directories.sh is 
 *       provided to automate this) 
 * 
 *  A root file of: 
 *      cooked variables 
 *      meta data 
 *  can optionallt be saved
 *  in the current folder.
 *
 * Dependencies
 *  root.cern - a working version of root is required
 *
 *  wmStyle.C - TStyle class settings for WATCHMAN visualisation
 *   This file is included in the distribution in
 *   /path/to/Wavedump_Wrapper/Common_Tools/
 * 
 *  FileNameParser.h - class for accessing file ID
 *   also included in common tools
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
    
    // argv should be full path to data file
    // in standard WATCHMAN PMT Testing format
    // (option 1 is for use with this format)
    fNP = new FileNameParser(argv[iFile],1);

    // Check root file
    inFile = new TFile(argv[iFile],"READ");
    if( !IsFileReady(inFile,argv[iFile]) )
      continue;
    
    printf("\n ------------------------------ \n");
    printf("\n  Input File:       ");
    printf("\n    %s  \n",argv[iFile]);
    
    // Get tree called 'T'
    inFile->GetObject("T",tree); 
    
    // initalise TCooker object using 
    // tree from input file
    cooker = new TCooker(tree);
    //cooker = new TCooker(tree,'D');// desktop digi version
       
    // set the cooker object FileID using the
    // FileNameParser object member function
    cooker->SetFileID(fNP->GetFileID().c_str());

    // Optional method:
    // reduce event loop for faster code testing
    // NB no check that this is lower that nentries
    int user_nentries = 100000; 
    //cooker->SetTestMode(user_nentries);
    
    //-------------------
    //-------------------
    // Monitor Raw Data 
    cooker->PrintConstants();
    
    // DAQ info
    //  Print mean trigger rate
    //  Save: rate,timing and event plots
    cooker->DAQ();
    
    //.....
    // In progress - currently saves one waveform
    //
    // Plot Waveforms, options: 
    //    'w' waveform only
    //    'f' fft only
    //    'b' waveform & fft
    char option = 'b';  
    cooker->Waveform(option);    

    //
    //-------------------
    //-------------------
    // Cook Data
    
    // 'Cook' to mV and ns
    // find peak voltage
    // and peak sample
    // Save meta data tree
    // Save cooked data tree
    cooker->Cook();
    
    // Data has been cooked
    //-------------------


    //-------------------
    //-------------------
    // Monitor/Analyse Cooked Data
        
    // re-connect to file
    // and tree
    cooker->InitCookedData();
    
    // Monitor Noise
    // plot raw variables: min, max, PPV, mean
    // print noise rate @ -5 mV and -10 mV wrt mean
    cooker->Noise();
    
    // Baseline investigation (not applied to data)
    // plot: baseline, vs event, peak vs baseline
    cooker->Baseline();
    
    // Delete outFile pointer
    cooker->CloseCookedFile();
    
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
  else 
    return true;
}
