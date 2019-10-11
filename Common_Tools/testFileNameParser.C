#include <string>
#include <cstdlib>
#include <iostream>

using namespace std;

// To run, first do
// $ root
// [0] .L FileNameParser.C+
// then
// [1] .x testFileNameParser.C

void testFileNameParser(){

  // ------------------------
  // Usage with file name containing IDs
  string filename = "Run_102_PMT_152_Loc_0_Test_S.root";

  FileNameParser * fnp1 = new FileNameParser();

  // possible usage but does not set class data members 
  cout << endl;
  cout << " run  = " << fnp1->run(filename) << endl;

  // incorrect usage - need to first give 
  // filename as argument upon creation
  // this will produce and error
  cout << endl;
  cout << " run  = " << fnp1->GetRun() << endl;
  
  // ------------------------
  // Usage with raw data path
  filename = "Data_Storage/Binary_Data/RUN000001/PMT0130/SPEtest/wave_0.dat";
  
  FileNameParser * fnp2 = new FileNameParser(filename,1);

  cout << endl;
  cout << " run  = " << fnp2->GetRun() << endl;
  
}
