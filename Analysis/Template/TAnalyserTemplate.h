/*

 TAnalyserTemplate 

 gary.smith@ed.ac.uk
 13 05 2020

 Purpose:
   A program to assist in developing PMT data analyses.

   Copy this folder and use the template class to create 
   your own analysis methods.
   
   This class has the same format as TCookedAnalyser
   so routines developed here can be easily migrated.

 Input 
    This class reads in data created using cook_raw
     - see $WM_COOK

 How to run:

 Option 1 
     Load program in root to be run in interpreted mode
     $ root
     [0] .L TAnalyserTemplate.C
     // Create an object 'analyser' of type TAnalyserTemplate
     // NB change filename (path) to one you have ready in this folder (elsewhere)
     [1] TAnalyserTemplate * analyser = new TAnalyserTemplate("./Run_1_PMT_29_Loc_3_Test_D.root"); 
     // Run Pulse Height method (outputs pdf)
     [2] analyser->Pulse_Height()
     // list contents of current directory in root
     [3] .ls
     // set the y axis to log scale using the global object gPad
     [4] gPad->SetLogy()
     // Plot histogram in root
     [5] hPulse_Height->Draw()
     // Limit number of entries included in analysis to 100
     [6] analyser->SetTestMode(100)
     // print sample-by-sample
     [7] analyser->Loop_Over_Samples()
     // print event-by-event
     [8] analyser->Loop_Over_Samples(1)

*/

#ifndef TAnalyserTemplate_h
#define TAnalyserTemplate_h

#include <TROOT.h>

#include <TTree.h>
#include <TFile.h>

#include <TStyle.h>
#include <TCanvas.h>
#include <TH2.h>
#include <TLegend.h>

#include <vector>
#include <limits.h>

using namespace std;

class TAnalyserTemplate {
 public :

  // Most of the data members are public.
  // Really they should be private with
  // with public Getter methods implemented
  // for accessing them.

  TFile * inFile = nullptr;

  //-------------------------
  // META DATA
  // File specific constants that
  // were added at the cooking stage
  
  TTree * metaTree;
  
  short  SampFreq;
  short  NSamples;
  short  NADCBins;
  short  Range_V;
  float  nsPerSamp;
  float  mVPerBin;
  float  Length_ns;
  short  FirstMaskBin;
  float  AmpGain;
  char   FileID[128]; 
  
  int    Run;
  int    PMT;
  int    Loc;
  char   Test;
  int    HVStep;

  TBranch * b_SampFreq     = 0;
  TBranch * b_NSamples     = 0;
  TBranch * b_NADCBins     = 0;
  TBranch * b_Range_V      = 0;
  TBranch * b_nsPerSamp    = 0;
  TBranch * b_mVPerBin     = 0;
  TBranch * b_Length_ns    = 0;
  TBranch * b_AmpGain      = 0;
  TBranch * b_FirstMaskBin = 0;
  TBranch * b_FileID       = 0;
  TBranch * b_Run          = 0;
  TBranch * b_PMT          = 0;
  TBranch * b_Loc          = 0;
  TBranch * b_Test         = 0;
  TBranch * b_HVStep       = 0;
  
  // END OF META DATA
  //-------------------------
  

  //--------------------
  // PMT DATA

  // This TTree is an ntuple containing
  // the digitiser data.
  TTree * cookedTree;
  
  // Raw ADC data  
  // NB ADC pulses are set to positive
  // polarity at the (previous) cooking stage
  // Vector elements are the samples
  // - see Loop_Over_Samples()
  vector <short> * ADC = 0;   
  
  // 'Cooked' variables
  //  (event level)
  float peak_mV;
  short peak_samp;
  float min_mV;
  float mean_mV;
  float start_s;
  float base_mV;

  // for connecting to the tree
  TBranch * b_ADC = 0;
  TBranch * b_peak_mV  = 0;  
  TBranch * b_peak_samp = 0;  
  TBranch * b_min_mV  = 0;  
  TBranch * b_mean_mV = 0;  
  TBranch * b_start_s = 0;  
  TBranch * b_base_mV = 0;  
  
  // END PMT DATA
  //--------------------
  
  TAnalyserTemplate(string path = "./Run_1_PMT_29_Loc_3_Test_D.root");
  ~TAnalyserTemplate();
  int  GetEntry(int entry);
  void Init();

  void  InitMeta();
  void  InitCooked();
   
  void InitCanvas(float w = 1000.,
		  float h = 800.);
  void DeleteCanvas();
  
  string GetFileID();
  
  string GetCookedTreeID();
  string GetMetaTreeID();
  
  void  PrintMetaData();
  
  // limit entries for faster testing
  void  SetTestMode(int);
  
  void  SetTest(char Test);
  char  GetTest();

  void  SetRun(int Run);
  int   GetRun();
  
  float ADC_To_Wave(short ADC);
  float Wave_To_Amp_Scaled_Wave(float wave);

  //----
  void  Loop_Over_Samples(int verbosity);
  // Dark Counts

  TH1F * hPulse_Height = nullptr;
  
  void  Pulse_Height(float thresh_mV = 10.);
  void  Init_Pulse_Height();
  void  Save_Pulse_Height(string outFolder = "./Plots/");
  
 private:
  
   // only accommodating int size here
   Long64_t nentries64_t; // dummy
   int      nentries;

   TCanvas * canvas = nullptr;
   
   void  Set_THF_Params(float *,float *,float *, int *);
   
   void  SetStyle();
   
};

#endif

