#!/bin/bash
#---- Script to move wavedump files to PMT directories
echo 'Which test? (SPE or Gain or AP or DR)'
read test

echo 'Which RUN ? ( e.g. 000001 )'
read RUN

PARENT=/home/user1/Watchman/Wavedump_Wrapper/Data_Storage/Setup/RUN${RUN}/

#----------------------------------------------------------------------
#--------------------DR test moving statements-------------------------
#----------------------------------------------------------------------
if [ $test = "DR" ]
then
    echo 'PMT and run of wave0? (e.g. 0052)'
    read PMT0
    mv -vn wave0.dat ${PARENT}PMT$PMT0/DarkRateTest/

    echo 'PMT and run of wave1?'
    read PMT1
    mv -vn wave1.dat ${PARENT}PMT$PMT1/DarkRateTest/

    echo 'PMT and run of wave2?'
    read PMT2
    mv -vn wave2.dat ${PARENT}PMT$PMT2/DarkRateTest/

    echo 'PMT and run of wave3?'
    read PMT3
    mv -vn wave3.dat ${PARENT}PMT$PMT3/DarkRateTest/
    
else
    echo "TEST TYPE NOT FOUND!"
fi
