#!/bin/bash

echo -e "\n-------------------------------------------"
echo -e "About to initiate. \n"
echo -e "Begin? y or n"
echo -e "-------------------------------------------\n"

read Answer

if [ "$Answer" != "y" ] && [ "$Answer" != "Y" ];
then
    echo -e "Exiting"	
    exit 0
fi

echo -e "Number of Hours?"

read Hours

echo -e "Input Run Number:"
read Run 

OUTPUT_DIR=${BINARY_DATA_DIR}/Cooldown/RUN0000${Run}

echo -e $OUTPUT_DIR

mkdir -p ${OUTPUT_DIR}

for i in $(seq $Hours)
do
    (sleep 3s && echo "s" && sleep 3s && echo "W" && sleep 15m && echo "W" && sleep 3s && echo "q") | wavedump_G_SPE_DR

    for j in 0 1 2 3 4 5 6 7    
    do
	if [ $i -lt 10 ]
	then
	    mv wave_${j}.dat ${OUTPUT_DIR}/wave_${j}_0${i}.dat
	else
	    mv wave_${j}.dat ${OUTPUT_DIR}/wave_${j}_${i}.dat    
	fi
    done
    mv ${OUTPUT_DIR}* /mnt/usb-Seagate_BUP_RD_NA9FWF45-0\:0-part1/BinaryData_Backups/Cooldown/RUN000033/
    sleep 42m
done	

exit 1
fi

