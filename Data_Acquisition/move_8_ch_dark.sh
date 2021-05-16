#!/bin/bash
#---- Script to move wavedump files to PMT directories

PARENT=/Users/gsmith23/Desktop/Watchman/Optical_Detector/PMT/PMT_Performance_Testing/Wavedump_Wrapper/Data_Storage/Dummy/

echo 'Which RUN ? ( e.g. 000001 )'
read RUN

PARENT=${PARENT}RUN${RUN}/

echo $PARENT

for i in 0 1 2 3 4 5 6 7 
do
    echo 'PMT for wave0? (e.g. 0052)'
    read PMT 
    FOLDER=${PARENT}PMT$PMT/DarkRateTest/
    mkdir -pv $FOLDER
    mv ./wave${i}.dat ${FOLDER}wave_${i}.dat 
done

    

