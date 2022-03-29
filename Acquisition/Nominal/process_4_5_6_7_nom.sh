#!/bin/bash

#---- Script to move wavedump files to PMT directories

export -a PMTs=()
source ../read_pmts.sh

export FOLDER_NAME=""
source ../read_folder_name.sh

echo 'FOLDER_NAME = ' $FOLDER_NAME

echo 'Which RUN ? ( e.g. 000001 )'
read RUN

PARENT=${FOLDER_NAME}RUN${RUN}/

echo $PARENT
echo $PMTs

# process PMT data
for i in 4 5 6 7
do
    echo 'PMT for wave' $i ' ? (e.g. 0052)'
    FOLDER=${PARENT}PMT${PMTs[$i]}/Nominal/
    echo $FOLDER 
    mkdir -pv $FOLDER
    mv ./wave${i}.dat ${FOLDER}wave_${i}.dat 
done

# the lines below can be used to process all of the files
# for a full run in separate sub shells 
# - see $WM_CODE/run_process_analyse_locally_auto_on_full_run.sh
 
cd $PARENT
echo $PWD

source run_process_analyse_locally_auto_on_full_run.sh



    

