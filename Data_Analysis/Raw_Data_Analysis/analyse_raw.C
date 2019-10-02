/*****************************************************
 * A program to process raw root files from wavedump
 *
 * Author 
 *  gary.smith@ed.ac.uk
 *  27 09 2019
 *
 * Purpose
 *  This program reads in a TTree
 *  of cooked variables
 *
 * How to build
 *  $ make 
 *
 * How to run
 *
 * $ ./analyse_raw 
 * 
 * Input
 * 
 * Output
 * 
 *
 * Dependencies
 *  root.cern - a working version of root is required
 *
 *  wmStyle.C - TStyle class for WATCHMAN plotting style
 *   This file is included in the distribution and
 *   can be found by the system by adding the following 
 *   lines to ~/.bashrc:  
 *    export TESTING_DIR=/path/to/Wavedump_Wrapper
 *    export CPATH=$TESTING_DIR/Data_Analysis/Common_Tools/
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

int main(int argc, char * argv[]){
  
  if( !Welcome(argc) )
    return -1;
     
  TTree * tree    = nullptr;
  
  // object used to cook 
  // raw (root) data
  TCooker * cooker = new TCooker(tree);
  
  // reduce event loop for
  // faster code testing
  int user_nentries = 100000;
  cooker->SetTestMode(user_nentries);
  
  //-------------------
  // Analyse Data
  
  // re-connect to file
  // and tree
  cooker->InitCookedData();
  
  // Plot Waveforms, options: 
  //    'w' waveform only
  //    'f' fft only
  //    'b' waveform & fft
//   char option = 'b';  
//   cooker->Waveform(option);    
  
  // Monitor Noise
  // plot raw variables: min, max, PPV, mean
  // print noise rate @ -5 mV and -10 mV wrt mean
  cooker->Noise();
  
  // Baseline investigation
  // plot: baseline, vs event, peak vs baseline
  cooker->Baseline();
  
  // Delete outFile pointer
  cooker->CloseCookedFile();
  
  return 1;
}


bool Welcome(int argc){
  
  printf("\n      --------------------    ");
  printf("\n      --------------------  \n");
  printf("\n            cook_raw        \n");
  printf("\n      --------------------    ");
  printf("\n      --------------------  \n");

  printf("\n ------------------------------ \n");
}
