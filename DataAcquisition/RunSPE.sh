#!/bin/bash

# A master shell script to assist in the recording 
# of raw data for Watchman PMT tests
 
# Author:
# gary.smith@ed.ac.uk
# 08 11 18

# Requirements:

# Environment variable 'BinaryData_DIR'
# which is the parent path in which 
# folders used to store data will be
# created in and which is (or can be) set 
# in e.g. ~user1/.bashrc (or otherwise).

# Shell scripts
#    SPE_DAQ.sh

# How to run:
# At terminal prompt do:
# $ ./RunSPE.sh

# Warning: This script presumes that raw data
# does not exist for the PMTs being tested.
# Should raw data exist already in the 
# target location (folder specific to PMT 
# serial number) this will be overwritten.

# Set location

export location="Test"
#location="Edinburgh"
#location="Boulby"

#--------------------------
# Introductory Message

echo -e "\n"
echo -e "\n"
echo You have executed RunSPE.sh 
echo -e "\n"
echo This script will help to run the
echo DAQ and manage the output data.

#--------------------------
# Determine PMT numbers

# echo How many PMTs are you testing? e.g. 4
# read nPMTs

nPMTs=4

for ((iPMT=0;iPMT<$nPMTs;iPMT++)); do

    echo -e "\n"
    echo For PMT $iPMT
    echo Enter the serial number using a four number format 
    echo E.g. enter: 0034

    read pmtList[iPMT]

done

echo -e "\n"
echo PMT list: ${pmtList[*]}
echo -e "\n"
echo Is this correct?
echo Enter: y or n

read isCorrect

if [ "$isCorrect" == "Y" ]
   then
   isCorrect=y
fi

if [ "$isCorrect" != "y" ]
then 
    echo " Error in PMT list " 
    echo " Try running the script again "
    echo " Exiting "
    exit
fi

export pmtList

# PMT numbers have been determined
#-------------------------------------

#------------------------------------------
# Make directory structure for storing data

echo -e "\n"
echo Parent folder is: 
echo $BinaryData_DIR

echo -e "\n"
echo Raw data storage folders:

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
echo output data exists or has been created.
#------------------------------------------


#------------------------------------------
# Run the DAQ


# SPE / Nominal voltage test
./SPE_DAQ.sh

# End of Data Acquisition
#------------------------------------------

echo -e "\n"
echo The End.
