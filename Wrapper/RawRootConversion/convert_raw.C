/*****************************************************
 * A program to process raw root files from wavedump
 *
 * Author 
 *  gary.smith@ed.ac.uk
 *  29 07 2019
 *
 * Purpose
 *  This program reads a TTree
 *
 * How to build
 *  $ make 
 *
 * How to run
 *
 * $ ./convert_raw wave_0.dat.root
 *
 * Dependencies
 *  root.cern
 *
 * Known issues
 *   Under linux you will likely be required to add 
 *   the shared (.so) file location to your library path 
 *   ie add the path to this directory 
 *
 */ 
#include <iostream>

#include <string>
#include "TFile.h"
#include "TTree.h"
#include "TConvert.h"

bool IsValidFile(TFile *, char *);
  
int main(int argc, char * argv[]){
  
  printf("\n --------------------  \n");
  printf("\n    convert_raw        \n");
  printf("\n --------------------  \n");
  TFile * inFile = nullptr;
  TTree * tree = nullptr;
  TConvert * t = nullptr;
  
  for( int iFile = 1 ; iFile < argc ; iFile++){
    
    inFile = new TFile(argv[iFile],"READ");
    
    if( !IsValidFile(inFile,argv[iFile]) )
      continue;

    inFile->GetObject("T",tree); 

    t = new TConvert(tree);
    
    // SET OUTPUT: conversion options, plots
    
    // Loop over events
    t->Loop();
    
    //tree->Delete();
    //inFile->Close();
    //inFile->Delete();
  }
  
  return 1;
}

bool IsValidFile(TFile * inFile, char * arg){
  
  if ( !inFile || !inFile->IsOpen()) {
    fprintf(stderr,"\n Error, Check File: %s \n",arg);
    return false;
  }
  else 
    return true;
}
