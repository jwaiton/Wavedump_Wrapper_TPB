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
   Int_t  fCurrent;
   
   uint HEAD[6];
   
   std::vector<short> * ADC = 0;
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
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Show(Long64_t entry = -1);
   
   void  ProcessEntries();
   void  ADC_Loop();
   
   void  GetDAQInfo();
   
   void  BeforeDAQ();
   void  AfterDAQ();
   
   void  InitHistosDAQ();
   void  SaveHistosDAQ(std::string outFolder = "./Plots/DAQ/");
   
   void  PrintConstants();

   double GetTrigTimeTag(Long64_t entry);
   
   double GetElapsedTime(const Long64_t entry, 
			 int  * cycles,
			 double prevElapsedTime);

   void CountMissedEvents(int dTrigEntry);

   int   Get_peakSample(Long64_t entry); 
   float Get_peakT_ns(Long64_t entry); 

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
   short  fRange_mV;
   
   // calculate
   float  f_nsPerSamp;
   
   float  f_mvPerBin;
   
   float  fLength_ns;

   Long64_t nentries;
   double   startTime;

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

   void  Set_THF_Params(float *,float *,float *, Long64_t *);
   
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
   
   Init(tree);

}

TConvert::~TConvert()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t TConvert::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t TConvert::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
   }
   return centry;
}

void TConvert::ProcessEntries(){
  
  ADC_Loop();
}

void TConvert::Init(TTree *tree)
{
  printf("\n ------------------------------ \n");
  printf("\n Initialising Data \n");
  
  if (!tree){
    fprintf( stderr, "\n Error: tree not loaded \n ");
    return;
  }
  fChain = tree;

  fCurrent = -1;
  fChain->SetMakeClass(1);
  fChain->SetBranchAddress("HEAD",HEAD, &b_HEAD);
  fChain->SetBranchAddress("ADC",&ADC, &b_ADC);
  
  LoadTree(0);
  
  nentries = fChain->GetEntriesFast();
  startTime = GetTrigTimeTag(0);

  // conversion factors
  SetConstants();

  SetStyle();

  InitCanvas();

  printf("\n ------------------------------ \n");
}

void TConvert::Show(Long64_t entry)
{
   if (!fChain) return;
   fChain->Show(entry);
}


#endif // #ifdef TConvert_cxx
