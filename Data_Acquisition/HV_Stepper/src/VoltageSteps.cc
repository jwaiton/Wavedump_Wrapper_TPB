#include "VoltageSteps.h"

/*
VoltageSteps::VoltageSteps(){
    //constructor, just to see if it works.
}
*/
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

void VoltageSteps::createHVScanFile(std::string filename, PMTs& data, 
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
