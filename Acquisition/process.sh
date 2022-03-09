#!/bin/bash
#---- Script to move wavedump files to PMT directories

export -a PMTs=()

source read_pmts.sh

PARENT=/home/user1/Watchman/Wavedump_Wrapper/Data_Storage/Retest/

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

read TEST

FOLDER=${PARENT}PMT0000/$TEST/
mkdir -pv $FOLDER
mv ./wave8.dat ${FOLDER}wave_8.dat
cd $FOLDER
# process with no analysis
process_locally_auto.sh > output.txt

cd $HOME 
#--

# process PMT data
for i in 4 5 6 7
do
    #echo 'PMT for wave' $i ' ? (e.g. 0052)'
    FOLDER=${PARENT}PMT${PMTs[$i]}/$TEST/
    #echo $FOLDER 
    mkdir -pv $FOLDER
    mv ./wave${i}.dat ${FOLDER}wave_${i}.dat 
done

# the lines below can be used to process all of the files
# for a full run in separate sub shells 
# - see $WM_CODE/run_process_analyse_locally_auto_on_full_run.sh
 
cd $PARENT
source run_process_analyse_locally_auto_on_full_run.sh



    

