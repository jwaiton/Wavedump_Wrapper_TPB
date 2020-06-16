#ifndef _VOLTAGE_STEPS_H_
#define _VOLTAGE_STEPS_H_

#include "PMTParser.h"

#include <cmath>
#include <vector>
#include <utility>
#include <string>

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

    //functions
    double toHV(double gain, double v);
    std::pair<double,double> hvRange(double vnom);
    int INTV(double val);
    void hvSteps(std::pair<double,double> hv,std::vector<int>& voltages);
    void gainSteps(std::vector<double>& gains);
    void gainsToHV(std::vector<int>& voltages, double v);

public:
    void createHVScanFile(std::string filename, 
    PMTs& data, bool flatGain,bool recreate);
    void createTree(std::string filename, PMTs& data);
    //VoltageSteps(); Not used
    //TODO: Create the tree in the constructor.

};
#endif