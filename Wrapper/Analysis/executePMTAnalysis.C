/***************************************************
 * A program to process raw root files from wavedump
 *
 * Author 
 *  gary.smith@ed.ac.uk
 *  01 01 2019
 *
 * Purpose
 *  This program reads a TTree
 *
 * How to build
 *  $ make 
 *
 * How to run
 *  $ ./executePMTAnalysis  
 *
 * Dependencies
 *  root.cern
 *
 *
 */ 
#include<iostream>

#include <string>
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "PMTAnalyser.h"
#include "ShippingData.h"
#include "fileNameParser.h"

using namespace std;

int main(Int_t argc, Char_t *argv[]){
  
  TFile * file = nullptr;
  TTree * tree = nullptr;
  
  PMTAnalyser * analysePMT = nullptr;

  Float_t thresh_mV  = 10.0;
  Int_t   darkRate   = 8000.;

  ShippingData * shipData = nullptr;
  
  // Loop over argv[] which should be a path (paths)
  // to a file (list of files)
  for( int iFile = 1 ; iFile < argc ; iFile++){
     
     file = new TFile(argv[iFile],"READ");
     
     if (!file || !file->IsOpen()) {
       file = new TFile();
       cerr << " Error, Check File: " << argv[iFile] << endl; 
       return -1;
     }

     //-------
     // use parser to extract test ID
     cout << endl;
     cout << " Run      " << run(argv[iFile])      << endl;
     cout << " PMT      " << pmtID(argv[iFile])    << endl;
     cout << " Location " << location(argv[iFile]) << endl;
     
     cout << endl;
     shipData = new ShippingData(pmtID(argv[iFile]));

     // connect to tree in input file
     file->GetObject((TString)GetTreeName(argv[iFile]),tree); 
     
     // initalise analysis object using tree 
     analysePMT = new PMTAnalyser(tree);
     
     // Double Chooz dark rate threshold (x10 to include amplification)
     darkRate   = analysePMT->DarkRate(thresh_mV);
     
     cout << " Hamamatsu Dark Rate = " << shipData->GetDR() << endl;
     cout << " PMT Test  Dark Rate = " << darkRate          << endl;
     
  }
  
  //TFile * resultsFile = new TFile("Results","RECREATE");
  
  return 1;
}
