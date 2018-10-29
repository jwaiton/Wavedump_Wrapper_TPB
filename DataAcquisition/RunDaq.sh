#!/bin/bash

# A shell script to assist in the recording 
# of raw data for Watchman PMT tests
 
# Author:
# gary.smith@ed.ac.uk
# 24 10 18

# Requirements:

# Environment variable 'BinaryData_DIR'
# which is the parent path in which 
# folders used to store data will be
# created in and which is (or can be) set 
# in e.g. ~user1/.bashrc (or otherwise).

# How to run,:
# At terminal prompt do:
# $ ./RunDaq.sh

# Warning: This script presumes that raw data
# does not exist for the PMTs being tested.
# Should raw data exist already in the 
# target location (folder specific to PMT 
# serial number) this will be overwritten.

# Set location

location="Test"
#location="Edinburgh"
#location="Boulby"

#--------------------------
# Introductory Message

echo -e "\n"
echo -e "\n"
echo You have executed RunDaq.sh 
echo -e "\n"
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
echo Enter: y or n

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
echo parent folder is: 
echo $BinaryData_DIR

echo -e "\n"
echo raw data storage folders:

for pmtNumber in "${pmtList[@]}"
do

iPMT_DIR=$BinaryData_DIR/PMT$pmtNumber

echo $iPMT_DIR

mkdir -p  $iPMT_DIR/SPEtest
mkdir -p  $iPMT_DIR/APTest
mkdir -p  $iPMT_DIR/DarkRateTest
mkdir -p  $iPMT_DIR/GainTest

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

#------------------------------------------
# SPE - acquire data for five mins
# .... coding in progress ....

echo -e "\n"
echo Running wavedump with SPE configuration
echo which will take 5 mins
echo  ... pausing for 5 seconds first
echo "(Ctrl-C to quit )"


if    [ "$location" = "Boulby" ]
then
    sleep 10
    wavedump_G_SPE_DR < input_wavedump_10secs.txt
elif  [ "$location" = "Edinburgh" ]
then
    sleep 10
    wavedump < input_wavedump_10secs.txt
elif  [ "$location" = "Test" ]
then
    echo -e "\n"
    echo Test Mode
    cat input_wavedump_10secs.txt
else
    echo -e "\n"
    echo Unknown location
    exit
fi

iPMT=0;
for pmtNumber in "${pmtList[@]}"
do

    if  [ "$location" != "Test" ]
    then
	iPMT_DIR=$BinaryData_DIR/PMT$pmtNumber

	echo -e "\n"
	echo Moving raw data to:
	echo $iPMT_DIR

	mv wave_$iPMT.dat $iPMT_DIR/SPEtest/
    fi

((iPMT++))

done

echo -e "\n"
echo The SPE data has been acquired
echo and was moved to the storage
echo location.
#------------------------------------------


echo -e "\n"
echo The End.
