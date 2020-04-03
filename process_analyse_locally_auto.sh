#!/bin/bash

echo " ------------------------------"
date  
echo " ------------------------------"

echo " Enter full path to binary file, e.g.: " 
echo "/home/user1/Watchman/Wavedump_Wrapper/Data_Storage/Binary_Data/Setup/RUN000002/PMT0015/Nominal/" 

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

analyse_cooked  ${DIR_PATH}/Run*

echo " ------------------------------"
date 
echo " ------------------------------"
