/*****************************************************
 * A program to plot waveforms from files made 
 * using TCooker
 *
 * Author 
 *  gary.smith@ed.ac.uk
 *  31 10 2019
 *
 * Purpose
 *  This program reads in a TTree
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
 * $ ./waveform_plotter /path/to/Run_1_PMT_130_Loc_0_Test_D.root
 * 
 * Input
 *  .root file created with TCooker
 * 
 * Output
 *   Plots can be saved in directory structure
 *     ./Plots/Waveforms 
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

bool Welcome(int argc);

int main(int argc, char * argv[]){
  
  if( !Welcome(argc) )
    return -1;
     
  TCookedAnalyser * wave_plotter = nullptr;

  string path;
  
  //-------------------
  //-------------------
  // Setting Up
  
  path = argv[1];
  
  printf("\n ------------------------------ \n");
  printf("\n  Input File:       ");
  printf("\n    %s  \n",path.c_str());    
  
  // initalise object from class with Waveform() method 
  wave_plotter = new TCookedAnalyser(path);
  
  //-------------------
  // Meta Data 
  
  wave_plotter->PrintMetaData();

  // prepare default plots
  gSystem->Exec("mkdir -p ./Plots/Waveforms/");
  string outPath = "./Plots/Waveforms/";

  wave_plotter->Waveform('d');

  // ----
  // Waveform plotting
  char aChar = 'b';
  
  printf("\n Plot waveform? ");
  printf("\n  options:  ");
  printf("\n  'n' - No ");
  printf("\n  'w' - Waveform ");
  printf("\n  'f' - FFT ");
  printf("\n  'b' - 'w' and 'f' \n");
  scanf("%c",&aChar);
  

  switch(aChar){
  case('w'):
    outPath += "hWave.pdf";
    break;
  case('f'):
    outPath += "hFFT.pdf";
    break;
  case('b'):
    outPath += "hWaveFFT.pdf";
    break;
  default:
    break;
  }

  string sysCommand = "";
  
#ifdef  __linux__  
  sysCommand = "evince ";
#else
  sysCommand = "open ";
#endif   

  sysCommand += outPath;
  sysCommand += " &";
  system(sysCommand.c_str());

  if(aChar!='N' && aChar!='n'){
    wave_plotter->Waveform(aChar);
  }
  
  return 1;
}


bool Welcome(int argc){
  
  printf("\n      --------------------    ");
  printf("\n      --------------------  \n");
  printf("\n        waveform_plotter    \n");
  printf("\n      --------------------    ");
  printf("\n      --------------------  \n");

  printf("\n ------------------------------ \n");
  
  if(argc == 2){
    printf("\n  Processing single file \n");
    printf("\n ------------------------------ \n");
    return true;
  }
  else{
    printf("\n  enter single file as argument \n");
    printf("\n  e.g. \n");
    printf("\n  ./waveform_plotter Run_1_PMT_130_Loc_0_Test_D.root \n\n");
    printf("\n ------------------------------ \n");
    return false;
  }
  
  
}

