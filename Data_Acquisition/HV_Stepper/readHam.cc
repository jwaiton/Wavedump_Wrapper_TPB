#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ostream>
#include <cmath>
#include <utility>

#include "VoltageSteps.h"
// command line parsing
#include "cxxopts.hpp" 

void createTree(std::string filename, PMTs& data);
void ShowOpts();
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

  std::cout << result["i"].as<std::string>() << std::endl;
  /*
  if (argc == 1) {
    ShowOpts();
    return -1;
  }*/
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
  VoltageSteps VoltageStep;
  VoltageStep.createHVScanFile(outputfile,data,flatGain,recreate);
  
  // making a tree
  //createTree("PMTData.root",data);
  
  return 1;

}//main

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

void ShowOpts() {
  //Shows options if no input is entered
  std::printf("readHam: Reader of Hamamatsu PMT's.\n");
  std::printf("./readHam <options> [Arguments]\n");
  std::printf("Options:\n");
  std::printf("i, input: input filename. Default: PMTdata.csv\n");
  std::printf("o, output: The output filename. Default: HVScan-new.txt\n");
  std::printf("r, recreate: The output file is recreated.\n");
  std::printf("g, flatgain: The solution will create steps flat in gain.\n");
}

