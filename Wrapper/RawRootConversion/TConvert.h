#ifndef TConvert_h
#define TConvert_h

#include <TROOT.h>
#include <TChain.h>
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
   TTree *fChain;
   int   fCurrent;

   uint HEAD[6];
   
   vector<short> * ADC  = 0;

   TBranch * b_HEAD = 0;  
   TBranch * b_ADC  = 0;   
   
   
   TFile * outFile;

   // Intermediate processing variables 
   // not written 
   
   TTree * rawTree;
   
   float min_raw_mV;
   float max_raw_mV;
   float ppV_raw_mV;
   float mean_raw_mV;

   TBranch * b_min_raw_mV = 0;  
   TBranch * b_max_raw_mV = 0;  
   TBranch * b_ppV_raw_mV = 0;  
   TBranch * b_mean_raw_mV = 0;  

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

   bool InitRawTree();
   
   void InitCanvas(float w = 1000.,
		   float h = 800.);
   void DeleteCanvas();
   
   void  InitCalibration();
   void  Calibrate();
   
   void   SetFileID();
   string GetFileID();

   void  CalibrateRaw();
   void  SubtractBaseline();

   float GetRange_mV();
   float Get_mVPerBin();
      
   float ADC_To_Wave(short ADC);

   void  PrintConstants();

   //---   
   // Study of DAQ
   void  DAQ();
   
   void  InitDAQ();
   void  SaveDAQ(string outFolder = "./Plots/DAQ/");
  
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

   //---
   void  Dark(float thresh_mV = 10.);
   
   void  InitDark();
   void  SaveDark(string outFolder = "./Plots/Dark/");
   //

   float GetTrigTimeTag(int entry);
   
   float GetElapsedTime(const int entry, 
			 int  * cycles,
			 float prevElapsedTime);

   void CountMissedEvents(int dTrigEntry);

   int   Get_peakSample(int entry); 
   float Get_peakT_ns(int entry); 

   void  PrintVec(vector<short> & myVec);
   
   void  SetTestMode(int);
   
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
   TH1F * hMean_Raw = nullptr;
   TH1F * hPPV_Raw  = nullptr;

   TH1F * hMinRaw = nullptr;
   TH1F * hMaxRaw = nullptr;

   TH2F * hMin_Max_Raw = nullptr;
   
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
		   char pulsePol) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {

     printf(" Tree is zero ");

      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("wave_0.dat.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("wave_0.dat.root");
      }
      f->GetObject("T",tree);

   }
   
   SetDigitiser(digitiser); 
   SetSampSet(sampSet); // for desktop digitiser

   SetPulsePol(pulsePol); 
   
   bool success = Init(tree);
   
   if(!success)
     fprintf( stderr, "\n Error: Init failed with this file \n");
   
   PrintConstants();

}

TConvert::~TConvert()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

int TConvert::GetEntry(int entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
int TConvert::LoadTree(int entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   int centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
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
  fChain = tree;

  fCurrent = -1;
  fChain->SetMakeClass(1);
  fChain->SetBranchAddress("HEAD",HEAD, &b_HEAD);
  fChain->SetBranchAddress("ADC",&ADC, &b_ADC);
  
  if (fChain == 0){
    fprintf(stderr,"\n Error, fChain == 0 \n ");
    return false;
  }
  
  nentries64_t = fChain->GetEntriesFast();
  
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

  SetStyle();

  InitCanvas();

  //MakeWaves();

  printf("\n ------------------------------ \n");

  return true;
}

bool TConvert::InitRawTree()
{
  printf("\n ------------------------------ \n");
  printf("\n  Initialising Raw Tree \n");
  
  if (!rawTree){
    fprintf( stderr, "\n Error: no raw tree  \n ");
    return false;
  }

  rawTree->SetBranchAddress("min_raw_mV",&min_raw_mV, &b_min_raw_mV);
  rawTree->SetBranchAddress("max_raw_mV",&max_raw_mV, &b_max_raw_mV);
  rawTree->SetBranchAddress("ppV_raw_mV",&ppV_raw_mV, &b_ppV_raw_mV);
  rawTree->SetBranchAddress("mean_raw_mV",&mean_raw_mV, &b_mean_raw_mV);
  
  rawTree->SetBranchAddress("peak_samp",&peak_samp, &b_peak_samp);

  fChain->AddFriend(rawTree);
  
  return true;
}

void TConvert::Show(int entry)
{
   if (!fChain) return;
   fChain->Show(entry);
}


#endif // #ifdef TConvert_cxx
