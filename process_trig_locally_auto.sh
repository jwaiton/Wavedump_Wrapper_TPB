#!/bin/bash

echo " -------------------------------"
date  
echo " running  "
echo " process_analyse_locally_auto.sh "
echo " -------------------------------"

DIR_PATH=${PWD}/

FILE_NAME=$(find . -maxdepth 1 -name "wave_*.dat" )

FILE_PATH=${DIR_PATH}${FILE_NAME}

dat_to_root ${FILE_PATH}

echo " ------------------------------"
date 
echo " ------------------------------"

mkdir -p ./Plots/DAQ/

cook_raw ${FILE_PATH}.root

echo " ------------------------------"
date 
echo " ------------------------------"
# Commented out, as its not needed for trigger
#analyse_cooked  ${DIR_PATH}/Run*

echo " ------------------------------"
date 
echo " ------------------------------"
