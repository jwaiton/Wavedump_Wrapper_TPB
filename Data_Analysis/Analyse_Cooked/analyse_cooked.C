/*****************************************************
 * A program to process raw root files produced  
 * using TCooker
 *
 * Author 
 *  gary.smith@ed.ac.uk
 *  15 10 2019
 *
 * Purpose
 *  This program reads in a TTree and performs
 *  an analysis.
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
 * $ analyse_cooked /optional/path/to/Run_1_PMT_130_Loc_0_Test_D.root
 * 
 * Input
 *  .root file created with TCooker
 * 
 * Output
 *   Plots are saved in directory structure
 *    e.g.
 *     ./Plots/Dark 
 *   which must be created prior to running.
 *   ( see make_plot_directories.sh)
 * 
 * Dependencies
 *  root.cern - a working version of root is required
 *
 *  FileNameParser
 *  TCookedAnalyser
 */ 
#include <iostream>

#include <string>
#include "TSystem.h"
#include "TCookedAnalyser.h"

#include "TF1.h"

bool Welcome(int argc);

int main(int argc, char * argv[]){
  
  if( !Welcome(argc) )
    return -1;
      
  TCookedAnalyser * cooked_analyser = nullptr;

  string path;
  
  for( int iFile = 1 ; iFile < argc ; iFile++){

    //-------------------
    //-------------------
    // Setting Up

    path = argv[iFile];
    
    printf("\n ------------------------------ \n");
    printf("\n  Input File:       ");
    printf("\n    %s  \n",path.c_str());    

    cooked_analyser = new TCookedAnalyser(path);

    // Optional method:
    // reduce event loop for faster code testing
    // NB no check that this is lower that nentries

    //int user_nentries = 10; 
    //cooked_analyser->SetTestMode(user_nentries);
    
    //-------------------
    //-------------------
    // Monitoring
    
    cooked_analyser->PrintMetaData();

    gSystem->Exec("mkdir -p ./Plots/Noise");
    cooked_analyser->Noise();
    
    //-------------------
    //-------------------
    // Analysis 
    
    char  test = cooked_analyser->GetTest();
    
    switch(test){
    case('D'):
      gSystem->Exec("mkdir -p ./Plots/Dark");
      cooked_analyser->Dark();
      break;
    default:
      //-------------
      // Timing
      gSystem->Exec("mkdir -p ./Plots/Timing");
      
      // Mean LED pulse peak time
      cooked_analyser->Fit_Peak_Time_Dist();
      
      //-------------
      // Charge
      gSystem->Exec("mkdir -p ./Plots/Charge");
      
      // Save a new root file with charge hist
      cooked_analyser->Make_hQ_Fixed();

      //-------------
      // Pulse fitting test
      gSystem->Exec("mkdir -p ./Plots/PulseFit");
      
      // towards rise and fall time extraction
      //TF1 * fWave = cooked_analyser->Fit_Pulse();
      
    }
    
  }// end of: for( int iFile = 1 ;

  return 1;
}


bool Welcome(int argc){
  
  printf("\n      --------------------    ");
  printf("\n      --------------------  \n");
  printf("\n         analyse_cooked     \n");
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
    printf("\n  ./analyse_cooked /path/to/Run_1_PMT_130_Loc_0_Test_D.root \n\n");
    printf("\n ------------------------------ \n");
    return false;
  }
  
  
}

