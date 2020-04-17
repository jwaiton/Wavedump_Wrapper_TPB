#!/bin/bash

# compile command line programs 
# (after first cleaning folders)

# convert binary to basic root version
cd $WM_CONVERT
pwd
make clean ; make -f ./Build_Options/Makefile_clang++ dat_to_root

# 'cook' basic root file 
#  o/p includes: ADC, basic variables, Meta data 
cd $WM_COOK
pwd
make realclean ; make -f ./Build_Options/Makefile_clang++

# perform (primative) analysis
# e.g. dark counts, LED timing peak fit
cd $WM_ANALYSE_COOKED
pwd
make realclean ; make -f ./Build_Options/Makefile_clang++

# print shipping data
cd $WM_SHIPPING_DATA
pwd
make realclean ; make -f ./Build_Options/Makefile_clang++

# plot waveform and/or FFT
cd $WM_WAVE_PLOT
pwd
make realclean ; make -f ./Build_Options/Makefile_clang++

cd $WM_CODE
