/***************************************************
 * A program to process wavedump binary output files
 * 
 * VME version
 *
 * Author 
 *  gary.smith@ed.ac.uk
 *  26 07 19
 *
 * Purpose
 *  This C program reads a CAEN wavedump 
 *  binary output binary file and writes 
 *  a root file containing a TTree 
 *
 * How to build
 *  $ make DatToRoot
 *
 * How to run
 *  Option 1: 
 *  $ ./Dat2Root inputFile
 * 
 * Input - 
 *  binary file written by CAEN's 
 *  wavedump software
 * 
 * Output - a root file containing
 *  HEAD[6]  6 * 32 bits = 24  bytes 
 *  ADC[N]  N * 16 bits = 16N bytes (N = No. samples) 
 * 
 * Dependencies
 *  The cern developed root framework
 *  Makefile (included) uses g++ compiler
 *
 */ 

#include <cstdlib>
#include <fstream>
#include <iostream>

#include "TFile.h"
#include "TTree.h"

#include "TROOT.h"

using namespace std;

int main(int argc, char **argv){
  
  printf("\n DatToRoot \n" );
  
  ifstream inFile(argv[1]);
  
  if(!inFile.good()){
    printf("\n Error: check filename\n");
    return -1;
  }
  
  string outName = argv[1];
  string outPath = argv[1];
  outPath += ".root";
  
  printf("\n %s \n",outName.c_str());
  
  TFile * outFile = new TFile(outPath.c_str(),
			      "RECREATE",
			      outName.c_str());
   
  TTree * outTree = new TTree(outName.c_str(),
			      outName.c_str());

  int HEAD[6];
  int NS = 0;
  int ID = -1; // Board ID
  int PN = 0; // Pattern (VME)
  int CL = 0; // Channel
  int EC = 0; // Event Counter
  int TT = 0; // Trigger Time Tag
  
  outTree->Branch("HEAD[6]",HEAD,"HEAD[6]/I");
  
  inFile.seekg(0, ios::beg);
  for (int i = 0 ; i < 6 ; i++ ) 
    inFile.read((char*)&HEAD[i],sizeof(int));
  
  // HEAD[0] is event size in bytes 
  // (header plus samples)
  NS = (HEAD[0] - 24)/2;
  printf("\n %d Samples \n",NS);

  short ADC[NS];
  char name[50],type[50];
  sprintf(name,"ADC[%d]",NS);
  sprintf(type,"ADC[%d]/S",NS);
  outTree->Branch(name,ADC,type);

  inFile.seekg(0, ios::beg);
  while ( inFile.is_open() && 
 	  inFile.good()    && 
 	  !inFile.eof()      ){
    
    //------------------
    // header is six lots 32 bits    
    for (int i = 0 ; i < 6 ; i++ )
      inFile.read((char*)&HEAD[i],sizeof(int)); 

    if(ID==-1)
      for (int i = 0 ; i < 6 ; i++ ) 
	printf("\n HEAD[%d] %d \n",i,HEAD[i]);
    
    // HEAD[0] is event size
    // (header plus samples)
    NS = (HEAD[0] - 24)/2; 
    
    //------------------
    // waveform is N lots of 16 bits    
    for (short i = 0; i < NS ; i++)
      inFile.read((char*)&ADC[i],sizeof(short)); 
    
    ID = HEAD[1];
    PN = HEAD[2];
    CL = HEAD[3];
    EC = HEAD[4];
    TT = HEAD[5];
    
    outTree->Fill();
    
    if( EC%100000 == 0 )
      printf("\n Event %d \n", EC);
    
  } // end: while loop
  
  outTree->Write();
  outTree->Delete();
  
  outFile->Write();
  outFile->Close();

  inFile.close();	
  
  return 1;
}
