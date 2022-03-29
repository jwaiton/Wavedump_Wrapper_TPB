#!/bin/bash

# move trigger data (as PMT0000) 



export FOLDER_NAME=""
source ../read_folder_name.sh

echo 'FOLDER_NAME = ' $FOLDER_NAME

echo 'Which RUN ? ( e.g. 000001 )'
read RUN

PARENT=${FOLDER_NAME}RUN${RUN}/

echo $PARENT

# storage of pulser data
FOLDER=${PARENT}PMT0000/Nominal/
mkdir -pv $FOLDER
mv ./wave8.dat ${FOLDER}wave_8.dat
cd $FOLDER

## process pulser data (no analysis)
# process_locally_auto.sh > output.txt
  

