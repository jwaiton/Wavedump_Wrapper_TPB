#ifndef _PMTPARSE_H
#define _PMTPARSE_H

#include "csvparser.h"
#include "PMTData.h"
#include <vector>
#include <string>
#include<iostream>
#include<fstream>
#include<sstream>
#include<ostream>

/*** Helper functions to parse the PMT data file and create PMTData objects ***/


// helper to check line is PMT data
bool dataLine(std::string& line);

// read file into lines
void readdata(std::string file, std::vector<std::string>& lines);


// get the PMT data
typedef std::vector<PMTData> PMTs;
void fillPMTData(std::string file, PMTs& vec);

#endif
