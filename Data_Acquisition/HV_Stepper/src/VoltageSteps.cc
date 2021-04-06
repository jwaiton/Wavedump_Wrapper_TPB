#include "VoltageSteps.h"

double VoltageSteps::toHV(double gain, double v){
  return v*std::pow(gain, 1/alpha); 
}

std::pair<double,double> VoltageSteps::hvRange(double vnom){
  double minHV = toHV(minGain,vnom); 
  double maxHV = toHV(maxGain,vnom); 
  maxHV  = std::min(2000.,maxHV); // PMT only goes to 2000
  return std::make_pair(minHV,maxHV);
}

int VoltageSteps::INTV(double val) {
    return 10*TMath::Nint<double>(val/10.);
}

void VoltageSteps::hvSteps(std::pair<double,double> hv, std::vector<int>&voltages){
  // create hv steps flat in HV
  double stepSize = (hv.second -hv.first)/float(nStep - 1);
  for (int i = 0; i < nStep; ++i){
    int v = INTV(hv.first + stepSize*i);
    voltages.push_back(v);
  }
}

void VoltageSteps::gainSteps(std::vector<double>& gains){
  double stepSize = (maxGain -minGain)/float(nStep - 1);
  for (int i = 0; i < nStep; ++i){
    gains.push_back(minGain + stepSize*i);
  }
}

void VoltageSteps::gainsToHV(std::vector<int>& voltages, double v){

  std::vector<double> gains;
  gainSteps(gains);
  for (auto step: gains){
    voltages.push_back(INTV(toHV(step,v)));
  }
}

bool VoltageSteps::fExists(std::string &filename) {
  return (access(filename.c_str(), F_OK ) != -1);
}

bool VoltageSteps::askUser(std::string &filename) {
  //check if user wants to overwrite file.
  std::string response;
  bool repeat = true;
  bool ok;
  std::cout << "\"" << filename << "\" already exists" << std::endl;
  std::cout << "Would you like to recreate it? (y/n): " << std::endl;

  do {
    std::cin >> response;
    if (response == 'y' || response == 'Y') {
      ok = true;
      repeat = false;
    }
    else if (response == 'n' || response == 'N') {
      ok = false;
      repeat = false;
    }
    else {std::cout << "Invalid response. (y/n): " << std::endl;}
  } while(repeat);

  return ok;
}
void VoltageSteps::createHVScanFile(std::string filename,
  bool flatGain,bool recreate){

  /* createHVScanFile
  *  Creates voltage settings for each PMT, flat in either gain
  *  or HV.
  *  Results can be written to file. If the recreate option is
  *  specified, then the original file is overwritten.
  * */
  std::ofstream myfile;
  if (recreate){
    myfile.open(filename, std::ios_base::out);
  }
  else if (fExists(filename)) {
    if (askUser(filename)) myfile.open(filename, std::ios_base::out);
    }
  else {
     myfile.open(filename,std::ios_base::app | std::ios_base::out);
  }
  
  //write column titles to file
  myfile << "Serial,gainV0,gainV1,gainV2,gainV3,gainv4,nomV\n";

  for (auto pmt: PMTs){

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
    for (auto v: voltage){
      stream << "," << v;
    }
    stream << std::endl;
    myfile << stream.str();
  }
  myfile.close();
}
