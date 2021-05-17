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
  
  //--------------------
  // Input 
  TTree *rawTree;
  int   treeNumber;

  // raw root data tree variables
  uint HEAD[6];
  vector<short> * ADC = 0;     // reading
  vector<short>   ADC_buff;    // writing
  
  TBranch * b_HEAD = 0;  
  TBranch * b_ADC  = 0;   
  
  //--------------------
  // Output
  TFile * outFile;
  
  // meta data tree for 
  // storing constants
  TTree * metaTree;
  TTree * cookedTree;
  
  // dont save - ADC is smaller
  vector <float>  wave_mV;

  // variables for writing
  float base_mV; // baseline (average in mV) 
  float min_mV;
  float peak_mV;
  float mean_mV;
  short peak_samp;
  float start_s; // event start time

  TCooker(TTree *tree=0,
	  char digitiser='V', // Program default is VME 1730
	  char sampSet='2',   // variable only used for digitiser='D'
	  char pulsePol='N'); // 'N' Neg or 'P' Pos
  virtual ~TCooker();
  virtual int  GetEntry(int entry);
  virtual int  LoadTree(int entry);
  virtual bool Init(TTree *tree=0);
  virtual void Show(int entry = -1);
  
  void InitCanvas(float w = 1000.,
		  float h = 800.);
  void DeleteCanvas();
  
  //void   SetFileID();
  void   SetFileID(string userFileID);
  string GetFileID();
  
  void   SetDir(string userFileDir);
  string GetDir();
  
  ///
  void   SetRun(int);
  void   SetPMT(int);
  void   SetLoc(int);
  void   SetTest(char);
  void   SetHVStep(int);
  ///

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
  float Wave_To_Amp_Scaled_Wave(float wave);

  short Wave_To_ADC(float wave_mV);
  float Amp_Scaled_Wave_To_Unscaled_Wave(float wave);

  float GetRange_mV();
  float Get_mVPerBin();
  float GetLength_ns();
  short GetNSamples();
  short GetNADCBins();

  //---   
  // Monitor DAQ
  void  DAQ();
  
  void  InitDAQ();
  void  SaveDAQ(string outFolder = "./Plots/DAQ/");
  
  double GetTrigTimeTag();
  double GetTrigTimeTag(int entry);
  
  double GetElapsedTime(int * cycles,
			double prevTime);
  
  void  CountMissedEvents(int dTrigEntry);


  short Invert_Negative_ADC_Pulses(short ADC);
  
  void  InitBaseline();
  void  SaveBaseline(string outFolder = "./Plots/Baseline/");
  
  bool  IsSampleInBaseline(short iSample);

  
  void  CloseCookedFile();
  
  void  SetAmpGain(float amp_gain);
  void  SetFirstMaskBin(short first_mask_bin);

 private:
  
  string f_fileID;
  string f_fileDir;
  char   FileID[128];   

  int    fRun;
  int    fPMT;
  int    fLoc;
  char   fTest;
  int    fHVStep;
  
  // default or user input
  char   fDigitiser;        
  char   fSampSet;
  char   fPulsePol;
  
  float  fAmpGain;
  
  // default or set using above
  short  fSampFreq;

  short  fFirstMaskBin;
  
  // set using header info
  short  fNSamples;
  
  // set using digitiser
  short  fNADCBins;
  
  // set using digitiser
  short  fRange_V;
  
  // calculate
  float  f_nsPerSamp;
  float  f_mVPerBin;
  float  fLength_ns;
  
  // mean of peak time
  float  fDelay_ns; 

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
  
  // Baseline
  TH1F * hBase = nullptr;
  
  int    nEvents_Base = 10000;
  TH2F * hEvent_Base = nullptr;
  
  TH1F * hPeak = nullptr;
  TH2F * hBase_Peak = nullptr;
  TH2F * hMin_Peak = nullptr;
  
  
  TCanvas * canvas = nullptr;
  
  void  SetDigitiser(char);
  void  SetSampSet(char);
  void  SetPulsePol(char);
  
  void  SetConstants();
  
  short SetSampleFreq();
  short SetNSamples();
  float SetLength_ns();
  
  short SetNADCBins();
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
  
  // initialise to default
  // user to set cook_raw.C
  SetAmpGain(10);
  SetFirstMaskBin(-1);

  //SetFileID();
  
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

  // conversion factors
  SetConstants();

  SetStyle();

  InitCanvas();

  printf("\n ------------------------------ \n");

  return true;
}


void TCooker::Show(int entry)
{
   if (!rawTree) return;
   rawTree->Show(entry);
}


#endif // #ifdef TCooker_cxx
