#!/bin/bash
#---- Script to write analysis data from hQ.root files to data storage folder
#---- Start in the /Data_Storage/RUN0000NN folder that you wish to analyse
#---- Put me in the Wavedump_Wrapper/ folder and use me with source command
#---- This code will automatically generate a text file with the results P:V, mu, and efficiency within them in the RUN0000NN/PMT0YYY/Nominal/Text_Files/ folder.

# Currently leaves the hQ file in the fitting folder, will adjust to delete this after it is finished with it.


# Should start in the RUN0000NN folder that you wish to be in
HOME=$PWD
# Where the analysis is done, can be adjusted based on where you want to send the hQ.root file
MOVE=/home/user1/Watchman/Wavedump_Wrapper/Data_Analysis/Fitting/

# Find PMT folders after PMT000000. Will just be one for XY but can be expanded for more.
array=(*/)
array=("${array[@]:1}")

# For each array, make new TextFiles folder and send off the hQ.root file
for dir in "${array[@]}"
do echo $dir
cd $dir
# assuming we're using Nominal for all systems here
cd Nominal
mkdir -p ./TextFiles/
# Will write to this folder
TEXT=${PWD}/TextFiles/
# Find the file we're going to move to fitting
FILE_NAME=$(find . -maxdepth 1 -name "hQ_Fixed_Run_*_PMT_*_Loc_*_Test_N.root")
cp $FILE_NAME ${MOVE}
cd $MOVE
# Remove ./ from FILE_NAME
FILE_NAME="${FILE_NAME:2}"
FILE=\"$FILE_NAME\"
# Run XYwrite.C
root -b -q 'XYwrite.C+('$FILE')'
# Move .txt file over
mv xy_results.txt $TEXT
# Move back to $dir
cd $TEXT
cd ..
cd ..
done

# Move back to DAQ
cd $WM_DAQ


# Determine which run/runs we're analysing
#echo 'Which RUN ? ( e.g. 000001 )'
#read RUN
#echo 'How many RUNs? (eg 2 is 000001 and 000002)'
#read nRUN
#RUN=`expr $RUN + $nRUN`
#echo $RUN


# OLD CODE
#PARENT=${PARENT}RUN${RUN}/

#echo $PARENT

#for i in 8 9
#do
#    #echo 'PMT for wave' $i ' ? (e.g. 0052)'
#    FOLDER=${PARENT}PMT${PMTs[$i]}/Nominal/
#    #echo $FOLDER 
#    mkdir -pv $FOLDER
#    mv ./wave${i}.dat ${FOLDER}wave_${i}.dat 
#done

# the lines below can be used to process all of the files
# for a full run in separate sub shells 
# - see $WM_CODE/run_process_analyse_locally_auto_on_full_run.sh
 
#cd $PARENT
#source run_process_analyse_locally_auto_on_trig.sh



    

