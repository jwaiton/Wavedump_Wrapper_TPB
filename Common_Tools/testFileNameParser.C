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

  printf("\n --------------------");
  printf("\n Example 1: default constructor \n");
  
  // ------------------------
  // Usage with file name containing IDs
  string filename = "/path/to/Run_102_PMT_152_Loc_0_Test_S.root";

  FileNameParser * fnp1 = new FileNameParser();
  
  // possible usage but does not set class data members 
  cout << endl;
  cout << " run  = " << fnp1->run(filename) << endl;

  // incorrect usage - need to first give 
  // filename as argument upon creation
  // this will produce an error
  cout << endl;
  cout << " run  = " << fnp1->GetRun() << endl;
  
  // ------------------------
  // Usage with file name containing IDs
  printf("\n --------------------");
  printf("\n Example 2: file name \n");

  FileNameParser * fnp2 = new FileNameParser(filename);
  
  cout << endl;
  cout << " FileID   = " << fnp2->GetFileID() << endl;
  cout << " dir      = " << fnp2->GetDir() << endl;
  // ------------------------
  // Usage with tree name containing IDs
  printf("\n --------------------");
  printf("\n Example 3: tree name \n");

  filename ="Events_Run_102_PMT_152_Loc_0_Test_S";

  FileNameParser * fnp3 = new FileNameParser(filename);
  
  cout << endl;
  cout << " FileID  = " << fnp3->GetFileID() << endl;

  // ------------------------
  // Usage with raw data path
  printf("\n --------------------");
  printf("\n Example 4: binary data path \n");

  filename = "/Data_Storage/Binary_Data/RUN000001/PMT0130/SPEtest/wave_0.dat";
  
  // note usage of option = 1 
  FileNameParser * fnp4 = new FileNameParser(filename,1);

  cout << endl;
  cout << " run  = " << fnp4->GetRun() << endl;
  cout << " dir  = " << fnp4->GetDir() << endl;
  
}
