#!/bin/bash

# compile command line programs 
# (after first cleaning folders)

# convert binary to basic root version
cd $WD_PROCESS/data_to_root/
pwd
make clean ; make dat_to_root

# 'cook' basic root file 
#  o/p includes: ADC, basic variables, Meta data 
##cd $WM_COOK
##pwd
##make realclean ; make

## perform (primative) analysis
## e.g. dark counts, LED timing peak fit
##cd $WM_ANALYSE
##pwd
##make realclean ; make

## print shipping data
##cd $WM_SHIPPING
##pwd
##make realclean ; make

# plot waveform and/or FFT
##cd $WM_WAVEPLOT
##pwd
##make realclean ; make

##cd $WM_CODE
