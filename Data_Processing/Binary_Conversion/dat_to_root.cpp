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
 *  $ make dat_to_root
 * 
 * or
 * 
 * make -f ./Build_Options/Makefile_clang++ dat_to_root
 * 
 * How to run
 *  Option 1: 
 *  $ ./dat_to_root wave_0.dat
 * 
 * Input - 
 *  binary file written by CAEN's 
 *  wavedump software
 * 
 * Output - a root file (e.g. wave_0.dat.root) containing
 *  unsigned int HEAD[6]  6 * 32 bits = 24  bytes 
 *  std::vector<short> ADC(N)  N * 16 bits = 16N bytes (N = No. samples) 
 * 
 * Dependencies
 *  The cern developed root framework
 *  Makefile (included) which uses g++ compiler
 *
 */ 

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

#include "TFile.h"
#include "TTree.h"

#include "TROOT.h"

using namespace std;

int main(int argc, char **argv){
  
  // 0 - no printing
  // 1 - standard printing
  // 2 - print HEAD and ADC values for first entry
  int verbosity = 1;
  
  if( verbosity > 0 ){
    printf("\n ---------------------------------- \n" );
    
    printf("\n           dat_to_root              \n" );
    
    printf("\n ---------------------------------- \n" );
    
    printf("\n wavedump binary to root conversion \n" );
    printf("          ( VME version )             \n" );
  }
  
  ifstream inFile(argv[1]);
  
  if(!inFile.good()){
    fprintf( stderr, "\n Error: check filename \n ");    
    return -1;
  }
  
  string inName  = argv[1];
  string outName = argv[1];
  outName += ".root";
  
  if( verbosity > 0 ){
    printf("\n ---------------------------------- \n" );
    printf("\n input file:  %s      \n",inName.c_str());
    
    printf("\n output file: %s     \n",outName.c_str());
    printf("\n ---------------------------------- \n" );
  }
  
  TFile * outFile = new TFile(outName.c_str(),
			      "RECREATE",
			      inName.c_str());
   
  TTree * outTree = new TTree("T","T");

  unsigned int HEAD[6];
  unsigned int NS = 0; 
  unsigned int ID = 0; // Board ID
  unsigned int PN = 0; // Pattern (VME)
  unsigned int CL = 0; // Channel
  unsigned int EC = 0; // Event Counter
  //unsigned int TT = 0; // Trigger Time Tag
  
  short buffer   = 0;

  int nEntries   = 0;
  int firstEntry = 0;
  int lastEntry  = -1;
  
  outTree->Branch("HEAD",HEAD,"HEAD[6]/i");
  
  inFile.seekg(0, ios::beg);
  for (int i = 0 ; i < 6 ; i++ ) 
    inFile.read((char*)&HEAD[i],sizeof(int));
  
  // HEAD[0] is event size in bytes 
  // (header plus samples)
  NS = (HEAD[0] - 24)/2;
  
  if( verbosity > 1)
    for (int i = 0 ; i < 6 ; i++ )
      printf("\n HEAD[%d] %u \n",i,HEAD[i]);
  
  std::vector<short> ADC;
  
  outTree->Branch("ADC",&ADC);
  
  inFile.seekg(0, ios::beg);
  while ( inFile.is_open() && 
 	  inFile.good()    && 
 	  !inFile.eof()      ){
    
    //------------------
    // header is six lots 32 bits    
    for (int i = 0 ; i < 6 ; i++ )
      inFile.read((char*)&HEAD[i],sizeof(int)); 
      
    // HEAD[0] is event size in bytes
    // (header plus samples)
    if( ( (HEAD[0] - 24)/2 ) != NS )
      fprintf( stderr, "\n Error: Number of Samples has changed \n ");    

    ADC.clear();

    //------------------
    // waveform is N lots of 16 bits    
    for (int i = 0; i < (int)NS ; i++){
      inFile.read((char*)&buffer,sizeof(short));     
      ADC.push_back(buffer);
    }
    
    ID = HEAD[1]; // Board ID
    PN = HEAD[2]; // Pattern (VME)
    CL = HEAD[3]; // Channel
    EC = HEAD[4]; // Event Counter
    //TT = HEAD[5]; // Trigger Time Tag
	
    if( nEntries==0 ){
      firstEntry = EC;

      if( verbosity > 0 ){
	printf("\n  Board ID      %u \n", ID);
	printf("\n  Pattern       %u \n", PN);
	printf("\n  Channel       %u \n", CL);
	printf("\n  %u Samples per waveform           \n",NS);
	printf("\n ---------------------------------- \n" );
	
	printf("\n  First Entry   %d \n", firstEntry);
      }
      
      if( verbosity > 1 )
	for (int i = 0 ; i < (int)NS ; i++)
	  printf("\n ADC[%d] = %d \n",i,ADC.at(i));
    }
    else if ( (NS <= 1000  && EC%500000 == 0) ||
	      (NS >  1000  && EC%50000  == 0) ){
      printf("\n  Entry         %d \n", EC);
    }

    // skip last iteration as it  
    // takes previous event values
    if( (int)EC == lastEntry){
      break;
    }
    
    lastEntry = EC;
    
    nEntries++;
  
    outTree->Fill();
    
  } // end: while loop
  
  printf("\n  Last Entry    %d \n", lastEntry);
  printf("\n  Total Entries %d \n", nEntries);
  printf("\n ---------------------------------- \n" );
  
  outTree->Write();
  outTree->Delete();
  
  outFile->Write();
  outFile->Close();
  
  inFile.close();	
  
  return 1;
}
