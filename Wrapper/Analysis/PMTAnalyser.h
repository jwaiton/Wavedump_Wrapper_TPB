#ifndef PMTAnalyser_h
#define PMTAnalyser_h

#include <iostream>

#include "TTree.h"
#include "TFile.h"

#include "DataInfo.h"

using namespace std;

class PMTAnalyser {
 public :
  TTree          *rawRootTree;  
  Int_t           fCurrent;

  char test;
  char digitiser;
  
  DataInfo      *dataInfo;
  Int_t          NSamples;
  Int_t          NVDCBins;
  Float_t        VoltageRange;
  Float_t        mVPerBin;
  Float_t        nsPerSample;

  Int_t          event;
  
  Short_t        minVDC;
  Short_t        maxVDC;
  Short_t        minT;
  Short_t        maxT;
  Short_t        pulse[5100];
  
  // new variables / variable names
  Short_t        peakSampleNo;
  Short_t        peakADC;
  
  Short_t        voltage_mV; 
  Short_t        peakVoltage_mV; 
  
  Short_t        time_ns; 
  Short_t        peakTime_ns;
  
  TBranch        *b_event;
  TBranch        *b_minVDC;
  TBranch        *b_maxVDC;
  TBranch        *b_minT;  
  TBranch        *b_maxT;  
  TBranch        *b_pulse; 
  
  PMTAnalyser(TTree *tree=0);
  virtual ~PMTAnalyser();
  virtual Int_t    GetEntry(Long64_t entry);
  virtual Long64_t LoadTree(Long64_t entry);
  virtual void     Init(TTree *tree, DataInfo *dataInfo);
  virtual Int_t    DarkRate(Float_t);
  virtual Bool_t   Notify();
  virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef PMTAnalyser_cxx
PMTAnalyser::PMTAnalyser(TTree *tree) : rawRootTree(0) 
{
  
  Init(tree,dataInfo);
}

PMTAnalyser::~PMTAnalyser()
{
  if (!rawRootTree) return;
  delete rawRootTree->GetCurrentFile();
}

Int_t PMTAnalyser::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!rawRootTree) return 0;
   return rawRootTree->GetEntry(entry);
}
Long64_t PMTAnalyser::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!rawRootTree) return -5;
   Long64_t centry = rawRootTree->LoadTree(entry);
   if (centry < 0) return centry;
   if (rawRootTree->GetTreeNumber() != fCurrent) {
      fCurrent = rawRootTree->GetTreeNumber();
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
  rawRootTree = tree;
  fCurrent = -1;
  rawRootTree->SetMakeClass(1);
  
  rawRootTree->SetBranchAddress("event", &event, &b_event);
  rawRootTree->SetBranchAddress("minVDC", &minVDC, &b_minVDC);
  rawRootTree->SetBranchAddress("maxVDC", &maxVDC, &b_maxVDC);
  rawRootTree->SetBranchAddress("minT", &minT, &b_minT);
  rawRootTree->SetBranchAddress("maxT", &maxT, &b_maxT);
  rawRootTree->SetBranchAddress("pulse", pulse, &b_pulse);

  Notify();
}

Bool_t PMTAnalyser::Notify()
{
   return kTRUE;
}

void PMTAnalyser::Show(Long64_t entry)
{
   if (!rawRootTree) return;
   rawRootTree->Show(entry);
}

#endif // #ifdef PMTAnalyser_cxx
