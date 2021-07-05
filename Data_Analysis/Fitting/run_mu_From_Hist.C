
void run_mu_From_Hist(int pmt=15, int loc=4, int run=40){
  
  string rootFileName;
  rootFileName =  "hQ_Fixed_Run_";
  rootFileName += std::to_string(run);
  rootFileName += "_PMT_";
  rootFileName += std::to_string(pmt);
  rootFileName += "_Loc_";
  rootFileName += std::to_string(loc);
  rootFileName += "_Test_N";

  string pathToData;
  pathToData =  "/home/user1/Watchman/Wavedump_Wrapper";
  pathToData += "/Data_Storage/Gary/Setup";
  pathToData += "/RUN0000";
  pathToData += std::to_string(run);
  pathToData += "/PMT0";
  
  if(pmt<100)
    pathToData +="0";
  if(pmt<10)
    pathToData +="0";
  
  pathToData += std::to_string(pmt);
  pathToData += "/Nominal/";

  mu_From_Hist(rootFileName,pathToData);
  
}
