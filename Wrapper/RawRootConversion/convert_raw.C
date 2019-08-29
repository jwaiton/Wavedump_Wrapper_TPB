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


bool Welcome(int argc);
bool IsFileReady(TFile *, char *);

int main(int argc, char * argv[]){
  
  if( !Welcome(argc) )
    return -1;
     
  TFile * inFile = nullptr;
  TTree * tree = nullptr;
  
  TConvert * convert = nullptr;
  
  for( int iFile = 1 ; iFile < argc ; iFile++){
    
    inFile = new TFile(argv[iFile],"READ");

    if( !IsFileReady(inFile,argv[iFile]) )
      continue;
    
    printf("\n  Input File:       \n");
    printf("\n    %s  \n",argv[iFile]);
    
    inFile->GetObject("T",tree); 
    
    convert = new TConvert(tree);
    
    convert->GetDAQInfo();

    // Noise    
    
    // Create Converted ata
    
    // Loop 
    //convert->ProcessEntries();
    
    //tree->Delete();
    //inFile->Close();
    //inFile->Delete();
  }
  
  return 1;
}

bool Welcome(int argc){
  
  printf("\n      --------------------  \n");
  printf("\n          convert_raw       \n");
  printf("\n      --------------------  \n");

  if(argc == 2){
    printf("\n  processing single file \n");
    return true;
  }
  else if  (argc > 2){
    printf("\n  processing %d files \n",argc-1);
    return true;
  }
  else{
    printf("\n  enter file as argument \n");
    printf("\n  e.g. \n");
    printf("\n  ./convert_raw ./wave_0.dat.root \n");
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
