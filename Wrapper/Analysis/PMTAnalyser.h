#ifndef PMTAnalyser_h
#define PMTAnalyser_h

#include <iostream>

#include "TTree.h"
#include "TFile.h"

#include "DataInfo.h"

using namespace std;

class PMTAnalyser {
 public :
  TTree          *fChain;   // pointer to the analyzed TTree or TChain
  Int_t           fCurrent; // current Tree number in a TChain

  char test;
  char digitiser;
  
  DataInfo      *dataInfo;
  Int_t          NSamples;
  Int_t          NVDCBins;
  Float_t        VoltageRange;
  Float_t        mVPerBin;
  Float_t        nsPerSample;

  Int_t           event;
  
  Short_t         minVDC;
  Short_t         maxVDC;
  Short_t         minT;
  Short_t         maxT;
  Short_t         pulse[5100];
  
  Short_t         milliVolts; 
  Short_t         peakTime; 
  
  TBranch        *b_event;
  TBranch        *b_minVDC;
  TBranch        *b_maxVDC;
  TBranch        *b_minT;  
  TBranch        *b_maxT;  
  TBranch        *b_pulse; 
  
  PMTAnalyser(TTree *tree=0);
  virtual ~PMTAnalyser();
  virtual Int_t    Cut(Long64_t entry);
  virtual Int_t    GetEntry(Long64_t entry);
  virtual Long64_t LoadTree(Long64_t entry);
  virtual void     Init(TTree *tree, DataInfo *dataInfo);
  virtual void     Loop();
  virtual Bool_t   Notify();
  virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef PMTAnalyser_cxx
PMTAnalyser::PMTAnalyser(TTree *tree) : fChain(0) 
{
  
  Init(tree,dataInfo);
}

PMTAnalyser::~PMTAnalyser()
{
  if (!fChain) return;
  delete fChain->GetCurrentFile();
}

Int_t PMTAnalyser::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t PMTAnalyser::LoadTree(Long64_t entry)
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

void PMTAnalyser::Init(TTree *tree,
		       DataInfo *dataInfo)
{
  // To do: Get test type from Tree Name
  test      = 'D';
  
  dataInfo = new DataInfo();
  
  NSamples     = dataInfo->GetNSamples(test);
  VoltageRange = dataInfo->GetVoltageRange();
  NVDCBins     = dataInfo->GetNVDCBins();
  mVPerBin     = dataInfo->GetmVPerBin();  
  nsPerSample  = dataInfo->GetnsPerSample();  


  if (!tree) return;
  fChain = tree;
  fCurrent = -1;
  fChain->SetMakeClass(1);
  
  fChain->SetBranchAddress("event", &event, &b_event);
  fChain->SetBranchAddress("minVDC", &minVDC, &b_minVDC);
  fChain->SetBranchAddress("maxVDC", &maxVDC, &b_maxVDC);
  fChain->SetBranchAddress("minT", &minT, &b_minT);
  fChain->SetBranchAddress("maxT", &maxT, &b_maxT);
  fChain->SetBranchAddress("pulse", pulse, &b_pulse);
  Notify();
}

Bool_t PMTAnalyser::Notify()
{
   return kTRUE;
}

void PMTAnalyser::Show(Long64_t entry)
{
   if (!fChain) return;
   fChain->Show(entry);
}

Int_t PMTAnalyser::Cut(Long64_t entry)
{
   return 1;
}

#endif // #ifdef PMTAnalyser_cxx
