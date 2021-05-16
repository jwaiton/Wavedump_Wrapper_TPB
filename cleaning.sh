#!/bin/bash

# cleaning folders

# convert binary to basic root version
cd $WM_CONVERT
pwd
make clean

# convert binary to basic root version
cd $WM_CONVERT/DT_Version
pwd
make clean

# 'cook' basic root file 
#  o/p includes: ADC, basic variables, Meta data 
cd $WM_COOK
pwd
make realclean

# perform (primative) analysis
# e.g. dark counts, LED timing peak fit
cd $WM_ANALYSE_COOKED
pwd
make realclean

# print shipping data
cd $WM_SHIPPING_DATA
pwd
make realclean
# plot waveform and/or FFT
cd $WM_WAVE_PLOT
pwd
make realclean

cd $WM_CODE
