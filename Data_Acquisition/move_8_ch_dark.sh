#!/bin/bash
#---- Script to move wavedump files to PMT directories
# 
# depends on read_pmts.sh

export -a PMTs=()

source read_pmts.sh

PARENT=/home/user1/Watchman/Wavedump_Wrapper/Data_Storage/Gary/Setup/

echo 'Which RUN ? ( e.g. 000001 )'
read RUN

PARENT=${PARENT}RUN${RUN}/

echo $PARENT

for i in 0 1 2 3 4 5 6 7 
do
    #echo 'PMT for wave' $i ' ? (e.g. 0052)'
    FOLDER=${PARENT}PMT${PMTs[$i]}/DarkRateTest/
    #echo $FOLDER 
    mkdir -pv $FOLDER
    mv ./wave${i}.dat ${FOLDER}wave_${i}.dat 
done

    

