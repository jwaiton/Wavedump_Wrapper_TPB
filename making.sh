#!/bin/bash

cd $WM_CONVERT
make clean ; make dat_to_root

cd $WM_COOK
make realclean ; make

cd $WM_ANALYSE_COOKED
make realclean ; make