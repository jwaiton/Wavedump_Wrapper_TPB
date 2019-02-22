/*****************************************************
 * A program to filter
 *
 * Author 
 *  Steve Quillin
 *
 * Translated for use in wavedump wrapper 
 *  Gary Smith 22 02 19 

 * Purpose
 *  Filter unwanted frequency components from waveforms
 *
 * How to build
 *  $ make 
 *
 * How to run
 *
 * $ ./
 *
 * Dependencies
 *  root.cern
 *
 * Known issues
 *   
 * Description
 *
 *  Smallest non-zero frequency component in bin #2 
 *  at a frequency of  1/110 samples * 2ns/sample ~ 4.55 MHz
 *  bins 2 - 54 hold magnitudes at multiples of this frequency
 *  bin 55  Nyquist frequency component at 250 MHz = 1/2*2ns  
 *
 *  Note that the magnitude of the zero frequency FFT bin is 
 *  a measure of the trace baseline value (multiplied by the 
 *  number of samples)
 *  Propose using only data traces where the peak FFT component 
 *  falls in the lowest non-zero bin (bin #2)
*/

#include "TFile.h"
#include "TString.h"

int FrequencyFilter(TH1D* Wave){
  
  int    recordlength  = 110;
  double timepersample = 2.; //in nanoseconds
  double aoff          = 8700.;
  double themaximumvalue = 0.;
  
  TH1D*  Wave = new TH1D("Wave","Waveform;Time /ns;ADC counts",
			 recordlength, 0.,
			 recordlength*timepersample);
  
  //
  TH1F* ffttemplate = new TH1F("ffttemplate","ffttemplate",
			       recordlength , 0,recordlength ); 
  //
  TH1D* maxval = new TH1D("maxval","max signal; maxADC counts;counts",
			  1000, 200.,1200.); 
  
  //
  TH1D* maxvalfiltered = new TH1D("maxvalfiltered",
				  "max signal filtered;maxADC counts;counts",
				  1000, 200.,1200.);
  
  string wholename = "thepath/wave_O.dat";
  
  std::ifstream f1(wholename.c_str() , std::ios_base::binary);  
  
  // while(f1.is_open() && f1.good() && !fl.eof() && counter < maxcounts){ 
//     counter++;
//     for(int ii=0;ii<nheaders;ii++){
//                    f1.read((char*)&dummyi, sizeof(int));
//            }
//            for(int ii=0;ii<recordlength;ii++){
//                   fi. read((char*)&dummys, sizeof(short));
//                   double flip_signal = ((double)dummys-aoff)*-1.; 
//                   Wave->SetBinContent(ii+1,flip_signal);
//            }
  
  double themaximumvalue = Wave->GetMaximum() ; 
  maxval->Fill(themaximumvalue);
  
  // compute the FFT and store the magnitude in the 
  // ffttemplate histogram
  Wave ->FFT(ffttemplate ,"MAG");
  
  // a measure of the trace baseline
  double fftoff=ffttemplate->GetBinContent(1)/(double)recordlength; 
  
  //  delete zero frequency data
  ffttemplate->SetBinContent(1,0.) ;
  int maxbin=ffttemplate->GetMaximumBin();
  
  // try to filter out the traces corrupted by ringing
  if( maxbin == 2){  
    maxvalfiltered->Fill(themaximumvalue);
    
  }
  //f1.close();
  
}
