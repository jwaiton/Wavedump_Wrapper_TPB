#ifndef TConvert_h
#define TConvert_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TStyle.h>

class TConvert {
public :
   TTree *fChain;
   Int_t  fCurrent;

   // maximum number of samples
   const static short maxNS = 5100;
   
   unsigned int HEAD[6];
   short ADC[maxNS];

   TBranch *b_HEAD;  
   TBranch *b_ADC;   

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
   virtual void     Loop();
   virtual void     Show(Long64_t entry = -1);
   
   void  PreLoop();
   void  PostLoop();

   void  PrintConstants();

   int   Get_peakSample(short[]);
   float Get_peakT_ns(short[]);

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

   void  SetDigitiser(char);
   void  SetSampSet(char);
   void  SetPulsePol(char);
   
   void  SetConstants();

   short SetSampleFreq();
   short SetNSamples();
   float SetLength_ns();
   
   int   SetNADCBins();
   short SetRange_mV();
   float Set_mVPerBin();
   float Set_nsPerSamp();

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

void TConvert::Init(TTree *tree)
{
  printf("\n --------------------  \n");
  printf("\n Initialising  \n");
  
  if (!tree){
    fprintf( stderr, "\n Error: tree not loaded \n ");
    return;
  }
  fChain = tree;
  fCurrent = -1;
  fChain->SetMakeClass(1);
  fChain->SetBranchAddress("HEAD",HEAD, &b_HEAD);
  fChain->SetBranchAddress("ADC",ADC, &b_ADC);
  
  // conversion factors
  SetConstants();
  PrintConstants();

  SetStyle();

  printf("\n --------------------  \n");
}

void TConvert::Show(Long64_t entry)
{
   if (!fChain) return;
   fChain->Show(entry);
}


#endif // #ifdef TConvert_cxx
