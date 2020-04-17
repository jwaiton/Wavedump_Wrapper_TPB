/*****************************************************
 * A program to access shipping data 
 * using the ShippingData class
 *
 * Author 
 *  gary.smith@ed.ac.uk
 *  19 10 2019
 *
 * Purpose
 *  This program reads from ShippingData.root
 *  (which can be created from a spreadsheet
 *  using CSVToRoot.C)
 *
 * Setting Up
 *   The environment should first be set using 
 *   the WM.sh script - located in ../../
 * 
 * How to build
 *  $ make
 * 
 *  OR 
 *  
 *  $ make -f ./Build_Options/Makefile_clang++ 
 *
 * How to run 
 * $ ./shipping_data PMT <data>
 * 
 * Input
 *  ShippingData.root - located in Common_Tools
 * 
 * Output
 *  printed output of value
 * 
 * Dependencies
 *  root.cern - a working version of root is required
 *
 *  ShippingData class
 */ 

#include <iostream>
#include <string>
#include "ShippingData.h"

using namespace std;

void Welcome();

int main(int argc, char * argv[]){
  
  char   chrPMT[20];//, chrDat;
  string strPMT, strDat;
  int    pmt = 130;
  
  if( argc == 1 ){
      Welcome();
    printf("\n Which PMT? ");
    scanf("%s",chrPMT);    
    strPMT = chrPMT;
  }
  else if( argc == 2 ){
    Welcome();
    strPMT = argv[1];
  }
  //else if( argc == 3 ){
  //strPMT = argv[1];
  //strDat = argv[2];
  //chrDat = strDat[0];
  //}    
    
  pmt = stoi(strPMT);

  ShippingData * ship_data = new ShippingData(pmt);

  return 1;
}

void Welcome(){

  printf("\n      --------------------    ");
  printf("\n      --------------------  \n");
  printf("\n         shipping_data      \n");
  printf("\n      --------------------    ");
  printf("\n      --------------------  \n");
  printf("\n ------------------------------ \n");

  
}
