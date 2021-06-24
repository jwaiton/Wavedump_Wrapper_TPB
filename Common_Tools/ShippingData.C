#define ShippingData_cxx
#include "ShippingData.h"

#include <iostream>

#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#include "dark_rate_at_temp.h"

using namespace std;

void ShippingData::PrintAll(){
  
  cout <<  endl; 
  cout << " Shipping Data for PMT " << GetPMT() << endl; 
  cout << "  Nominal Voltage = " << GetEBB() << endl;
  cout << "  Peak To Valley  = " << GetPTV() << endl;
  cout << "  Dark Rate       = " << GetDR()  << endl;
  
}

void ShippingData::SetAll(int inputPMT)
{
   if (fTree == 0) return;

   userPMT = inputPMT;

   Long64_t nentries = fTree->GetEntriesFast();

   for (Long64_t jentry = 0; jentry < nentries ; jentry++ ) {
     Long64_t ientry = LoadTree(jentry);
     if (ientry < 0) break;
     
     fTree->GetEntry(jentry);

     if(userPMT == PMT){
       //Show();
       userSk  = Sk;
       userSkb = Skb;
       userSp  = Sp;
       userIdb = Idb;
       userEBB = EBB;
       userDR  = DR;
       userTTS = TTS;
       userPTV = PTV;
       
       isAllSet = kTRUE;
       
       return;
     }
     
     if( PMT > userPMT || 
	 ientry == (nentries-1)){
       std::cerr << " No Shipping Data for PMT " << userPMT << std::endl;
       break;
     }
   
   }
}

void ShippingData::SetNewPMT(int inputPMT){
  SetAll(inputPMT);
}

Float_t ShippingData::GetSk(){
  return userSk;
}
Float_t ShippingData::GetSkb(){
  return userSkb;
}
Float_t ShippingData::GetSp(){
  return userSp;
}
Float_t ShippingData::GetIdb(){
  return userIdb;
}
Int_t   ShippingData::GetEBB(){
  return userEBB;
}
Int_t   ShippingData::GetDR(){
  return userDR;
}
Int_t   ShippingData::GetDR(float temp){
  return dark_rate_at_temp(userDR,temp);
}
Float_t ShippingData::GetTTS(){
  return userTTS;
}
Float_t ShippingData::GetPTV(){
  return userPTV;
}

Int_t ShippingData::GetPMT(){
  return userPMT;
}
