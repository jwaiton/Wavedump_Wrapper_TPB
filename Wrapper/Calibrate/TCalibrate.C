#define TCalibrate_cxx
#include "TCalibrate.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>


void TCalibrate::PreLoop(){
  
  printf("\n %d \t N samples\n",fNSamples);
  printf("\n %d \t board ID \n",HEAD[1]);
  printf("\n %d \t pattern  \n",HEAD[2]);
  printf("\n %d \t channel  \n",HEAD[3]);
  
}


void TCalibrate::PostLoop(){
  
  h2->Draw("colz");
  h2->SaveAs("./h2.pdf");
}


void TCalibrate::Loop()
{
  if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      
      trigTimeTag = 4.0*(1.0 + (float)HEAD[5]/4294967295);
      
      //  printf("\n peak time   = %f \n", Get_peakT_ns(ADC));
      //printf("\n trigTimeTag = %f \n", trigTimeTag);

      h2->Fill(trigTimeTag,Get_peakT_ns(ADC));
      
      //if(jentry == 1000)
      //return;

      // if (Cut(ientry) < 0) continue;
   }
}

int TCalibrate::Get_peakSample(short ADC[]){
  
  int   peakSample = 0;
  short peakADC    = 32767;
    
  for ( int i = 0 ; i < fNSamples ; i++) {
    if(ADC[i] < peakADC){
      peakADC = ADC[i];
      peakSample = i;
    }
  }
  
  //printf("\n peakSample = %d \n ",peakSample);
  
  return peakSample;

}

float TCalibrate::Get_peakT_ns(short ADC[]){
  
  return 2.0*Get_peakSample(ADC);

} 

// private
void TCalibrate::SetDigitiser(char digitiser){
  
  if(digitiser=='V' || 
     digitiser=='D')
    fDigitiser = digitiser;
  else{
    fprintf( stderr, "\n Error: unknown digitiser \n ");
    fprintf( stderr, "\n Setting to default ('V')  \n ");
    fDigitiser = 'V';
  }
}

void TCalibrate::SetSampSet(char sampSet){
  
  if  (fDigitiser=='V')
    fSampSet = 'V';
  else
    fSampSet = sampSet;
}

void TCalibrate::SetConstants(){

  fSampFreq = SetSampleFreq();
  fNSamples = SetNSamples();
  fNADCBins = SetNADCBins();
  fRange_mV = SetRange_mV();
  
  // dependent on above
  f_nsPerSamp = Set_nsPerSamp();
  f_mvPerBin  = Set_mVPerBin();
  fLength_ns  = SetLength_ns();
}

void TCalibrate::PrintConstants(){ 

  printf("\n fDigitiser  = %c \n",fDigitiser);
  if(fDigitiser=='D')
    printf("\n fSampSet    = %c \n",fSampSet);
  printf("\n fSampFreq   = %d \n",fSampFreq);
  printf("\n fNSamples   = %d \n",fNSamples);
  printf("\n fNADCBins   = %d \n",fNADCBins);
  printf("\n fRange_mV   = %d \n",fRange_mV);
  printf("\n f_nsPerSamp = %f \n",f_nsPerSamp);
  printf("\n f_mvPerBin  = %f \n",f_mvPerBin);
  printf("\n fLength_ns  = %f \n",fLength_ns);

}

short TCalibrate::SetSampleFreq(){
  
  if(fDigitiser=='D'){
    switch(fSampSet){
    case '0':
	return 5000;
    case '1':
	return 2500;
    case '2':
	return 1000;
    case '3':
	return 750;
    default:
      return 1000;
    }
  }
  else
    return 500; // 'V'
}

float TCalibrate::Set_nsPerSamp(){
  return 1000./fSampFreq;
}

short TCalibrate::SetNSamples(){
  
  fChain->GetEntry(0);   
  
  short hdrByts = 24;
  short smpByts = HEAD[0] - hdrByts;

  if(fDigitiser=='V')
    return smpByts/2; // shorts
  else
    return smpByts;   // ints
}

float TCalibrate::SetLength_ns(){
  return f_nsPerSamp*fNSamples;
}

int TCalibrate::SetNADCBins(){
  
  if( fDigitiser == 'D' )
    return 4096;
  else   
    return 16384;
}

short TCalibrate::SetRange_mV(){

  if(fDigitiser=='V')
    return 2000;
  else
    return 1000;
  
}

float TCalibrate::Set_mVPerBin(){
  
  return (float)fRange_mV/fNADCBins;
  
}
