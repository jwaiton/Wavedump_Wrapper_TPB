/*****************************************************
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
 *
 *  Example One - analyse one raw root file
 *  $ ./executePMTAnalysis ../../RawRootData/Run_11_PMT_90_Loc_0_Test_D.root  
 *
 *  Example Two - analyse two raw root files (two HV steps)
 *  $ ./executePMTAnalysis ../../RawRootData/Run_11_PMT_90_Loc_0_HV_1.root ../../RawRootData/Run_11_PMT_90_Loc_0_HV_2.root 
 *
 *  Example Three - analyse multiple raw root files ( all run numbers )
 *  $ ./executePMTAnalysis ../../RawRootData/Run_*_PMT_90_Loc_0_Test_D.root  
 *
 * Dependencies
 *  root.cern
 *  PMTAnalyser
 *  DataInfo
 *  ShippingData
 *
 * Known issues
 *   Under linux you will likely be required to add 
 *   the shared (.so) file to your library path 
 *
 */ 

#include <iostream>

#include <string>
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TString.h"
#include "PMTAnalyser.h"
#include "ShippingData.h"
#include "FileNameParser.h"
    
using namespace std;

int main(Int_t argc, Char_t *argv[]){

  TFile * outFile = nullptr;  
  TFile * inFile = nullptr;
  
  TTree          * tree = nullptr;
  PMTAnalyser    * PMT = nullptr;
  ShippingData   * shipData = nullptr;
  FileNameParser * testInfo = new FileNameParser();
  
  // 'V' for VME, 'D' for desktop
  Char_t  digitiser = 'D';
  // Old style BinToRoot output
  // or new BinToRoot output?
  // (pulse[] -> waveform[] e.g.)
  Bool_t oldRootFileVersion = kFALSE;
  
  // Dark Rate
  Float_t thresh_mV  = 10.0;
  Int_t   darkRate   = 8000.;
  
  if(argc==1){
    cerr << " Error, argument needed " << endl; 
    return 2;
  }

  outFile = new TFile("./output.root","RECREATE");

  // Testing reading from root file, writing to new file
  TH1F  * hQ   = nullptr;

  // argv should be a path to a file
  // or list of files ( wildcards work )
  for( int iFile = 1 ; iFile < argc ; iFile++){
     
     inFile = new TFile(argv[iFile],"READ");
     
     if (!inFile || !inFile->IsOpen()) {
       inFile = new TFile();
       cerr << " Error, Check File: " << argv[iFile] << endl; 
       return -1;
     }
     
     // connect to tree in input file
     TString treeName = (TString)testInfo->GetTreeName(argv[iFile]);
     inFile->GetObject(treeName,tree); 
     
     
     // initalise analysis object using tree 
     PMT = new PMTAnalyser(tree,
			   digitiser,
			   oldRootFileVersion);
     
     // Set plot attributes to bespoke TStyle 
     PMT->SetStyle();
     
     // Limit to subset of entries for quicker testing
     PMT->SetTestMode(kTRUE);
     
     // Towards saving analysis output 
     PMT->MakeCalibratedTree();

     shipData = new ShippingData(testInfo->pmtID(argv[iFile]));
     
     int event = 0;
     
     PMT->PlotAccumulatedFFT();

     while ( event!= -1 ){
       cout << endl;
       cout << " Which waveform to plot?" << endl;
       cout << " enter event number " << endl;
       cout << " (-1 to quit) " << endl;

       cin >> event;
       if(event > -1)
	 PMT->PlotWaveform(event);
     }
     //------------
     // Timing Study
     //PMT->TimeOfPeak();

     //------------
     //  Dark Rate
     Bool_t investigateDarkRate = kTRUE;
     if( investigateDarkRate && 
	 testInfo->test(argv[iFile])=='D'){
       
       if(oldRootFileVersion){
	 cout << " Dark Rate method only applicable to new BinToRoot files " << endl;
       }
       else{
	 darkRate = PMT->DarkRate(thresh_mV);
	 cout << " PMT Test  Dark Rate = " << darkRate          << endl;
       }
     }
     
     //------------
     //  FFT investigation
     Bool_t investigateFFT = kFALSE;
     // Make Filtered Histograms
     if(investigateFFT){ 
       TCanvas * canvas = PMT->Make_FFT_Canvas();
       
       TString canvasName;
       canvasName.Form("./Plots/FFT_Run_%d_PMT_%d_Test_%c.pdf",
		       testInfo->run(argv[iFile]),
		       testInfo->pmtID(argv[iFile]),
		       testInfo->test(argv[iFile]));
       
       if(canvas){
	 canvas->SaveAs(canvasName);
	 
	 canvasName.Form("./Plots/FFT_Run_%d_PMT_%d_Test_%c.root",
			 testInfo->run(argv[iFile]),
			 testInfo->pmtID(argv[iFile]),
			 testInfo->test(argv[iFile]));
	 
	 canvas->SaveAs(canvasName);
       }
       else{
	 cout << endl;
	 cout << " No canvas produced " << endl;
       }
     }
     
     inFile->Delete();

     TString hName = testInfo->Get_hQ_Fixed_Name(argv[iFile]); 
     hQ = (TH1F*)inFile->Get(hName);
     
     // Writing to file
     // put file in same directory as objects
     outFile->cd();
     
     // Write specific histogram
     hQ->Write();
     
     // hQ->SetDirectory(outFile);
     
     // Write objects in directory 
     // outFile->Write();
     
     outFile->Close();
     
  }

  
  return 1;
}
