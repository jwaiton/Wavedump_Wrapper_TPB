#!/bin/bash

echo " -------------------------------"
date  
echo " running  "
echo " process_analyse_locally_auto_permissions.sh "
echo " -------------------------------"

DIR_PATH=${PWD}/

FILE_NAME=$(find . -maxdepth 1 -name "wave_*.dat" )

FILE_PATH=${DIR_PATH}${FILE_NAME}

dat_to_root ${FILE_PATH}

chmod 755 *.root

echo " ------------------------------"
date 
echo " ------------------------------"

mkdir -p ./Plots/
chmod 755 ./Plots

mkdir -p ./Plots/DAQ/

cook_raw ${FILE_PATH}.root

chmod 755 Run*.root

echo " ------------------------------"
date 
echo " ------------------------------"

analyse_cooked  ${DIR_PATH}/Run*

echo " ------------------------------"
date 
echo " ------------------------------"
