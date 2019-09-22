/*****************************************************
 * A program to process raw root files from wavedump
 *
 * Author 
 *  gary.smith@ed.ac.uk
 *  17 09 2019
 *
 * Purpose
 *  This program reads in a TTree
 *  of raw variables (ADC and HEAD)
 *  and created an output TTree of
 *  'cooked' variables and stores 
 *  it in a root file.
 *
 * How to build
 *  $ make 
 *
 * How to run
 *
 * $ ./cook_raw wave_0.dat.root
 * 
 * Input
 *  .root file created with dat_to_root
 * 
 * Output
 *  Monitoring Plots are saved in directory structure
 *    ./Plots/DAQ 
 *    ./Plots/Noise
 *  which must be created prior to running
 *  A root file of cooked variables is saved.
 * 
 *
 * Dependencies
 *  root.cern
 *
 * Known issues
 *   Under linux you will likely be required to add 
 *   the shared (.so) file location to your library path 
 *   ie add the path to this directory  
 *  e.g. add the following line to your .bashrc file:
 *   export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/insert/the/path/here/
 */ 

#include <iostream>

#include <string>
#include "TFile.h"
#include "TTree.h"
#include "TCooker.h"

bool Welcome(int argc);
bool IsFileReady(TFile *, char *);

int main(int argc, char * argv[]){
  
  if( !Welcome(argc) )
    return -1;
     
  TFile * inFile = nullptr;
  TTree * tree   = nullptr;
  
  // object used to cook 
  // raw (root) data
  TCooker * cooker = nullptr;
  
  // Loop over files
  for( int iFile = 1 ; iFile < argc ; iFile++){

    // Check file
    inFile = new TFile(argv[iFile],"READ");
    if( !IsFileReady(inFile,argv[iFile]) )
      continue;
    
    printf("\n  Input File:       \n");
    printf("\n    %s  \n",argv[iFile]);
    
    // Get tree called 'T'
    inFile->GetObject("T",tree); 
    
    // initalise object using tree
    // from input file
    cooker = new TCooker(tree);
    
    cooker->PrintConstants();

    int user_nentries = 100000;
    cooker->SetTestMode(user_nentries);
    
    // DAQ info
    // calculate mean trigger rate
    // rate,timing and event plots
    //cooker->DAQ();
    
    //-------------------
    // Cooking
    
    // cook to mV and ns
    // find peak voltage
    // and peak sample
    //
    // write tree to file
    // (which remains open)
    cooker->Cook();
    
    //-------------------
    // Monitor Data
    
    // (connect to tree)
    cooker->InitCookedData();

    // Plot Waveforms, options: 
    //    'w' waveform only
    //    'f' fft only
    //    'b' waveform & fft
    char option = 'b';  
    cooker->Waveform(option);    

    // Monitor Noise
    // plot raw variables: min, max, PPV, mean
    // print noise rate @ -5 mV and -10 mV wrt mean
    // cooker->Noise();

    // Baseline investigation
    // plot: baseline, vs event, peak vs baseline
    // cooker->Baseline();
    
    // Delete outFile pointer
    cooker->End();
    
    inFile->Delete();
  }
  
  return 1;
}


bool Welcome(int argc){
  
  printf("\n      --------------------  \n");
  printf("\n      --------------------  \n");
  printf("\n            cook_raw        \n");
  printf("\n      --------------------  \n");
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
