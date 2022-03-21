#!/bin/bash

echo " -------------------------------"
date  
echo " running  "
echo " cook_analyse_locally_auto.sh "
echo " -------------------------------"

DIR_PATH=${PWD}/

FILE_NAME=$(find . -maxdepth 1 -name "wave_*.dat.root" )

FILE_PATH=${DIR_PATH}${FILE_NAME}

#echo ${FILE_PATH}
cook_raw ${FILE_PATH}

echo " ------------------------------"
date 
echo " ------------------------------"

#echo ${DIR_PATH}/Run* 
analyse_cooked  ${DIR_PATH}

echo " ------------------------------"
date 
echo " ------------------------------"
