#!/bin/bash

echo " ----------------------------------------"
date  
echo " running  "
echo " process_analyse_locally_auto_desktop.sh "
echo " ----------------------------------------"

DIR_PATH=${PWD}/

FILE_NAME=$(find . -maxdepth 1 -name "wave_*.dat" )

FILE_PATH=${DIR_PATH}${FILE_NAME}


desktop_dat_to_root ${FILE_PATH} 

echo " ------------------------------"
date 
echo " ------------------------------"

mkdir -p ./Plots/DAQ/

# must specify desktop at runtime
# other argument options available - see $WM_COOK/cook_raw.C
cook_raw ${FILE_PATH}.root -d D 

echo " ------------------------------"
date 
echo " ------------------------------"

analyse_cooked  ${DIR_PATH}/Run*

echo " ------------------------------"
date 
echo " ------------------------------"
