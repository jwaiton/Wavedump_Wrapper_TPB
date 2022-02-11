#ifndef RawData_h
#define RawData_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TCanvas.h>

class RawData {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain
   
// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           event;
   Short_t         minVDC;
   Short_t         maxVDC;
   Short_t         minT;
   Short_t         maxT;
   Float_t         peakT_ns;
   Float_t         peakV_mV;
   Short_t         waveform[1024];

   // List of branches
   TBranch        *b_event;   //!
   TBranch        *b_minVDC;   //!
   TBranch        *b_maxVDC;   //!
   TBranch        *b_minT;   //!
   TBranch        *b_maxT;   //!
   TBranch        *b_peakT_ns;   //!
   TBranch        *b_peakV_mV;   //!
   TBranch        *b_waveform;   //!

   RawData(Int_t userRun = 50, TTree *tree=0);
   virtual ~RawData();
   virtual Int_t     GetEntry(Long64_t entry);
   virtual Long64_t  LoadTree(Long64_t entry);
   virtual void      Init(TTree *tree);
   virtual Double_t * Modes4Gates(Double_t gateWidths[],
				  Int_t cableLength,
				  Int_t verbosity = 0);
   virtual Bool_t    Notify();
   virtual void      Show(Long64_t entry = -1);

 private:
   
   Int_t     run;
};

#endif

#ifdef RawData_cxx
RawData::RawData(Int_t userRun,TTree *tree) : fChain(0) 
{

  run = userRun;
  // if parameter tree is not specified (or zero), connect the file
  // used to generate this class and read the Tree.
  if (tree == 0) {
    
    TString ID = "Run_50_PMT_152_Loc_0_HV_4";
    ID.Form("Run_%d_PMT_152_Loc_0_HV_4",run);
    
    TString fileName = ID + ".root";
    
    TString filePath = "/Users/gsmith23/Desktop/Watchman/Testing/Wavedump_Wrapper/RawRootData/";
    
    fileName = filePath + fileName;
    
    TString treeName = "Events_" + ID;
    
    TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(fileName);
    if (!f || !f->IsOpen()) {
      f = new TFile(fileName);
    }
    f->GetObject(treeName,tree);

   }
   Init(tree);
}

RawData::~RawData()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t RawData::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t RawData::LoadTree(Long64_t entry)
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

void RawData::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("event", &event, &b_event);
   fChain->SetBranchAddress("minVDC", &minVDC, &b_minVDC);
   fChain->SetBranchAddress("maxVDC", &maxVDC, &b_maxVDC);
   fChain->SetBranchAddress("minT", &minT, &b_minT);
   fChain->SetBranchAddress("maxT", &maxT, &b_maxT);
   fChain->SetBranchAddress("peakT_ns", &peakT_ns, &b_peakT_ns);
   fChain->SetBranchAddress("peakV_mV", &peakV_mV, &b_peakV_mV);
   fChain->SetBranchAddress("waveform", waveform, &b_waveform);
   Notify();
}

Bool_t RawData::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void RawData::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
#endif // #ifdef RawData_cxx
