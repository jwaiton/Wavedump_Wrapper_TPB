#!/bin/bash

#---- Script to move wavedump files to PMT directories

export -a PMTs=()

source ../read_pmts.sh

PARENT=/home/user1/Watchman/Wavedump_Wrapper/Storage/Position1/

echo 'Which RUN ? ( e.g. 000001 )'
read RUN

PARENT=${PARENT}RUN${RUN}/

echo $PARENT
echo $PMTs

# process PMT data
for i in 4 5 6 7
do
    echo 'PMT for wave' $i ' ? (e.g. 0052)'
    FOLDER=${PARENT}PMT${PMTs[$i]}/Dark/
    #echo $FOLDER 
    mkdir -pv $FOLDER
    mv ./wave${i}.dat ${FOLDER}wave_${i}.dat 
done

# the lines below can be used to process all of the files
# for a full run in separate sub shells 
# - see $WM_CODE/run_process_analyse_locally_auto_on_full_run.sh
 
cd $PARENT
echo $PWD

#cp $WM_CODE/Scripts/process_analyse_locally_auto.sh .
#source process_analyse_locally_auto.sh
source run_process_analyse_locally_auto_on_full_run.sh




    

