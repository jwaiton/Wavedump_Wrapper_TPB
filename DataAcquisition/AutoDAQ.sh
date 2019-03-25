4#!/bin/bash

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

# !!!! CHECK PARENT FOLDER NAME
OUTPUT_DIR=${BINARY_DATA_DIR}/Cooldown/RUN0000${Run}

echo -e $OUTPUT_DIR

mkdir ${OUTPUT_DIR}

for i in $(seq $Hours)
do
# !!!! MODIFY default config file for 'wavedump' or rewrite to accept alias
#    (sleep 3s && echo "s" && sleep 3s && echo "W" && sleep 5s echo "W" && sleep 3s && echo "q") | wavedump
    (sleep 3s && echo "s" && sleep 3s && echo "W" && sleep 5s && echo "W" && sleep 3s && echo "q") | wavedump

    for j in 0 1 2 3 4 5 6 7    
    do
	if [ $i -lt 10 ]
	then
	    mv wave_${j}.dat ${OUTPUT_DIR}/wave_${j}_0${i}.dat
	else
	    mv wave_${j}.dat ${OUTPUT_DIR}/wave_${j}_${i}.dat    
	fi
    done

    sleep 10s
done	

else
echo -e "Invalid input \n"
echo -e "Exiting \n"	

exit 1
fi

