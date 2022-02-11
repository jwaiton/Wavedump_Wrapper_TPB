#!/bin/bash
#---- Script to move wavedump files to PMT directories
echo 'Which test? (Nom or DR)'
read test

echo 'Which RUN ? ( e.g. 000001 )'
read RUN

PARENT=/home/user1/Watchman/Wavedump_Wrapper/Data_Storage/Gary/Setup/${RUN}/

echo 'PMT for wave0? (e.g. 0052)'
read PMT0
mkdir -pv ${PARENT}PMT$PMT0/DarkRateTest/
mkdir -pv ${PARENT}PMT$PMT0/Nominal/

echo 'PMT for wave1?'
read PMT1
mkdir -pv ${PARENT}PMT$PMT1/DarkRateTest/
mkdir -pv ${PARENT}PMT$PMT1/Nominal/
echo 'PMT for wave2?'
read PMT2
mkdir -pv ${PARENT}PMT$PMT2/DarkRateTest/
mkdir -pv ${PARENT}PMT$PMT2/Nominal/

echo 'PMT for wave3?'
read PMT3
mkdir -pv ${PARENT}PMT$PMT3/DarkRateTest/
mkdir -pv ${PARENT}PMT$PMT3/Nominal/

#----------------------------------------------------------------------
#--------------------DR test moving statements-------------------------
#----------------------------------------------------------------------
if [ $test = "DR" ]
then
    mv -vn wave0.dat ${PARENT}PMT$PMT0/DarkRateTest/wave_0.dat

    mv -vn wave1.dat ${PARENT}PMT$PMT1/DarkRateTest/wave_1.dat

    mv -vn wave2.dat ${PARENT}PMT$PMT2/DarkRateTest/wave_2.dat

    mv -vn wave3.dat ${PARENT}PMT$PMT3/DarkRateTest/wave_3.dat
elif [ $test = "Nom" ]
then
    mv -vn wave0.dat ${PARENT}PMT${PMT0}/Nominal/wave_0.dat

    mv -vn wave1.dat ${PARENT}PMT${PMT1}/Nominal/wave_1.dat

    mv -vn wave2.dat ${PARENT}PMT${PMT2}/Nominal/wave_2.dat

    mv -vn wave3.dat ${PARENT}PMT${PMT3}/Nominal/wave_3.dat
    
else
    echo "TEST TYPE NOT FOUND!"
fi
