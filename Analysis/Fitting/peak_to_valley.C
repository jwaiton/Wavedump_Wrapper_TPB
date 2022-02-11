#include <iostream>
#include <string>

#include "mu_From_Hist.h"

using namespace std;

int main(int argc, char * argv[]){

  float mu = 0.;
  
  mu = mu_From_Histo("hQ_Fixed_Run_1_PMT_131_Loc_1_Test_N", "./");
  
  cout << " hello world " << endl;

}
