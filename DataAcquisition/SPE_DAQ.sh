#!/bin/bash

isReady=n

#----------------------------
# SPE / nominal voltage test

while [ "$isReady" != "y" ] 
do
    echo -e "\n"
    echo Ready for the nominal voltage SPE run ?
    echo HVs set, LED on
    echo Enter: y or n
    read isReady

    if [ "$isReady" == "Y" ]
    then
    isReady=y
    fi

done

#------------------------------------------
# Acquire data for five mins

echo -e "\n"
echo Running wavedump with SPE configuration
echo which will take 5 mins
echo  ... pausing for 10 seconds first
echo "(Ctrl-C to quit )"

if    [ "$location" = "Boulby" ]
then
    sleep 10
    { cat startDAQ.txt ; sleep 600 ; cat endDAQ.txt ; } | wavedump_G_SPE_DR
elif  [ "$location" = "Edinburgh" ]
then
    sleep 10
    { cat startDAQ.txt ; sleep 10 ; cat endDAQ.txt ; } | wavedump
elif  [ "$location" = "Test" ]
then
    echo -e "\n"
    echo Test mode
    cat startDAQ.txt; sleep 5; cat endDAQ.txt;
else
    echo -e "\n"
    echo Unknown location
    exit
fi

# DAQ channel for PMT
iChannel=0;
for pmtNumber in "${pmtList[@]}"
do

    if  [ "$location" != "Test" ]
    then
	iPMT_DIR=$BinaryData_DIR/PMT$pmtNumber

	echo -e "\n"
	echo Moving raw data to:
	echo $iPMT_DIR

	mv wave_$iChannel.dat $iPMT_DIR/SPEtest/
    fi

((iChannel++))

done

if  [ "$location" != "Test" ]
then
    echo -e "\n"
    echo The SPE data has been acquired
    echo and was moved to the storage
    echo location.
else
    echo -e "\n"
    echo Testing complete
fi
