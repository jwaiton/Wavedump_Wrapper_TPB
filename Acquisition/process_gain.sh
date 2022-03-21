#!/bin/bash
#---- Script to move wavedump files to PMT directories

export -a PMTs=()

source read_pmts.sh

PARENT=/home/user1/Watchman/TestGain/

echo ' Parent Directory is ' $PARENT

date

echo 'Which Subfolder name? '
read SUBFOLDER

PARENT=${PARENT}${SUBFOLDER}/

echo ' Parent Directory is ' $PARENT

echo 'Which RUN ? ( e.g. 000001 )'
read RUN

PARENT=${PARENT}RUN${RUN}/

echo $PARENT

# --
# process trigger data (as PMT0000) 
HOME=$PWD

echo ' Which Test? '
echo ' (e.g. Dark, Nominal) '

TEST='Gain'

# the lines below can be used to process all of the files
# for a full run in separate sub shells 
# - see $WM_CODE/run_process_analyse_locally_auto_on_full_run.sh
 
cd $PARENT
source run_process_analyse_locally_auto_on_full_run.sh



    

