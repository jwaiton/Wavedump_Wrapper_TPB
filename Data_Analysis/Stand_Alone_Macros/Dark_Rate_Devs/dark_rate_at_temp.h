float kB(){
  return 8.617E-5;
}

float alpha(){
  //return 1.32;
  return 1.46; // produces lambda values of about 0.5
}

float A_0(){

  float A_0 = 1.20173E6; // A K^(-2)
  
  return  A_0/1.60217662E-19; //s^(-1) K^(-2)
}

float A_G(float n25 = 2000){

  // N = J * Area 
  // N = A_G T^2 exp(-W/kT) * Area
  // A_G = N / ( T^2 exp(-W/kT) * Area )

  float t25   = 25 + 273.15;
  float denom = t25*t25*exp(-alpha()/(kB()*t25));
  float area  = 0.038013271; // 220 mm diameter  
  denom *= area;
  
  return n25/denom;
}

float dark_rate_at_temp(float n25 = 2000, float temp = 25){

  float tX    = temp + 273.15;
  float t25   = 25 + 273.15;
  float expon = alpha() / kB() * (tX - t25)/( tX * t25 );
  float coeff = n25 * (tX / t25) * (tX / t25);  
  
  return coeff * exp ( expon );
}

