#ifndef TCookedAnalyser_h
#define TCookedAnalyser_h

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

#include "FileNameParser.h"

using namespace std;

class TCookedAnalyser {
 public :

  FileNameParser * fnp = nullptr;
  TFile * inFile = nullptr;

  // meta data tree for 
  // storing constants
  TTree * metaTree;
  
  short  SampFreq;
  short  NSamples;
  int    NADCBins;
  short  Range_V;
  float  nsPerSamp;
  float  mVPerBin;
  float  Length_ns;
  char   FileID_char[128]; 
  string FileID; 
  
  TBranch * b_SampFreq  = 0;
  TBranch * b_NSamples  = 0;
  TBranch * b_NADCBins  = 0;
  TBranch * b_Range_V   = 0;
  TBranch * b_nsPerSamp = 0;
  TBranch * b_mVPerBin  = 0;
  TBranch * b_Length_ns = 0;
  //TBranch * b_FileID    = 0;

  //--------------------
  // cooked data
  TTree * cookedTree;
  
  // Input 
  vector <float> * wave_mV = 0;   
  float min_mV;
  float max_mV;
  float ppV_mV;
  float mean_mV;
  short peak_samp;
  
  TBranch * b_wave_mV = 0;
  TBranch * b_min_mV  = 0;  
  TBranch * b_max_mV  = 0;  
  TBranch * b_ppV_mV  = 0;  
  TBranch * b_mean_mV = 0;  
  TBranch * b_peak_samp = 0;  
  
  TCookedAnalyser(string path);
  ~TCookedAnalyser();
  int  GetEntry(int entry);
  void Init();

  void  InitMeta();
  void  InitCooked();
   
  void InitCanvas(float w = 1000.,
		  float h = 800.);
  void DeleteCanvas();
  
  string GetFileID();
  
  string GetCookedTreeID();
  string GetMetaTreeID();
  
  void  PrintMetaData();
  
  // limit entries for faster testing
  void  SetTestMode(int);
  
  
  //----
  // Study Dark Counts
  void  Dark(float thresh_mV = 10.);
  void  InitDark();
  void  SaveDark(string outFolder = "./Plots/Dark/");

 private:
  
   // only accommodating int size here
   Long64_t nentries64_t; // dummy
   int      nentries;

   // Dark Counts
   TH1F * hD_Peak = nullptr;
   TH2F * hD_Min_Peak = nullptr;
   
   TCanvas * canvas = nullptr;
   
   void  Set_THF_Params(float *,float *,float *, int *);
   
   void  SetStyle();

};

#endif

#ifdef TCookedAnalyser_cxx
TCookedAnalyser::TCookedAnalyser(string path) 
{

  inFile = new TFile(path.c_str(),"READ");
  
  if ( !inFile || !inFile->IsOpen()) {
    fprintf(stderr,"\n Error, Check File: %s \n",path.c_str());
    return;
  }
  
  fnp = new FileNameParser(path);  
  FileID = fnp->GetFileID();
  
  Init();
}

TCookedAnalyser::~TCookedAnalyser()
{ 
  delete cookedTree;
  delete metaTree;
  delete inFile;
}

int TCookedAnalyser::GetEntry(int entry)
{
// Read contents of entry.
   if (!cookedTree) return 0;
   return cookedTree->GetEntry(entry);
}

void TCookedAnalyser::SetTestMode(int user_nentries = 1000000){

  nentries = user_nentries;  
  printf("\n Warning: \n ");
  printf("  nentries set to %d for testing \n",nentries);
  
}

void TCookedAnalyser::Init()
{

  InitMeta();
  InitCooked();
  
  SetStyle();  
  InitCanvas();
  
}

void TCookedAnalyser::InitMeta(){
  
  printf("\n ------------------------------ \n");
  printf("\n Initialising Meta Data \n");
  printf("\n   %s \n",GetMetaTreeID().c_str());

  inFile->GetObject(GetMetaTreeID().c_str(),metaTree);
  
  if (!metaTree){
    fprintf( stderr, "\n Error: no meta tree  \n ");
    return;
  }

  TBranch * b_SampFreq  = 0;
  TBranch * b_NSamples  = 0;
  TBranch * b_NADCBins  = 0;
  TBranch * b_Range_V   = 0;
  TBranch * b_nsPerSamp = 0;
  TBranch * b_mVPerBin  = 0;
  TBranch * b_Length_ns = 0;
  //  TBranch * b_FileID    = 0;

  metaTree->SetBranchAddress("SampFreq",&SampFreq,&b_SampFreq);
  metaTree->SetBranchAddress("NSamples",&NSamples,&b_NSamples);
  metaTree->SetBranchAddress("NADCBins",&NADCBins,&b_NADCBins);
  metaTree->SetBranchAddress("Range_V",&Range_V,&b_Range_V);
  metaTree->SetBranchAddress("nsPerSamp",&nsPerSamp,&b_nsPerSamp);
  metaTree->SetBranchAddress("mVPerBin",&mVPerBin,&b_mVPerBin);
  metaTree->SetBranchAddress("Length_ns",&Length_ns,&b_Length_ns);
  //metaTree->SetBranchAddress("FileID",FileID_char,&b_FileID);
  
  //sprintf(FileID,"%s",FileID_char);

  metaTree->GetEntry(0);
  

  printf("\n ------------------------------ \n");

}

void TCookedAnalyser::InitCooked(){
  
  inFile->GetObject(GetCookedTreeID().c_str(),cookedTree);
  
  if (cookedTree == 0){
    fprintf( stderr, "\n Warning: No cooked data tree");
  }
  
  printf("\n ------------------------------ \n");
  printf("\n Initialising Cooked Data \n");
  printf("\n   %s \n",GetCookedTreeID().c_str());
  
  if (!cookedTree){
    fprintf( stderr, "\n Error: no cooked tree  \n ");
    return;
  }

  cookedTree->SetMakeClass(1);
  
  cookedTree->SetBranchAddress("wave_mV",&wave_mV, &b_wave_mV);

  cookedTree->SetBranchAddress("min_mV",&min_mV, &b_min_mV);
  cookedTree->SetBranchAddress("max_mV",&max_mV, &b_max_mV);
  cookedTree->SetBranchAddress("ppV_mV",&ppV_mV, &b_ppV_mV);
  cookedTree->SetBranchAddress("mean_mV",&mean_mV, &b_mean_mV);
  cookedTree->SetBranchAddress("peak_samp",&peak_samp, &b_peak_samp);
  
  nentries64_t = cookedTree->GetEntriesFast();
  
  if( nentries64_t > INT_MAX ){
      fprintf(stderr,
	      "\n Error, nentries = (%lld) > INT_MAX unsupported \n ",
	      nentries64_t);
      return;
  }
  else
    nentries = (int)nentries64_t;
  
  
  printf("\n ------------------------------ \n");
  
  return;
}


#endif // #ifdef TCookedAnalyser_cxx
