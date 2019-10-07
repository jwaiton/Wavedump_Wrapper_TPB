#ifndef TCooker_h
#define TCooker_h

#include <TROOT.h>
#include <TTree.h>
#include <TFile.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TRandom3.h>

#include <vector>
#include <limits.h>

using namespace std;

class TCooker {
 public :
  
  // Input variables
  TTree *rawTree;
  
  int   treeNumber;
  
  uint HEAD[6];
  
  vector<short> * ADC  = 0;
  
  TBranch * b_HEAD = 0;  
  TBranch * b_ADC  = 0;   
  
   TFile * outFile;

   // meta data tree for 
   // storing constants
   TTree * metaTree;
   
   TTree * cookedTree;

   // for writing
   vector <float>  wave_buff;
   
   // for reading
   vector <float> * wave_mV = 0;
   
   float min_mV;
   float max_mV;
   float ppV_mV;
   float mean_mV;
   short peak_samp;
   
   TBranch * b_wave_mV = 0;
   TBranch * b_min_mV  = 0;  
   TBranch * b_max_mV  = 0;  
   TBranch * b_ppV_mV  = 0;  
   TBranch * b_mean_mV = 0;  
   TBranch * b_peak_samp = 0;  
   
   // Output (to save)
   TTree * outTree;

   TCooker(TTree *tree=0,
	    char digitiser='V', // Program default is VME 1730
	    char sampSet='3',   // variable only used for digitiser='D'
	    char pulsePol='N'); // 'N' Neg or 'P' Pos
   virtual ~TCooker();
   virtual int  GetEntry(int entry);
   virtual int  LoadTree(int entry);
   virtual bool Init(TTree *tree=0);
   virtual void Show(int entry = -1);
   
   void InitCanvas(float w = 1000.,
		   float h = 800.);
   void DeleteCanvas();

   void   SetFileID();
   string GetFileID();
   
   string GetCookedTreeID();

   void  PrintConstants();
   
   // limit entries for faster testing
   void  SetTestMode(int);

   //--------------------------
   // Cooking 
   void  Cook();
   
   void  InitCooking();
   void  InitCookedDataFile(string option = "RECREATE");

   void  InitMetaDataTree();
   void  InitCookedDataTree();

   // init file and connect to tree

   void  InitCookedData();
   void  CloseCookedData();
   
   void  DoCooking();
   
   void  SaveMetaData();
   void  SaveCookedData();
   
   float ADC_To_Wave(short ADC);
   float GetRange_mV();
   float Get_mVPerBin();
   float GetLength_ns();
   short GetNSamples();
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

   bool  ConnectToCookedTree();

   //---
   // Study of Noise
   void  Noise();

   void  InitNoise();
   void  SaveNoise(string outFolder = "./Plots/Noise/");

   //---
   // Waveform Plotter
   void  Waveform(char option = 'f');

   void  InitWaveform();
   void  SaveWaveform(string outFolder = "./Plots/Waveforms/");
   
   void  InitFFT();
   void  SaveFFT(string outFolder = "./Plots/Waveforms/");
  
   void  SaveWaveFFT(string outFolder = "./Plots/Waveforms/");

   //----
   // Study of Baseline
   void  Baseline();

   void  InitBaseline();
   void  SaveBaseline(string outFolder = "./Plots/Baseline/");

   bool  IsSampleInBaseline(short i, short option);
   
   ///
   void  CloseCookedFile();
   
 private:

   string f_fileID;
   char   FileID[128]; 
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
   
   float  thresh_mV;
   float  th_low_mV;
   float  noise_thresh_mV;
   float  noise_th_low_mV;

   TH1F * hMean_Cooked = nullptr;
   TH1F * hPPV_Cooked  = nullptr;

   TH1F * hMin_Cooked = nullptr;
   TH1F * hMax_Cooked = nullptr;

   TH2F * hMin_Max_Cooked = nullptr;
   
   // Waveforms
   TH1F * hWave = nullptr;   
   TH1F * hFFT = nullptr;

   TRandom3 * rand3 = nullptr;

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
   
   void  SetStyle();

};

#endif

#ifdef TCooker_cxx
TCooker::TCooker(TTree *tree,
		   char digitiser,
		   char sampSet,
		   char pulsePol) : rawTree(0) 
{
  
  SetDigitiser(digitiser); 
  SetSampSet(sampSet); // for desktop digitiser
  SetPulsePol(pulsePol); 
  
  if (tree == 0){
    fprintf( stderr, "\n Warning: No input tree");
    
    Init(tree);
    
  }
  else{
  bool success = Init(tree);
  
  if(!success)
    fprintf( stderr, "\n Warning: raw tree not initialised \n");
  }
  
}

TCooker::~TCooker()
{
   if (!rawTree) return;
   delete rawTree->GetCurrentFile();
}

int TCooker::GetEntry(int entry)
{
// Read contents of entry.
   if (!rawTree) return 0;
   return rawTree->GetEntry(entry);
}
int TCooker::LoadTree(int entry)
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

void TCooker::SetTestMode(int user_nentries = 1000000){

  nentries = user_nentries;  
  printf("\n Warning: \n ");
  printf("  nentries set to %d for testing \n",nentries);
  
}

bool TCooker::Init(TTree *tree)
{
  printf("\n ------------------------------ \n");
  printf("\n Initialising Data \n");
  
  nentries64_t = 0;

  if (!tree){
    fprintf( stderr, "\n Warning: tree not loaded \n ");
  }
  else{
    
    rawTree = tree;
    treeNumber = -1;
    rawTree->SetMakeClass(1);
    rawTree->SetBranchAddress("HEAD",HEAD, &b_HEAD);
    rawTree->SetBranchAddress("ADC",&ADC, &b_ADC);

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
    
  }
  
  rand3 = new TRandom3(0);

  // conversion factors
  SetConstants();

  SetFileID();

  SetStyle();

  InitCanvas();

  printf("\n ------------------------------ \n");

  return true;
}

bool TCooker::ConnectToCookedTree()
{
  printf("\n ------------------------------- \n");
  printf("\n Connecting to cooked data TTree   ");
  printf("\n   %s \n",GetCookedTreeID().c_str());

  outFile->GetObject(GetCookedTreeID().c_str(),cookedTree);
  
  if (!cookedTree){
    fprintf( stderr, "\n Error: no cooked tree  \n ");
    return false;
  }

  cookedTree->SetBranchAddress("wave_mV",&wave_mV, &b_wave_mV);

  cookedTree->SetBranchAddress("min_mV",&min_mV, &b_min_mV);
  cookedTree->SetBranchAddress("max_mV",&max_mV, &b_max_mV);
  cookedTree->SetBranchAddress("ppV_mV",&ppV_mV, &b_ppV_mV);
  cookedTree->SetBranchAddress("mean_mV",&mean_mV, &b_mean_mV);

  cookedTree->SetBranchAddress("peak_samp",&peak_samp, &b_peak_samp);

  /* if(rawTree) */
  /*     rawTree->AddFriend(cookedTree); */
  
  return true;
}

void TCooker::Show(int entry)
{
   if (!rawTree) return;
   rawTree->Show(entry);
}


#endif // #ifdef TCooker_cxx
