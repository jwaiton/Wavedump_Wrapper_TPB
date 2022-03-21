#!/bin/bash

PARENT=/home/user1/Watchman/Wavedump_Wrapper/Storage/21mar22/

# move trigger data (as PMT0000) 

echo 'Which RUN ? ( e.g. 000001 )'
read RUN

PARENT=${PARENT}RUN${RUN}/

echo $PARENT

# storage of pulser data
FOLDER=${PARENT}PMT0000/Nominal/
mkdir -pv $FOLDER
mv ./wave8.dat ${FOLDER}wave_8.dat
cd $FOLDER

## process pulser data (no analysis)
# process_locally_auto.sh > output.txt


    

