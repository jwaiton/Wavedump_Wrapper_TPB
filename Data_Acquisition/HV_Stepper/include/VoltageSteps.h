#ifndef _VOLTAGE_STEPS_H_
#define _VOLTAGE_STEPS_H_

#include "PMT.h"

#include <cmath>
#include <vector>
#include <utility>
#include <string>
#include <fstream>

//root stuff
#include "TTree.h"
#include "TFile.h"
#include "TMath.h"

class VoltageSteps {
private:
  
  const double alpha = 6.9;
  const double minGain = 0.3;
  const double maxGain = 2;
  const double nStep = 5;
  std::vector<PMT> PMTs;
  //functions
  double toHV(double gain, double v);
  std::pair<double,double> hvRange(double vnom);
  int INTV(double val);
  void hvSteps(std::pair<double,double> hv,std::vector<int>& voltages);
  void gainSteps(std::vector<double>& gains);
  void gainsToHV(std::vector<int>& voltages, double v);
  bool fExists(std::string &filename);
  bool askUser(std::string &filename);

public:
  void createHVScanFile(std::string filename,
   bool flatGain,bool recreate);
  VoltageSteps(std::vector<PMT> &data) : PMTs(data) {}
  //TODO: Create the tree in the constructor.

};
#endif