double BytesPerSecond(char test = 'A'
		      ){
  
  // Trigger Rate
  double Tr = 680.;
  // Record Length
  double Rl = 5100;
  //  Bytes per sample
  double Bs = 2.;
  // Number of channels
  double Nc = 1.;
  // Bytes per header
  double Bh = 16.;

  if( test!='A' ){
    Tr = 10000.;
    Rl = 102.;
  }
  
  return Tr*(Rl*Bs*Nc+Bh);
  
}

double DataSize(char   test,
		double acquisitionTime){
  
  // convert to seconds
  acquisitionTime *= 60.;
  
  return acquisitionTime*BytesPerSecond(test);
  
}
