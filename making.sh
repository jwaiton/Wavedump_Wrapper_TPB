#!/bin/bash

cd $WM_CONVERT
pwd
make clean ; make dat_to_root

cd $WM_COOK
pwd
make realclean ; make


cd $WM_ANALYSE_COOKED
pwd
make realclean ; make

cd $WM_SHIPPING_DATA
pwd
make realclean ; make

cd $WM_WAVE_PLOT
pwd
make realclean ; make
