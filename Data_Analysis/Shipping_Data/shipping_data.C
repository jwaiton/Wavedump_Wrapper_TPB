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
#include <ctype.h>
#include <string>
#include "ShippingData.h"

using namespace std;

void Welcome();
void GetData(int argc, char * argv[]);
ShippingData * MakeShipData(int pmt);
void DeleteShipData(ShippingData * ship_data);
void HelpFunction();

int main(int argc, char * argv[]){

  char   chrPMT[20];//, chrDat;
  string strPMT, strDat;
  int    pmt = 130;

  //Some scenarios under which code won't work properly
  if (argc == 3){
    //If flags are entered, use flags
    if (argc < 3) HelpFunction(); //make sure a pmt number is entered
    else GetData(argc, argv); //get data using the flags

  }
  else if (argc == 2){
    //assume that a PMT has been entered
    pmt = atoi(argv[1]);
    //TODO: print all the shipping data for the value
    ShippingData *ship_data = MakeShipData(pmt);
    DeleteShipData(ship_data);
  }
  else HelpFunction();
  

/*
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
*/
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

void GetData(int argc, char *argv[]){
  int c; 
  int pmt = atoi(argv[2]);//pmt should be last option
  int i = 0; //counts how many times the loop executes
  //TODO: Add a check to make sure the above option is valid.
  ShippingData *ship_data = MakeShipData(pmt);

  while ((c = getopt(argc, argv, "(kbsIedtphaT):")) != -1) {
    switch (c) {
      case 'k':
        //Sk:
        printf("Sk: %fuA/Im\n", ship_data->GetSk());
        break;
      case 'b':
        //Skb
        printf("Skb: %f\n", ship_data->GetSkb());
        break;
      case 's':
        //Sp
        printf("Sp: %fA/Im\n", ship_data->GetSp());
      case 'I':
        //Idb 
        printf("Idb: %fnA\n", ship_data->GetIdb());
        break;
      case 'e':
        printf("EBB: %dV\n", ship_data->GetEBB());
        //nominal voltage?
        break;
      case 'd':
        printf("Dark Count: %dcps \n", ship_data->GetDR());
        break;
      case 't':
        printf("TTS: %fns\n", ship_data->GetTTS());
        break;
      case 'p':
        printf("PTV: %f\n", ship_data->GetPTV());
      case 'a':
        //Prints all of the above
        printf("Sk: %fuA/Im\n", ship_data->GetSk());
        printf("Skb: %f\n", ship_data->GetSkb());
        printf("Sp: %fA/Im\n", ship_data->GetSp());
        printf("Idb: %fnA\n", ship_data->GetIdb());
        printf("EBB: %dV\n", ship_data->GetEBB());
        printf("DR: %dcps \n", ship_data->GetDR());
        printf("TTS: %fns\n", ship_data->GetTTS());
        printf("PTV: %f\n", ship_data->GetPTV());
        break;
      /*
      case 'T':
        //Test case
        printf("PMT number: %f\n", MakeShipData(pmt)->GetPMT());
        break;
      */
      case 'h':
        printf("Help I need an adult \n");
        //TODO: Write a function to display help options
      case '?': //unknown input
        HelpFunction();
        break;
      default:
        printf("Aborting...\n");
        abort();
    }//exit switch 
    i += 1;
  }//exit while loop
  if (i == 0) HelpFunction();
  DeleteShipData(ship_data);
}
int WhichPMT(){
  char cPMT[20];
  string sPMT;
  printf("Which PMT?");
  scanf("%s", cPMT);
  sPMT = cPMT;
  //TODO: Validate Input
  return stoi(sPMT);
}
ShippingData * MakeShipData(int pmt){
  ShippingData *ship_data = new ShippingData(pmt);
  return ship_data;
}
void DeleteShipData(ShippingData * ship_data) {
  delete ship_data;
}

void HelpFunction(){
  /*
  HelpFunction prints a brief description of how to use the program
  TODO: Add units
  */
  printf("Usage:\n");
  printf("shipping_data <options> [pmt number]\n\n");
  printf("Options:\n");
  printf("-h: Prints this message.\n");
  printf("-a: Prints all shipping data for a single PMT.\n");
  printf("-b: Prints the Skb value for a single PMT. \n");
  printf("-d: Prints the Dark Count for a single PMT. \n");
  printf("-e: Prints the EBB value for a single PMT. \n");
  printf("-I: Prints the Idb value for a single PMT. \n");
  printf("-k: Prints the Sk value for a single PMT. \n");
  printf("-p: Prints the Peak-To-Valley ratio for a");
  printf("single PMT. \n");
  printf("-s: Prints the Sp value for a single PMT. \n");
}