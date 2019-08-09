#ifndef PMTAnalyser_h
#define PMTAnalyser_h

#include <iostream>

#include "TTree.h"
#include "TFile.h"

#include "TH2.h"

#include "DataInfo.h"
#include "FileNameParser.h"

#include <TCanvas.h>

#include "TRandom3.h"

using namespace std;

class PMTAnalyser {
 public :
  TTree          *rawRootTree;  
  Int_t           fCurrent;

  char  Test;
  Int_t HVStep;
  Int_t Loc;
  Int_t PMT;
  Int_t Run;
  TString FileID;

  char  digitiser;
  
  DataInfo       * dataInfo;
  FileNameParser * testInfo;

  Int_t          NSamples;
  Int_t          NVDCBins;
  Float_t        VoltageRange;
  Float_t        mVPerBin;
  Float_t        nsPerSample;
  bool           negPulsePol;
  
  Float_t        waveformDuration;

  Int_t          event;
  
  Short_t        minVDC;
  Short_t        maxVDC;
  Short_t        minT;
  Short_t        maxT;
  
  Float_t        peakT_ns;
  Float_t        peakV_mV;
  
  Short_t        waveform[5100];
  
  TBranch        * b_event;
  TBranch        * b_minVDC;
  TBranch        * b_maxVDC;
  TBranch        * b_minT;  
  TBranch        * b_maxT;  
  TBranch        * b_peakT_ns;  
  TBranch        * b_peakV_mV;  
  TBranch        * b_waveform; 
  
  TRandom3       * rand3; 

  PMTAnalyser(TTree *tree=0, Char_t userDigitiser='V',
	      Bool_t oldRootFileVersion = kFALSE);
  
  virtual ~PMTAnalyser();
  virtual Int_t    GetEntry(Long64_t entry);
  virtual Long64_t LoadTree(Long64_t entry);
  virtual void     Init(TTree *tree,Char_t digitiser,
			Bool_t oldRootFileVersion);
  Int_t    GetNEntriesTest(Int_t, Int_t);
  Bool_t   IsValidEntry(Long64_t);
  void     PlotFFT(Long64_t entry);
  void     PlotWaveform(Long64_t entry);
  void     PlotAccumulatedFFT();
  void     Get_hWave(Long64_t entry, TH1F * hWave);
  TH1F *   Get_hFFT(Long64_t entry);
  Int_t    DarkRate(Float_t);
  Bool_t   IsSampleInBaseline(int,Short_t);
  Short_t  Get_baseline_ADC(Long64_t,
			    Short_t option = 0);
  Float_t  Get_baseline_mV(Long64_t,
			   Short_t option = 0);
  //  Short_t  GetNPeaks(Int_t, Float_t);    
  Short_t  Select_peakSample(Short_t waveform[],
			     Short_t peakVDC);
  Float_t  TimeOfPeak(Float_t);
  TH1F*    FFTShift(TH1F *, Float_t);
  TCanvas* Make_FFT_Canvas();
  Int_t    FFT_Filter();
  Bool_t   IsCleanFFTWaveform(TH1F *);
  void     MakeCalibratedTree();
  Bool_t   Notify();
  void     Show(Long64_t entry = -1);
  void     SetStyle();
  void     SetTestMode(Bool_t userTestMode = kTRUE);
  void     RiseFallTime();
  
 private:

  Bool_t testMode;

};

#endif

#ifdef PMTAnalyser_cxx

PMTAnalyser::PMTAnalyser(TTree *tree,
			 Char_t userDigitiser,
			 Bool_t oldRootFileVersion) : rawRootTree(0) 
{
  digitiser = userDigitiser;
  Init(tree,digitiser,
       oldRootFileVersion);
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
		       Char_t digitiser,
		       Bool_t oldRootFileVersion)
{

  // default
  testMode = kFALSE;
  
  testInfo = new FileNameParser(tree->GetName());
  
  Test     = testInfo->Test;
  HVStep   = testInfo->HVStep;
  Run      = testInfo->Run;
  PMT      = testInfo->PMT;
  Loc      = testInfo->Loc;
  FileID   = testInfo->FileID;
  
  dataInfo = new DataInfo();

  NSamples     = dataInfo->GetNSamples(Test,
				       digitiser);
  VoltageRange = dataInfo->GetVoltageRange(digitiser);
  NVDCBins     = dataInfo->GetNVDCBins(digitiser);
  mVPerBin     = dataInfo->GetmVPerBin(digitiser);  
  nsPerSample  = dataInfo->GetnsPerSample(digitiser);  
  negPulsePol  = dataInfo->GetNegPulsePol(Test);  
  
  waveformDuration = (float)NSamples * nsPerSample;
  
  cout << endl;
  cout << " NSamples         = " << NSamples         << endl;
  cout << " VoltageRange     = " << VoltageRange     << endl;
  cout << " NVDCBins         = " << NVDCBins         << endl;
  cout << " mVPerBin         = " << mVPerBin         << endl;
  cout << " nsPerSample      = " << nsPerSample      << endl;
  cout << " negPulsePol      = " << negPulsePol      << endl;
  cout << " waveformDuration = " << waveformDuration << endl;
  cout << endl;

  // initalise to 0 to guarantee uniqueness
  rand3 = new TRandom3(0); 

  if (!tree) return;
  rawRootTree = tree;
  fCurrent = -1;
  rawRootTree->SetMakeClass(1);
  
  rawRootTree->SetBranchAddress("event", &event, &b_event);
  rawRootTree->SetBranchAddress("minVDC", &minVDC, &b_minVDC);
  rawRootTree->SetBranchAddress("maxVDC", &maxVDC, &b_maxVDC);
  rawRootTree->SetBranchAddress("minT", &minT, &b_minT);
  rawRootTree->SetBranchAddress("maxT", &maxT, &b_maxT);

  if(oldRootFileVersion)
    rawRootTree->SetBranchAddress("pulse",waveform, &b_waveform);
  else{
    rawRootTree->SetBranchAddress("waveform", waveform, &b_waveform);
    rawRootTree->SetBranchAddress("peakT_ns", &peakT_ns, &b_peakT_ns);
    rawRootTree->SetBranchAddress("peakV_mV", &peakV_mV, &b_peakV_mV);
  }
  
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
