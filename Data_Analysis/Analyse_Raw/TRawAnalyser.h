#ifndef TRawAnalyser_h
#define TRawAnalyser_h

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

class TRawAnalyser {
 public :
  
  //--------------------
  // Input 
  TTree *rawTree;
  int   treeNumber;

  // raw root data tree variables
  uint HEAD[6];
  vector<short> * ADC = 0;
  
  TBranch * b_HEAD = 0;  
  TBranch * b_ADC  = 0;   
  
  TRawAnalyser(TTree *tree=0,
	       char digitiser='V', // Program default is VME 1730
	       char sampSet='2',   // variable only used for digitiser='D'
	       char pulsePol='N'); // 'N' Neg or 'P' Pos
  virtual ~TRawAnalyser();
  virtual int  GetEntry(int entry);
  virtual int  LoadTree(int entry);
  virtual bool Init(TTree *tree=0);
  virtual void Show(int entry = -1);
  
  void InitCanvas(float w = 1000.,
		  float h = 800.);
  void DeleteCanvas();
  
  void  PrintConstants();
  
  // limit entries for faster testing
  void  SetTestMode(int);
  
  float ADC_To_Wave(short ADC);
  float GetRange_mV();
  float Get_mVPerBin();
  float GetLength_ns();
  short GetNSamples();
  
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
  
  bool  ConnectToCookedTree();
  
  //---
  // Monitor Waveforms
  void  Waveform(char option = 'f');
  
  void  InitWaveform();
  void  SaveWaveform(string outFolder = "./Plots/Waveforms/");
  
  void  InitFFT();
  void  SaveFFT(string outFolder = "./Plots/Waveforms/");
  
  void  SaveWaveFFT(string outFolder = "./Plots/Waveforms/");
  
 private:

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

   // Waveforms
   TH1F * hWave = nullptr;   
   TH1F * hFFT = nullptr;

   TRandom3 * rand3 = nullptr;

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

#ifdef TRawAnalyser_cxx
TRawAnalyser::TRawAnalyser(TTree *tree,
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

TRawAnalyser::~TRawAnalyser()
{
   if (!rawTree) return;
   delete rawTree->GetCurrentFile();
}

int TRawAnalyser::GetEntry(int entry)
{
// Read contents of entry.
   if (!rawTree) return 0;
   return rawTree->GetEntry(entry);
}
int TRawAnalyser::LoadTree(int entry)
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

void TRawAnalyser::SetTestMode(int user_nentries = 1000000){

  nentries = user_nentries;  
  printf("\n Warning: \n ");
  printf("  nentries set to %d for testing \n",nentries);
  
}

bool TRawAnalyser::Init(TTree *tree)
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

  SetStyle();

  InitCanvas();

  printf("\n ------------------------------ \n");

  return true;
}

void TRawAnalyser::Show(int entry)
{
   if (!rawTree) return;
   rawTree->Show(entry);
}


#endif // #ifdef TRawAnalyser_cxx
