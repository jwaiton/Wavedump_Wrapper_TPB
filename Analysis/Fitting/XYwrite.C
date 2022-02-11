#include <iostream>

#include "TFile.h"
#include "TH1F.h"
#include "TF1.h"
#include "TLine.h"

#include "XYanalysis.C"
#include <fstream>

#include <TLatex.h>

	// pathToWrite is where you want to put the file it will write
void XYwrite(TString rootFileName = "hQ_Fixed_Run_23_PMT_162_Loc_9_Test_N.root", string pathToData = "./",string pathToWrite ="./"){
		// Collect data
	Data * data = XYanalysis(rootFileName, pathToData);
		// Write data
	std::ofstream xy_results;
	xy_results.open ("xy_results.txt");
	xy_results << "File: " << rootFileName << "\n";
	xy_results << "P:V Ratio = " << data->peakToValley.value << " (" << data->peakToValley.error << ") \n";
	xy_results << "Efficiency = " << data->efficiency.value << " (" << data->efficiency.error << ") \n";
	xy_results << "mu = " << data->mu.value << " (" << data->mu.error << ") \n";
	xy_results << "gain = " << data->gain.value << "e+07" << " (" << data->gain.error  << ") \n";
	xy_results.close();


}

