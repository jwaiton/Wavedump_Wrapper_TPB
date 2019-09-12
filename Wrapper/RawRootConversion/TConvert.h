#ifndef TConvert_h
#define TConvert_h

#include <TROOT.h>
#include <TTree.h>
#include <TFile.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>

#include <vector>
#include <limits.h>

using namespace std;

class TConvert {
public :

  // Input variables
   TTree *rawTree;
   int   treeNumber;

   uint HEAD[6];
   
   vector<short> * ADC  = 0;

   TBranch * b_HEAD = 0;  
   TBranch * b_ADC  = 0;   
   
   TFile * outFile;

   // Intermediate processing variables 
   // not written 
   
   TTree * cookedTree;
   
   float min_cook_mV;
   float max_cook_mV;
   float ppV_cook_mV;
   float mean_cook_mV;

   TBranch * b_min_cook_mV = 0;  
   TBranch * b_max_cook_mV = 0;  
   TBranch * b_ppV_cook_mV = 0;  
   TBranch * b_mean_cook_mV = 0;  

   // Output (to save)
   TTree * outTree;

   // event-by-event calibrated 
   // variables to write

   float min_mV;
   float peak_mV;
   float base_mV;
   
   float peak_time_ns;
   short peak_samp;
   
   float event_time_ns;
   
   vector<float> * wave = 0;
   
   TBranch * b_min_mV = 0;  
   TBranch * b_peak_mV = 0;  
   TBranch * b_base_mV = 0;  
   TBranch * b_peak_time_ns = 0;  
   TBranch * b_peak_samp = 0;  
   TBranch * b_event_time_ns = 0;  
   
   TConvert(TTree *tree=0,
	    char digitiser='V', // Program default is VME 1730
	    char sampSet='3',   // variable only used for digitiser='D'
	    char pulsePol='N'); // 'N' Neg or 'P' Pos
   virtual ~TConvert();
   virtual int  GetEntry(int entry);
   virtual int  LoadTree(int entry);
   virtual bool Init(TTree *tree);
   virtual void Show(int entry = -1);
   
   void InitCanvas(float w = 1000.,
		   float h = 800.);
   void DeleteCanvas();

   void   SetFileID();
   string GetFileID();
   
   string GetCookedTreeID();
   string GetCalibratedTreeID();

   void  PrintConstants();
   
   // limit entries for faster testing
   void  SetTestMode(int);

   //--------------------------
   // Cooking 
   void  Cook();
   
   void  InitCooking();
   void  InitCookedDataFile();
   void  InitCookedDataTree();

   // init file and connect to tree
   void  InitCookedData();
   void  CloseCookedData();
   
   void  DoCooking();
   void  SaveCookedData();
   
   float ADC_To_Wave(short ADC);
   float GetRange_mV();
   float Get_mVPerBin();
   int   Get_peakSample(int entry); 
   float Get_peakT_ns(int entry); 



   //---   
   // Study of DAQ
   void  DAQ();
   
   void  InitDAQ();
   void  SaveDAQ(string outFolder = "./Plots/DAQ/");

   double GetTrigTimeTag();
   double GetTrigTimeTag(int entry);
   
   double GetElapsedTime(int * cycles,
			 double prevTime);
   
   void  CountMissedEvents(int dTrigEntry);

   //---
   // Study of Noise
   void  Noise();

   void  InitNoise();
   void  SaveNoise(string outFolder = "./Plots/Noise/");
   
   //----
   // Study of Baseline
   void  Baseline();

   void  InitBaseline();
   void  SaveBaseline(string outFolder = "./Plots/Baseline/");

   bool  IsSampleInBaseline(short i, short option);

   //--------------------------
   // Calibration
   void  Calibrate();
   
   void  InitCalibration();
   void  InitCalibratedDataFile();
   void  InitCalibratedDataTree();
   bool  ConnectToCookedTree();
   
   void  DoCalibration();
   void  SubtractBaseline();
   
   void  SaveCalibratedData();


   ///
   void  End();
   

   //----
   // Study of Dark Counts
   void  Dark(float thresh_mV = 10.);
   
   void  InitDark();
   void  SaveDark(string outFolder = "./Plots/Dark/");

   //
   void  PrintVec(vector<short> & myVec);
   

   
 private:

   string f_fileID;
   
   // default or user input
   char   fDigitiser;        
   char   fSampSet;
   char   fPulsePol;
   
   // default or set using above
   short  fSampFreq;

   // set using header info
   short  fNSamples;
   
   // set using digitiser
   int    fNADCBins;

   // set using digitiser
   short  fRange_V;
   
   // calculate
   float  f_nsPerSamp;
   float  f_mVPerBin;
   float  fLength_ns;

   // only accommodating int size here
   Long64_t nentries64_t; // dummy
   int      nentries;

   // DAQ
   float  startTime;
   int    nMissedEvents;
   
   TH1F * hNEventsTime = nullptr;
   TH1F * hEventRate   = nullptr;
   TH1F * hTrigFreq    = nullptr;
   TH2F * hTT_EC       = nullptr;