#ifdef TAnalyserTemplate_cxx
TAnalyserTemplate::TAnalyserTemplate(string path) 
{

  inFile = new TFile(path.c_str(),"READ");
  
  if ( !inFile || !inFile->IsOpen()) {
    fprintf(stderr,"\n Error, Check File: %s \n",path.c_str());
    return;
  }

  Init();
}

TAnalyserTemplate::~TAnalyserTemplate()
{ 
  delete cookedTree;
  delete metaTree;
  delete inFile;
}

int TAnalyserTemplate::GetEntry(int entry)
{
// Read contents of entry.
   if (!cookedTree) return 0;
   return cookedTree->GetEntry(entry);
}

void TAnalyserTemplate::SetTestMode(int user_nentries = 1000000){

  nentries = (int)nentries64_t;
  if(user_nentries == 0 || user_nentries > nentries){
    printf("  nentries set to TTree value \n");
    return;
  }
  else{
    nentries = user_nentries;  
    printf("\n Warning: \n ");
    printf("  nentries set to %d for testing \n",nentries);
  }
}

void TAnalyserTemplate::Init()
{

  InitMeta();
  InitCooked();
  
  SetStyle();  
  InitCanvas();
  
}

void TAnalyserTemplate::InitMeta(){
  
  printf("\n ------------------------------ \n");
  printf("\n Initialising Meta Data \n");
  printf("\n   %s \n",GetMetaTreeID().c_str());

  inFile->GetObject(GetMetaTreeID().c_str(),metaTree);
  
  if (!metaTree){
    fprintf( stderr, "\n Error: no meta tree  \n ");
    fprintf( stderr, "\n Was this file created with the latest cook_raw ? \n ");
    return;
  }

  metaTree->SetBranchAddress("SampFreq",&SampFreq,&b_SampFreq);
  metaTree->SetBranchAddress("NSamples",&NSamples,&b_NSamples);
  metaTree->SetBranchAddress("NADCBins",&NADCBins,&b_NADCBins);
  metaTree->SetBranchAddress("Range_V",&Range_V,&b_Range_V);
  metaTree->SetBranchAddress("nsPerSamp",&nsPerSamp,&b_nsPerSamp);
  metaTree->SetBranchAddress("mVPerBin",&mVPerBin,&b_mVPerBin);
  metaTree->SetBranchAddress("Length_ns",&Length_ns,&b_Length_ns);
  metaTree->SetBranchAddress("AmpGain",&AmpGain,&b_AmpGain);
  metaTree->SetBranchAddress("FirstMaskBin",&FirstMaskBin,&b_FirstMaskBin);
  metaTree->SetBranchAddress("FileID",&FileID,&b_FileID);
  
  metaTree->SetBranchAddress("Run",&Run,&b_Run);
  metaTree->SetBranchAddress("PMT",&PMT,&b_PMT);
  metaTree->SetBranchAddress("Loc",&Loc,&b_Loc);
  metaTree->SetBranchAddress("Test",&Test,&b_Test);
  metaTree->SetBranchAddress("HVStep",&HVStep,&b_HVStep);

  metaTree->GetEntry(0);
  
  printf("\n ------------------------------ \n");
  printf("\n FileID = %s ",FileID);
  printf("\n Run    = %d ",Run);
  printf("\n PMT    = %d ",PMT);
  printf("\n Loc    = %d ",Loc);
  printf("\n Test   = %c ",Test);
  printf("\n HVStep = %d \n",HVStep);
  
  printf("\n ------------------------------ \n");

}

void TAnalyserTemplate::InitCooked(){
  
  inFile->GetObject(GetCookedTreeID().c_str(),cookedTree);
  
  if (cookedTree == 0){
    fprintf( stderr, "\n Warning: No cooked data tree");
  }
  
  printf("\n ------------------------------ \n");
  printf("\n Initialising Cooked Data \n");
  printf("\n   %s \n",GetCookedTreeID().c_str());
  
  if (!cookedTree){
    fprintf( stderr, "\n Error: no cooked tree  \n ");
    return;
  }

  cookedTree->SetMakeClass(1);
  
  cookedTree->SetBranchAddress("ADC",&ADC, &b_ADC);
  cookedTree->SetBranchAddress("peak_mV",&peak_mV, &b_peak_mV);
  cookedTree->SetBranchAddress("peak_samp",&peak_samp, &b_peak_samp);
  cookedTree->SetBranchAddress("min_mV",&min_mV, &b_min_mV);
  cookedTree->SetBranchAddress("mean_mV",&mean_mV, &b_mean_mV);
  cookedTree->SetBranchAddress("start_s",&start_s, &b_start_s);
  cookedTree->SetBranchAddress("base_mV",&base_mV, &b_base_mV);
  
  nentries64_t = cookedTree->GetEntriesFast();
  
  if( nentries64_t > INT_MAX ){
      fprintf(stderr,
	      "\n Error, nentries = (%lld) > INT_MAX unsupported \n ",
	      nentries64_t);
      return;
  }
  else
    nentries = (int)nentries64_t;
  
  
  printf("\n ------------------------------ \n");
  
  return;
}

void TAnalyserTemplate::SetTest(char userTest ){
  Test = userTest;
}

char TAnalyserTemplate::GetTest(){
  return Test;
}

void TAnalyserTemplate::SetRun(int userRun ){
  Run = userRun;
}

int TAnalyserTemplate::GetRun(){
  return Run;
}

#endif // #ifdef TAnalyserTemplate_cxx
