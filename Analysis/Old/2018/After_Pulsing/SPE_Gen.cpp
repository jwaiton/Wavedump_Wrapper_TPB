//Project include files
#include "ns.h"

//Standard library include files.
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>

//ROOT include files
#include "TApplication.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TObject.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TLegend.h"
#include "TGraph.h"

using namespace std;

int main(int argc, char **argv)
{
  randomSeedTime();
	
	
  int channel[4]={0,0,0,0};
  char answer;
  char histname[200]= "";
  //int test;
  int Gain[4]={0,0,0,0};
  int totalwaves[4]={0,0,0,0};
	
	
  //Read in the HV data ====================================================================================
  string hvfile = "../HVScan.txt";
  ifstream file(hvfile.c_str());
  string hvdat;
	
  vector<int> PMT_number(125,0), HV(125,0);  //vector<vectorType>vectorName(size);
  vector<vector<int>> HVstep;
  vector<int> step(5,0);
  for (int i=0; i<125; i++)
    HVstep.push_back(step);
	
  for (int i=0; i<125; i++){
    for (int j=0; j<7; j++){
      file >> hvdat;
      int pmt_info =atof(hvdat.c_str());
      if (j==0){
	PMT_number[i]=pmt_info;
      }
      if (j!=0 && j!=6){
	HVstep[i][j-1]=pmt_info;
      }
      if (j==6){
	HV[i]=pmt_info;
      }
      //printf("j %d, val %d \n",j,pmt_info);
    }
  }
  //========================================================================================================
	
  while(answer!='Y'&& answer!='y'){
		
    //Determing the PMT number and the applied Voltage=====================================================
    cout << "Enter the PMT number in Channel 0 \n" ;
    cout << "Note: please neglect the NB and the zeros before the number \n" <<endl;
    cin  >> channel[0]; 
    cout <<endl;
		
    cout << "Input the PMT VOLTAGE in Channel 0 \n" ;
    cout << "ENTER THE VOLTAGE IN VOLTS \n" <<endl;
    cin  >> Gain[0]; 
    cout <<endl;
		
    cout << "Enter the PMT number in Channel 1 \n" ;
    cout << "Note: please neglect the NB and the zeros before the number \n" <<endl;
    cin  >> channel[1]; 
    cout <<endl;
		
    cout << "Input the PMT VOLTAGE in Channel 1 \n" ;
    cout << "ENTER THE VOLTAGE IN VOLTS \n" <<endl;
    cin  >> Gain[1]; 
    cout <<endl;
		
		
    cout << "Enter the PMT number in Channel 2 \n" ;
    cout << "Note: please neglect the NB and the zeros before the number \n" <<endl;
    cin  >> channel[2]; 
    cout <<endl;
		
    cout << "Input the PMT VOLTAGE in Channel 2 \n" ;
    cout << "ENTER THE VOLTAGE IN VOLTS \n" <<endl;
    cin  >> Gain[2]; 
    cout <<endl;
	
    cout << "Enter the PMT number in Channel 3 \n";
    cout << "Note: please neglect the NB and the zeros before the number \n" <<endl;
    cin  >> channel[3]; 
    cout <<endl;
		
    cout << "Input the PMT VOLTAGE in Channel 3 \n" ;
    cout << "ENTER THE VOLTAGE IN VOLTS \n" <<endl;
    cin  >> Gain[3]; 
    cout <<endl;
		
		
    cout <<"Please verifiy the following: "<<endl;
    for (int i=0; i<4; i++){
      if (channel[i]<10)
	sprintf(histname,"NB000%d is in Channel %d Biased at %d Volts \n",channel[i], i, Gain[i]);
			  
      if (channel[i]>=10 && channel[i] <100)
	sprintf(histname,"NB00%d is in Channel %d Biased at %d Volts \n",channel[i],  i, Gain[i]);
			 
      if (channel[i]>=100)
	sprintf(histname,"NB0%d is in Channel %d  Biased at %d Volts \n",channel[i],  i, Gain[i]);
			  
      cout << histname ;
    }
		
    cout <<"Is this correct? (y/n)  "<<endl;
    cin>>answer;
    cout <<answer<<endl;
		
  }
		
  //Create default ROOT application.
  TApplication *ta= new TApplication("ta",&argc,argv);
	
  //Stores one waveform for processing
  TH1D* Wave = new TH1D("Wave","Waveform; Time (us); ADC Counts",5100,0,10.2); //2ns bins, 5100,0.,10.2 for 10.2us TH1D("name","title",nbins_x)
	
  //Timing of the after-pulses
  TH1D **TIME=new TH1D*[4];	
  for (int w=0;w<4;w++){
    sprintf(histname, "TIME%d",w);
    TIME[w] = new TH1D(histname,"After-Pulsing; Time (usec) ; Counts",5100,0.,10.2); //2ns bins, 5100, 0., 10.2us
  }
	
  TH1D* Search = new TH1D("Search", "Search Window; Time (ns); Charge (mV-ns)", 100, 0., 200.); //2ns bins
	
	
  for (int w=0; w<4; w++){
		
    char filename[200]= "";
    //sprintf(filename,"../../BinaryData/PMT%d/APTest/wave_%d.dat",w); //TODO
    if (channel[w]<10)
      sprintf(filename,"/mnt/usb-Seagate_BUP_RD_NA9FWF45-0:0-part1/BinaryData_Backups/RUN000001/APData/PMT000%d/APTest/wave_%d.dat",channel[w],w);

    if (channel[w]>=10 && channel[w] <100)
      sprintf(filename,"/mnt/usb-Seagate_BUP_RD_NA9FWF45-0:0-part1/BinaryData_Backups/RUN000001/APData/PMT00%d/APTest/wave_%d.dat",channel[w],w);

    if (channel[w]>=100)
      sprintf(filename,"/mnt/usb-Seagate_BUP_RD_NA9FWF45-0:0-part1/BinaryData_Backups/RUN000001/APData/PMT0%d/APTest/wave_%d.dat",channel[w],w);

    ifstream fin(filename);

    //================= Reads in the headers and assigns values for things=============
    for (int i=0; i<12; i++){ //11 lines of header data
      //Read in the header for the script
      int header=0.;
      //fin.read((char*)&header,2);
      fin.read((char*)&header,sizeof(int));
      printf("P: %d\n",header);
    }

    //================= Reads in waveforms ==================

    int counter = 0;
    while(fin.is_open()&&fin.good()&&!fin.eof()&&counter<10001){
      counter ++;
		   
		   
      //////if (counter%100==0)
      if (counter%10000==0)
	printf("Waveform Progress: %d out of 10001 events\n",counter);
      //if (counter>3200)
      //		printf("Waveform Progress: %d \n",counter);
      //Records and ind. waveform into
      for (int i=0; i<5112; i++){  // <5112 for 10.2us of data 
	//Read in result.
	/////unsigned short result;
	unsigned short result=0.;
	fin.read((char*)&result,2);
	// if(i<10)
	// printf("P: %d\n",result);
	     if (i<5101){ // 5101 for 10.2us
	  //enact an arbitrary offset in the data to make the peak
	     int aoff = 8700; // 540e6;
	     int flip_signal = (float(result)-aoff)*-1.0;
	     Wave->SetBinContent(i+1,flip_signal);
	     }
      }
      return double wave;
	
		
      //Set up rolling windows of 60 usec =======================================================
      int pastmax = 0;
      // create loops for this background
      for (int i=51; i<5101;i++){ //look for a pulse between 0.1us and 10.2 us (i<5101) 
			
	double background = 0.;
	double pulse = 0.;
			
	double maxBack= 0.;
	int maxBin=0;
			
	for(int j=0; j<30; j++){ 
	  // calculate the total pulse in a 60ns window
	  pulse += Wave->GetBinContent(i+j);
	  // calculate the total background either side of the 60ns pulse window
	  background += Wave->GetBinContent(i+j-30);	 //h->GetBinContent(bin)
	  background += Wave->GetBinContent(i+j+30);	
				
	  //Also finding the max value in the window.
	  if (maxBack < Wave->GetBinContent(i+j)){
				
	    maxBack = Wave->GetBinContent(i+j);
	    maxBin = i+j;
	  }
	}
	//Determing if the charge exceeds 100 mV-ns
	//calculate the difference in ADC counts and convert to mVns
	double ChargeDiff = (pulse-background*0.5)*2/16384.0*2.0e3; 
	//printf ("Charge Difference: %f \n",ChargeDiff);
			
	if (ChargeDiff>100){ // find pulses over 100mVns greater than the background (changed from 200.e4)
	  //create a histogram 'Search' from 100ns before the pulse peak to 100 ns after 	
	  for (int s=-50.;s<50; s++){
	    Search->SetBinContent(s+1, Wave->GetBinContent(maxBin+s+1)); //2ns bins
	  }	
				
	  int binmax = Search->GetMaximumBin()+maxBin; 
	  if (binmax != pastmax){
	    TIME[w]->Fill(Wave->GetXaxis()->GetBinCenter(binmax)); //-30.9); where is this value from?
	    pastmax = binmax;
	  }
				
	}

      }

    }
    totalwaves[w]= counter;
    //closes the wave-dump file
    fin.close();
  }
	
  //Print out total number of waves for the relative quantum efficiency
  for (int i=0; i<4; i++)
    printf("Total Triggers from Wave %d: %d \n", i, totalwaves[i]);
	
	
	
  //Create canvas allowing for window close
  TCanvas *tc0= new TCanvas("Canvas0","ROOT Canvas",1);
  tc0->Connect("TCanvas0","Closed()","TApplication",gApplication, "Terminate()");
  tc0->SetGrid();
	
  TIME[0]->Draw("Same");
  TIME[0]->SetLineColor(kBlue);
	
  TIME[1]->Draw("Same");
  TIME[1]->SetLineColor(kMagenta+2);
	
  TIME[2]->Draw("Same");
  TIME[2]->SetLineColor(kGreen+2);
	
  TIME[3]->Draw("Same");
  TIME[3]->SetLineColor(kCyan+2);
	
	
	
	
	
  for (int i=0;i<4;i++){

    if (channel[i]<10)		
      sprintf(histname, "SPE/PMT_NB000%d_HV%d_AfterPulse.root",channel[i],  Gain[i]);
    if (channel[i]>=10 && channel[i] <100)
      sprintf(histname, "SPE/PMT_NB00%d_HV%d_AfterPulse.root",channel[i], Gain[i]);
    if (channel[i]>=100)
      sprintf(histname, "SPE/PMT_NB0%d_HV%d_AfterPulse.root",channel[i],  Gain[i]);
			
    TIME[i]->SaveAs(histname);
  }
  ta->Run();
	
	
  return 0;
}