   // Noise
   TH1F * hMean_Cooked = nullptr;
   TH1F * hPPV_Cooked  = nullptr;

   TH1F * hMin_Cooked = nullptr;
   TH1F * hMax_Cooked = nullptr;

   TH2F * hMin_Max_Cooked = nullptr;
   
   // Baseline
   TH1F * hBase = nullptr;
   
   int  nEvents_Base = 10000;
   TH2F * hEvent_Base = nullptr;

   TH1F * hPeak = nullptr;
   TH2F * hBase_Peak = nullptr;
   TH2F * hMin_Peak = nullptr;

   // Dark Counts
   TH1F * hD_Peak = nullptr;
   TH2F * hD_Min_Peak = nullptr;
   
   TCanvas * canvas = nullptr;

   void  SetDigitiser(char);
   void  SetSampSet(char);
   void  SetPulsePol(char);
   
   void  SetConstants();
   
   short SetSampleFreq();
   short SetNSamples();
   float SetLength_ns();
   
   int   SetNADCBins();
   short SetRange_V();
   float Set_mVPerBin();

   float Set_nsPerSamp();
   float SampleToTime();
   
   void  Set_THF_Params(float *,float *,float *, int *);
   
   void  InitHistos();

   void  SaveHistos();

   void  SetStyle();

};

#endif

#ifdef TConvert_cxx
TConvert::TConvert(TTree *tree,
		   char digitiser,
		   char sampSet,
		   char pulsePol) : rawTree(0) 
{
  
  if (tree == 0)
    fprintf( stderr, "\n Error: No Tree");
  
  SetDigitiser(digitiser); 
  SetSampSet(sampSet); // for desktop digitiser
  
  SetPulsePol(pulsePol); 
  
  bool success = Init(tree);
  
  if(!success)
    fprintf( stderr, "\n Error: Init failed with this file \n");
  
  //PrintConstants();
  
}

TConvert::~TConvert()
{
   if (!rawTree) return;
   delete rawTree->GetCurrentFile();
}

int TConvert::GetEntry(int entry)
{
// Read contents of entry.
   if (!rawTree) return 0;
   return rawTree->GetEntry(entry);
}
int TConvert::LoadTree(int entry)
{
// Set the environment to read one entry
   if (!rawTree) return -5;
   int centry = rawTree->LoadTree(entry);
   if (centry < 0) return centry;
   if (rawTree->GetTreeNumber() != treeNumber) {
      treeNumber = rawTree->GetTreeNumber();
   }
   return centry;
}

void TConvert::SetTestMode(int user_nentries = 1000000){

  nentries = user_nentries;  
  printf("\n Warning: nentries set to %d for testing \n",nentries);
  
}

bool TConvert::Init(TTree *tree)
{
  printf("\n ------------------------------ \n");
  printf("\n Initialising Data \n");
  
  if (!tree){
    fprintf( stderr, "\n Error: tree not loaded \n ");
    return false;
  }
  rawTree = tree;

  treeNumber = -1;
  rawTree->SetMakeClass(1);
  rawTree->SetBranchAddress("HEAD",HEAD, &b_HEAD);
  rawTree->SetBranchAddress("ADC",&ADC, &b_ADC);
  
  if (rawTree == 0){
    fprintf(stderr,"\n Error, rawTree == 0 \n ");
    return false;
  }
  
  nentries64_t = rawTree->GetEntriesFast();
  
  if( nentries64_t > INT_MAX ){
    fprintf(stderr,
	    "\n Error, nentries = (%lld) > INT_MAX unsupported \n ",
	    nentries64_t);
    return false;
  }
  else
    nentries = (int)nentries64_t;
  
  startTime = GetTrigTimeTag(0);

  // conversion factors
  SetConstants();

  SetFileID();

  SetStyle();

  InitCanvas();

  //MakeWaves();

  printf("\n ------------------------------ \n");

  return true;
}

bool TConvert::ConnectToCookedTree()
{
  printf("\n ------------------------------ \n");
  printf("\n Initialising Cooked Tree \n");

  //outFile->GetObject(GetCookedTreeID().c_str(),cookedTree);
  
  if (!cookedTree){
    fprintf( stderr, "\n Error: no cooked tree  \n ");
    return false;
  }

  cookedTree->SetBranchAddress("min_cook_mV",&min_cook_mV, &b_min_cook_mV);
  cookedTree->SetBranchAddress("max_cook_mV",&max_cook_mV, &b_max_cook_mV);
  cookedTree->SetBranchAddress("ppV_cook_mV",&ppV_cook_mV, &b_ppV_cook_mV);
  cookedTree->SetBranchAddress("mean_cook_mV",&mean_cook_mV, &b_mean_cook_mV);
  
  cookedTree->SetBranchAddress("peak_samp",&peak_samp, &b_peak_samp);

  rawTree->AddFriend(cookedTree);
  
  return true;
}

void TConvert::Show(int entry)
{
   if (!rawTree) return;
   rawTree->Show(entry);
}


#endif // #ifdef TConvert_cxx
