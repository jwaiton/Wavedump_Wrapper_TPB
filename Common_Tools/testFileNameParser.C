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

  printf("\n ---------------------------------");
  printf("\n Example 1: default constructor   ");
  printf("\n ---------------------------------");
  
  FileNameParser * fnp1 = new FileNameParser();

  printf("\n ---------------------------------");
  printf("\n Example 2: filename, no option  ");
  printf("\n ---------------------------------");
  
  // ------------------------
  // Usage with file name containing IDs
  string filename = "/path/to/Run_102_PMT_152_Loc_0_Test_S.root";

  // possible usage but does not set class data members 
  // need to choose option > 0
  // option 1 read from filename
  // option 2 user input

  filename = "/path/to/Run_102_PMT_152_Loc_0_Test_G_Step_3.root";
  
  FileNameParser * fnp2 = new FileNameParser(filename);
  
  cout << endl;
  cout << " FileID   = " << fnp2->GetFileID() << endl;
  cout << " dir      = " << fnp2->GetDir() << endl;
  // ------------------------
  // Usage with tree name containing IDs
  printf("\n ---------------------------------");
  printf("\n Example 3: tree name             ");
  printf("\n ---------------------------------");
  
  filename ="Events_Run_102_PMT_152_Loc_0_Test_S";

  FileNameParser * fnp3 = new FileNameParser(filename);
  
  cout << endl;
  cout << " FileID  = " << fnp3->GetFileID() << endl;

  // ------------------------
  // Usage with raw data path
  printf("\n ----------------------------------");
  printf("\n Example 4: Gain binary data path  ");
  printf("\n ----------------------------------");
  
  filename = "/Data_Storage/Binary_Data/RUN1234/PMT0130/Gain/STEP01/wave_0.dat";
  
  // note usage of option = 1 
  FileNameParser * fnp4 = new FileNameParser(filename,1);

  cout << endl;
  cout << " run  = " << fnp4->GetRun() << endl;
  cout << " dir  = " << fnp4->GetDir() << endl;
  cout << " test = " << fnp4->GetTest() << endl;

  // ------------------------
  // Usage with user input
  printf("\n ---------------------------------");
  printf("\n Example 5: binary file           ");
  printf("\n ---------------------------------");
  filename = "wave_0.dat";
  
  // note usage of option = 2 
  FileNameParser * fnp5 = new FileNameParser(filename,2);

  cout << endl;
  cout << " run  = " << fnp5->GetRun() << endl;
  cout << " dir  = " << fnp5->GetDir() << endl;
  cout << " test = " << fnp5->GetTest() << endl;
  
}
