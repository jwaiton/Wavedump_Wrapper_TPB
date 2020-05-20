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

#include <unistd.h>

using namespace std;

void GetData(int argc, char * argv[]);
ShippingData * MakeShipData(int pmt);
void DeleteShipData(ShippingData * ship_data);
void HelpFunction();

int main(int argc, char * argv[]){

  int    pmt = 130; //default pmt
  //Decide what to show the user;

  if (argc > 2){
    //If flags are entered, use flags
    if (argc < 3) HelpFunction(); //make sure a pmt number is entered; I've just realised this will never be called.
    else GetData(argc, argv); //get data using the flags

  }
  else if (argc == 2 && ((string)argv[1] == "-h" || (string)argv[1] == "-H")){
    HelpFunction();
  }
  else if (argc == 2){
    pmt = atoi(argv[argc-1]);
    if (pmt == 0) HelpFunction();
    else {
      ShippingData *ship_data = MakeShipData(pmt);
      //TODO: Make showing the units optional.
        printf("Information for PMT %d:\n", pmt);//comment
        printf("Sk: %fuA/Im\n", ship_data->GetSk());
        printf("Skb: %f\n", ship_data->GetSkb());
        printf("Sp: %fA/Im\n", ship_data->GetSp());
        printf("Idb: %fnA\n", ship_data->GetIdb());
        printf("EBB: %dV\n", ship_data->GetEBB());
        printf("Dark Count: %dcps \n", ship_data->GetDR());
        printf("TTS: %fns\n", ship_data->GetTTS());
        printf("PTV: %f\n", ship_data->GetPTV());
      DeleteShipData(ship_data);
    }
  }
  else HelpFunction();
  
  return 1;
}

void GetData(int argc, char *argv[]){
  int c; 
  int pmt = atoi(argv[argc-1]);//pmt should be last option

  if (pmt == 0) {
    HelpFunction();
    return;
  }
  int i = 0; //counts how many times the loop executes
  ShippingData *ship_data = MakeShipData(pmt);
  //potential alternative: use a map to hold the following data
  bool showunits = false;
  bool showsk = false;
  bool showskb = false;
  bool showsp = false;
  bool showidb = false;
  bool showebb = false;
  bool showdr = false;
  bool showtts =false;
  bool showptv = false;

  while ((c = getopt(argc, argv, "(kbsIedtphaTu):")) != -1) {
    switch (c) {
      case 'k':
        //Sk:
        //printf("Sk: %fuA/Im\n", ship_data->GetSk());
        showsk = true;
        break;
      case 'b':
        //Skb
        //printf("Skb: %f\n", ship_data->GetSkb());
        showskb = true;
        break;
      case 's':
        //Sp
        //printf("Sp: %fA/Im\n", ship_data->GetSp());
        showsp = true;
	      break;
      case 'I':
        //Idb 
        //printf("Idb: %fnA\n", ship_data->GetIdb());
        showidb = true;
        break;
      case 'e':
        showebb = true;
        //nominal voltage?
        break;
      case 'd':
        showdr = true;
        break;
      case 't':
        showtts = true;
        break;
      case 'p':
        showptv = true;
        break;
      case 'u':
        showunits = true;
        break;
      case 'a':
        //Prints all of the above
        showsk = true;
        showskb = true;
        showsp = true;
        showidb = true;
        showebb = true;
        showdr = true;
        showtts = true;
        showptv = true;
        break;
      /*
      case 'T':
        //Test case
        printf("PMT number: %f\n", MakeShipData(pmt)->GetPMT());
        break;
      */
      case 'h':
        HelpFunction();
        break;
      case '?': //unknown input
        HelpFunction();
        break;
      default:
        printf("Aborting...\n");
        abort();
    }//exit switch 
    i += 1;
  }//exit while loop

  if (i == 0) {
    HelpFunction();
    return;
  }
  
  //I'm only using the ternary (?) operator because I wanted to seem clever.
  if (showsk) printf("Sk: %f %s\n", ship_data->GetSk(), showunits ? "uA/Im" : "");
  if (showskb) printf("Skb: %f\n", ship_data->GetSkb());
  if (showsp) printf("Sp: %f %s\n", ship_data->GetSp(), showunits ? "A/Im" : "");
  if (showidb) printf("Idb: %f %s\n", ship_data->GetIdb(), showunits ? "nA" : "");
  if (showebb) printf("EBB: %d %s\n", ship_data->GetEBB(), showunits ? "V" : "");
  if (showdr) printf("DR: %d %s\n", ship_data->GetDR(), showunits ? "cps" : "");
  if (showtts) printf("TTS: %f %s\n", ship_data->GetTTS(), showunits ? "ns" : "");
  if (showptv) printf("PTV: %f\n", ship_data->GetPTV());

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
  ShippingData *ship_data = new ShippingData(pmt,0);
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
