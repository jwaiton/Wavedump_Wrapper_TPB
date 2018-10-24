#!/bin/bash

# A shell script to assist in the recording 
# of raw data for Watchman PMT tests
 
# Author:
# gary.smith@ed.ac.uk
# 24 10 18

# Requirements:

# Environment variable 'Testing_DIR'
# which is the parent path from which 
# certain paths used here will be relative 
# to and which is (or can be) set in 
# ~user1/.bashrc (or otherwise).

# How to run,:
# At terminal prompt do:
# $ ./RunDaq.sh

#--------------------------
# Introductory Message

echo -e "/n"
echo You have executed RunDaq.sh 
echo -e "/n"
echo This script will help to run the
echo DAQ and manage the output data.

#--------------------------
# Determine PMT numbers

echo How many PMTs are you testing? e.g. 4

read nPMTs

for ((iPMT=0;iPMT<$nPMTs;iPMT++)); do

    echo -e "\n"
    echo For PMT $iPMT
    echo Enter the PMT number using a four number format 
    echo E.g. enter: 0034

    read pmtList[iPMT]

done

echo -e "\n"
echo PMT list: ${pmtList[*]}
echo -e "\n"
echo Is this correct?

read isCorrect

if [ "$isCorrect" != "Y" ] && [ "$isCorrect" != "y" ]
then 
    echo " Error in PMT list " 
    echo " Try running the script again "
    echo " Exiting "
    exit
fi

# PMT numbers have been determined
#-------------------------------------

#------------------------------------------
# Make directory structure for storing data

echo -e "\n"
echo parent folder is: $BinaryData_DIR

for pmtNumber in "${pmtList[@]}"
do

iPMT_DIR=$BinaryData_DIR/PMT$pmtNumber

mkdir -p  $iPMT_DIR/APTest
mkdir -p  $iPMT_DIR/DarkRateTest
mkdir -p  $iPMT_DIR/GainTest
mkdir -p  $iPMT_DIR/SPEtest

done

echo -e "\n"
echo The directory structure for storing 
echo output data has been created.
#------------------------------------------


#------------------------------------------
# Run the DAQ

isReady=N

while [ "$isReady" != "Y" ] && [ "$isReady" != "y" ] 
do
    echo -e "\n"
    echo Ready for the nominal voltage SPE run ?
    echo Enter: y or n
    read isReady
done


# SPE - acquire data for five mins

echo -e "\n"
echo Running wavedump with SPE configuration
echo which will take 5 mins
echo  ... pausing for 10 seconds first

sleep 10

#wavedump_G_SPE_DR < inputSPE.txt

cat ./input_wavedump_5mins.txt 

echo -e "\n"
echo The End.
