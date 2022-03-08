#!/bin/bash

isReady=n

#----------------------------
# Gain test

while [ "$isReady" != "y" ] 
do
    echo -e "\n"
    echo Ready for Gain run $1 at $2 V ?
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
echo Running wavedump with Gain configuration
echo which will take 5 mins
echo  ... pausing for 10 seconds first
echo "(Ctrl-C to quit )"

if    [ "$location" = "Boulby" ]
then
    sleep 10
    wavedump_G_SPE_DR < input_wavedump_5mins.txt
elif  [ "$location" = "Edinburgh" ]
then
    sleep 10
    wavedump < input_wavedump_5mins.txt
elif  [ "$location" = "Test" ]
then
    echo -e "\n"
    echo Test mode
    cat input_wavedump_5mins.txt
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

	mv wave_$iChannel.dat $iPMT_DIR/Gaintest/
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
