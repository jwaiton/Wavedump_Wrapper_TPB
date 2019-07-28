#define TConvert_cxx
#include "TConvert.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void TConvert::PreLoop(){
  
  printf("\n %d \t N samples\n",fNSamples);
  printf("\n %d \t board ID \n",HEAD[1]);
  printf("\n %d \t pattern  \n",HEAD[2]);
  printf("\n %d \t channel  \n",HEAD[3]);
  
}


void TConvert::PostLoop(){
  
  h2->Draw("colz");
  h2->SaveAs("./h2.pdf");
}


void TConvert::Loop()
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

int TConvert::Get_peakSample(short ADC[]){
  
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

float TConvert::Get_peakT_ns(short ADC[]){
  
  return 2.0*Get_peakSample(ADC);

} 

// private
void TConvert::SetDigitiser(char digitiser){

  if(digitiser=='V' || 
     digitiser=='D')
    fDigitiser = digitiser;
  else{
    fprintf( stderr, "\n Error: unknown digitiser \n ");
    fprintf( stderr, "\n Setting to default ('V')  \n ");
    fDigitiser = 'V';
  }
  
  return;
}

void TConvert::SetSampSet(char sampSet){
  
  if  (fDigitiser=='V')
    fSampSet = 'V';
  else
    fSampSet = sampSet;
  
  return;
}

void TConvert::SetPulsePol(char pulsePol){
  
  if(pulsePol == 'N' || 
     pulsePol == 'P')
    fPulsePol = pulsePol;
  else{
    fprintf( stderr, "\n Error: unknown pulse polarity \n ");
    fprintf( stderr, "\n Setting to default ('N')  \n ");
    fPulsePol = 'N';
  }
  
  return;
}

void TConvert::SetConstants(){

  printf("\n Setting Constants \n");
  
  fSampFreq = SetSampleFreq();
  fNSamples = SetNSamples();
  fNADCBins = SetNADCBins();
  fRange_mV = SetRange_mV();
  
  // dependent on above
  f_nsPerSamp = Set_nsPerSamp();
  f_mvPerBin  = Set_mVPerBin();
  fLength_ns  = SetLength_ns();
}

void TConvert::PrintConstants(){ 

  printf("\n \t fDigitiser  = %c \n",fDigitiser);
  if(fDigitiser=='D')
    printf("\n \t fSampSet    = %c \n",fSampSet);
  printf("\n \t fPulsePol   = %c \n",fPulsePol);
  printf("\n \t fSampFreq   = %d \n",fSampFreq);
  printf("\n \t fNSamples   = %d \n",fNSamples);
  printf("\n \t fNADCBins   = %d \n",fNADCBins);
  printf("\n \t fRange_mV   = %d \n",fRange_mV);
  printf("\n \t f_nsPerSamp = %.1f \n",f_nsPerSamp);
  printf("\n \t f_mvPerBin  = %.4f \n",f_mvPerBin);
  printf("\n \t fLength_ns  = %.1f \n",fLength_ns);

}

short TConvert::SetSampleFreq(){
  
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

float TConvert::Set_nsPerSamp(){
  return 1000./fSampFreq;
}

short TConvert::SetNSamples(){
  
  fChain->GetEntry(0);   
  
  short hdrByts = 24;
  short smpByts = HEAD[0] - hdrByts;

  if(fDigitiser=='V')
    return smpByts/2; // shorts
  else
    return smpByts;   // ints
}

float TConvert::SetLength_ns(){
  return f_nsPerSamp*fNSamples;
}

int TConvert::SetNADCBins(){
  
  if( fDigitiser == 'D' )
    return 4096;
  else   
    return 16384;
}

short TConvert::SetRange_mV(){

  if(fDigitiser=='V')
    return 2000;
  else
    return 1000;
  
}

float TConvert::Set_mVPerBin(){
  
  return (float)fRange_mV/fNADCBins;
  
}
