#include "./include/PMTParser.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ostream>
#include <cmath>
#include <utility>

#include "TMath.h"

// command line parsing
#include "./include/cxxopts.hpp" 
//./include/... is a temporary way of getting some file organisation.



const double alpha = 6.9;
const double minGain = 0.3;
const double maxGain = 2;

double toHV(double gain, double v){
  return v*std::pow(gain, 1/alpha); 
}

std::pair<double,double> hvRange(double vnom){

  double minHV = toHV(minGain,vnom); 
  double maxHV = toHV(maxGain,vnom); 
  maxHV  = std::min(2000.,maxHV); // PMT only goes to 2000
  return std::make_pair(minHV,maxHV);
}

int INTV(double val){
  // convert double voltage to int
  return 10*TMath::Nint<double>(val/10.);
}

const double nStep = 5;

void hvSteps(std::pair<double,double> hv,std::vector<int>& voltages){

  // hv steps flat in HV
  double stepSize = (hv.second -hv.first)/float(nStep - 1);
  for (int i = 0; i < nStep; ++i){
    int v = INTV(hv.first + stepSize*i);
    voltages.push_back(v);
  }
}

void gainSteps(std::vector<double>& gains){
  double stepSize = (maxGain -minGain)/float(nStep - 1);
  for (int i = 0; i < nStep; ++i){
    gains.push_back(minGain + stepSize*i);
  }
}

void gainsToHV(std::vector<int>& voltages, double v){

  std::vector<double> gains;
  gainSteps(gains);
  for (auto step: gains){
    voltages.push_back(INTV(toHV(step,v)));
  }
}


void createHVScanFile(std::string filename, PMTs& data, bool flatGain,bool recreate){

  std::ofstream myfile;
  if (recreate){
    myfile.open(filename, std::ios_base::out);
  }
  else {
     myfile.open(filename,std::ios_base::app | std::ios_base::out);
  }
  
  for (auto pmt: data){

    std::vector<int> voltage;
    if (flatGain == false){
      // flat HV
      std::pair<double,double> hv = hvRange(pmt.workingVoltage());
      hvSteps(hv,voltage);
    }
    else {
      gainsToHV(voltage,pmt.workingVoltage());      
    }
    // nominal
    voltage.push_back(int(pmt.workingVoltage()));
    
    std::stringstream stream;
    stream << pmt.serial();
    for (auto i = 0; i< voltage.size();++i){
      stream << "," << voltage[i];
    }
    stream << std::endl;
    myfile << stream.str();
  }
  myfile.close();
}

#include "TTree.h"
#include "TFile.h"

void createTree(std::string filename, PMTs& data){

  TFile* file = new TFile(filename.c_str(),"RECREATE");
  TTree* tree = new TTree("PMTData","PMTData");
  int id; TBranch* branch_id = tree->Branch("id",&id , "id/I");
  float peakToValley; TBranch* branch_sv = tree->Branch("peakToValley",&peakToValley , "peakToValley/F");
  float tts; TBranch* branch_tts = tree->Branch("tts",&tts , "tts/F");
  float dc; TBranch* branch_dark = tree->Branch("darkCount",&dc , "darkCount/F");
  float wv; TBranch* branch_wv = tree->Branch("workingVoltage",&wv , "workingVoltage/F");
  float idb; TBranch* branch_idb = tree->Branch("idb",&idb , "idb/F");
  float sp; TBranch* branch_sp = tree->Branch("sp",&sp , "sp/F");
  float skb; TBranch* branch_skb = tree->Branch("skb",&skb , "skb/F");
  float sk; TBranch* branch_sk = tree->Branch("sk",&sk , "sk/F");

  for (auto pmt: data){
    id = std::stoi(pmt.serial().substr(2));
    peakToValley = pmt.peakToValley();
    tts = pmt.tts();
    dc = pmt.darkCount();
    wv = pmt.workingVoltage();
    idb = pmt.idb();
    sp = pmt.sp();
    skb = pmt.skb();
    sk = pmt.sk();
    tree->Fill();
  }
  tree->Write();
  file->Close();
}


/* g++ -std=c++11  -I/Users/needham/cern/root-build/include readHam.cc -o readHam -L/Users/needham/cern/root-build/lib -Wl,-rpath,/Users/needham/cern/root-build/lib -lCore -lRIO -lRooFit -lRooFitCore -lRooStats -lHist -lTree -lMatrix -lPhysics -lMathCore */



int main(int argc, char* argv[]){

  // set up command line parser
  cxxopts::Options options("readHam", "Reader of Hammatsu files");
  options.add_options()
    ("i,input", "Input File name", cxxopts::value<std::string>()->default_value("PMTdata.csv"))
    ("o,output","Output File name", cxxopts::value<std::string>()->default_value("HVScan-new.txt"))
    ("r,recreate","recreate", cxxopts::value<bool>()->default_value("false"))
    ("g,flatGain","flat gain", cxxopts::value<bool>()->default_value("false"))
    ;
  auto result = options.parse(argc, argv);

  // get the options we parse
  std::string inputfile =   result["i"].as<std::string>();
  std::string outputfile =  result["o"].as<std::string>();
  bool flatGain = result["g"].as<bool>();
  bool recreate=  result["r"].as<bool>();
  
  PMTs data;
  fillPMTData(inputfile,data);

  // example of printing out
  //  for (auto pmt : data){
  //  std::cout << pmt << std::endl;
  // }
  
  // making a scan file
  createHVScanFile(outputfile,data,flatGain,recreate);
  
  // making a tree
  //createTree("PMTData.root",data);
  
  return 1;

}
