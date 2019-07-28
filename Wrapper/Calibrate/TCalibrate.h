#ifndef TCalibrate_h
#define TCalibrate_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH2.h>

class TCalibrate {
public :
   TTree *fChain;
   Int_t  fCurrent;

   const static short maxNS = 5100;
   
   int   HEAD[6];
   short ADC[maxNS];

   TBranch *b_HEAD;  
   TBranch *b_ADC;   

   int   EC = 0; 
   float trigTimeTag = 0; // Trigger Time Tag (8 ns for V1730)

   TH2F * h2 = new TH2F("h2","h2;trig time tag (ns);peak time (ns)",
			128,0,8.,
			110,-1.0,219.); 
   

   TCalibrate(TTree *tree=0,
	      char digitiser='V', // Program default is VME 1730
	      char sampSet='3');  // only relevant for digitiser='D'
   virtual ~TCalibrate();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
   
   void  PreLoop();
   void  PostLoop();
   int   Get_peakSample(short[]);
   float Get_peakT_ns(short[]);

 private:

   // default or user input
   char   fDigitiser;        
   char   fSampSet;
   
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

   void  SetDigitiser(char);
   void  SetSampSet(char);
   
   void  SetConstants();
   void  PrintConstants();

   short SetSampleFreq();
   short SetNSamples();
   float SetLength_ns();
   
   int   SetNADCBins();
   short SetRange_mV();
   float Set_mVPerBin();
   float Set_nsPerSamp();

};

#endif

#ifdef TCalibrate_cxx
TCalibrate::TCalibrate(TTree *tree,
		       char digitiser,
		       char sampSet) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("wave_0.dat.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("wave_0.dat.root");
      }
      f->GetObject("T",tree);

   }
   
   SetDigitiser(digitiser); 
   SetSampSet(sampSet); // for desktop digitiser
   Init(tree);
}

TCalibrate::~TCalibrate()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t TCalibrate::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t TCalibrate::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void TCalibrate::Init(TTree *tree)
{
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);
   fChain->SetBranchAddress("HEAD[6]", HEAD, &b_HEAD);

   // calibration factors
   SetConstants();
   PrintConstants();
  
   char name[50];
   sprintf(name,"ADC[%d]",fNSamples);
   fChain->SetBranchAddress(name,ADC, &b_ADC);

   Notify();

}


Bool_t TCalibrate::Notify()
{
   return kTRUE;
}

void TCalibrate::Show(Long64_t entry)
{
   if (!fChain) return;
   fChain->Show(entry);
}

Int_t TCalibrate::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef TCalibrate_cxx
