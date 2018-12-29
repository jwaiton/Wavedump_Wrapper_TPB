#define PMTAnalyser_cxx
#include "PMTAnalyser.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include "TBranch.h"

void PMTAnalyser::Loop()
{
   if (fChain == 0) return;

   int verbosity = 0;
   
   Long64_t nbytes = 0, nb = 0, ientry;
   
   Float_t baselines[4] = {0.,0.,0.,0.};
   Short_t eventBaseline = 0;
   Short_t signalPeak = 0;
   
   Long64_t nentries = fChain->GetEntriesFast();
   Long64_t nDark    = 0 ;
   //   nentries = 10;
   
   for (Long64_t jentry=0; jentry < nentries; jentry++) {
     
     ientry = LoadTree(jentry);
     
     if (ientry < 0) break;
     nb = fChain->GetEntry(jentry);   
     nbytes += nb;

     //CalculateDarkRate()
     
     peakTime = minT * nsPerSample; 
     
     //cout <<  " peakTime = " << peakTime << endl;
     
     eventBaseline = 0;
     signalPeak = pulse[minT]*mVPerBin;
     
     for( int i = 0 ; i < 4 ; i++ )
       baselines[i] = 0;
     
     for(int iSample = 0 ; iSample < NSamples; iSample++){
       
       milliVolts = pulse[iSample]*mVPerBin;
       
//        cout << " iSample    = " << iSample    << endl;
//        cout << " milliVolts = " << milliVolts << endl;
       
       if( (iSample/25) < 4 )
	 baselines[iSample/25] += milliVolts;
       
       if( milliVolts < signalPeak )
	 signalPeak = milliVolts;
       
     }
     
     for( int i = 0 ; i < 4 ; i++ )
       baselines[i] = baselines[i] / 25.;
     
     if( peakTime > 60.  ) 
       eventBaseline = baselines[3];
     else
       eventBaseline = baselines[2];
     
     signalPeak = -1 * ( signalPeak - eventBaseline) ;
     
     if( signalPeak > 10 ){
       nDark++;

       if(verbosity > 0){
	 cout << endl;
	 cout << " jentry        = " << jentry        << endl;
	 cout << " eventBaseline = " << eventBaseline << endl;
	 cout << " signalPeak    = " << signalPeak    << endl;
	 cout << " peakTime      = " << peakTime      << endl;
       }
     }
     
//      if(  )
//      AccumulateDarkRate
          
     // Show();
     // if (Cut(ientry) < 0) continue;
   }
   
   Float_t darkRate = (Float_t)nDark/nentries/220.*1.0e9;
   cout << endl;
   cout << " nentries = " << nentries << endl;
   cout << " nDark    = " << nDark    << endl;
   cout << " rate     = " << darkRate << endl;
}
