#ifndef TConvert_h
#define TConvert_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TStyle.h>

#include <vector>

class TConvert {
public :
   TTree *fChain;
   int   fCurrent;
   
   uint HEAD[6];
   
   std::vector<short> * ADC  = 0;
   std::vector<float> * wave = 0;

   TBranch *b_HEAD = 0;  
   TBranch *b_ADC  = 0;   

   // event counter
   int   EC = 0; 
   float trigTimeTag = 0; //  (16 ns resolution)

   TConvert(TTree *tree=0,
	    char digitiser='V', // Program default is VME 1730
	    char sampSet='3',   // variable only used for digitiser='D'
	    char pulsePol='N'); // Neg or Pos
   virtual ~TConvert();
   virtual int  GetEntry(int entry);
   virtual int  LoadTree(int entry);
   virtual bool Init(TTree *tree);
   virtual void Show(int entry = -1);
   
   void  ProcessEntries();
   void  ADC_Loop();

   //---   
   void  GetDAQInfo();
   
   void  BeforeDAQ();
   void  AfterDAQ();
   
   void  InitHistosDAQ();
   void  SaveHistosDAQ(std::string outFolder = "./Plots/DAQ/");
   //---

   void  Noise();

   void  PrintConstants();

   double GetTrigTimeTag(int entry);
   
   double GetElapsedTime(const int entry, 
			 int  * cycles,
			 double prevElapsedTime);

   void CountMissedEvents(int dTrigEntry);

   int   Get_peakSample(int entry); 
   float Get_peakT_ns(int entry); 

   void  PrintVec(std::vector<short> & myVec);

   int   nMissedEvents;

 private:

   // default or user input
   char   fDigitiser;        
   char   fSampSet;
   char   fPulsePol;
   
   // set using above
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

   Long64_t nentries64_t;
   int    nentries;
   double startTime;

   TH1F * hNEventsTime = nullptr;
   TH1F * hEventRate   = nullptr;
   TH1F * hTrigFreq    = nullptr;
   TH2F * hTT_EC       = nullptr;

   TCanvas * canvas    = nullptr;

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

   //void  MakeWaves();

   void  Set_THF_Params(float *,float *,float *, int *);
   
   void  InitCanvas();
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

void TConvert::ProcessEntries(){
  
  ADC_Loop();
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
  
  LoadTree(0);
  
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

void TConvert::Show(int entry)
{
   if (!fChain) return;
   fChain->Show(entry);
}


#endif // #ifdef TConvert_cxx
