#include <iostream>
#include "TRandom3.h"

using namespace std;

float getRandy(int nPeaks){
  
  TRandom3 * rand3 = new TRandom3(0);
  
  return (Int_t)rand3->Uniform(nPeaks);;
}


void testRandom(){

  TRandom3 * rand3 = new TRandom3();
  cout << " random = " <<  (Int_t)rand3->Uniform(5) << endl; 
  cout << " random = " <<  (Int_t)rand3->Uniform(5) << endl; 
  cout << " random = " <<  (Int_t)rand3->Uniform(5) << endl; 
  cout << " random = " <<  (Int_t)rand3->Uniform(5) << endl; 
  cout << " random = " <<  (Int_t)rand3->Uniform(5) << endl; 
  
  cout << endl;
  
  cout << " random = " <<  getRandy(5) << endl; 
  cout << " random = " <<  getRandy(5) << endl; 
  cout << " random = " <<  getRandy(5) << endl; 
  cout << " random = " <<  getRandy(5) << endl; 
  cout << " random = " <<  getRandy(5) << endl; 
  
  return;
}
