#include <cstdlib>
#include "TMath.h"
#include <iostream>

float Impedance(float epsilon,
		float D,
		float d){
  return 60./TMath::Sqrt(epsilon)*TMath::Log(D/d);
}

float Velocity(float epsilon){
  return 1.0/TMath::Sqrt(epsilon);
}

float Velocity(float time_ns, 
	       float distance){
  return (distance*100./time_ns)/30.;
}

float Epsilon(float vR){
  return 1.0/(vR*vR);
}

float Wavelength(float freq_MHz){
  return 300./freq_MHz;
}

float Freq_MHz(float wavelength){
  return Wavelength(wavelength);
}

void CableParameters(){
  
  float D  = 4.0;
  float d  = 1.0;
  
  float cableLength = 80.0;
  float propTime_ns = 400.0;
  
  float vR = Velocity(propTime_ns,
		      cableLength);

  float epsilon = Epsilon(vR);

  float impedance = Impedance(epsilon,D,d);

  printf("\n Velocity Factor          = %f \n",vR);
  printf("\n Epsilon                  = %f \n",epsilon);
  printf("\n Characteristic Impedance = %f \n",impedance);
  //printf("\n Velocity Factor          = %f \n",Velocity(epsilon));  

  printf("\n Frequency @ %.0f m = %.2f MHz \n",2.,Wavelength(2.));
  printf("\n Frequency @ %.0f m = %.2f MHz \n",80.,Wavelength(80.));

  printf("\n Wavelength @ %.2f MHz = %.2f m \n",100.,Wavelength(100.));
  printf("\n Wavelength @ %.2f GHz = %.2f cm \n",1.,Wavelength(1000.)*100. );
  
}
