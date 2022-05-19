#!/bin/bash

# subscript for acquiring data

# inputs
# 'config_file' and 'test' are exported in mother script
#

# pause length between commands
pause=3s

# length of acquisition, default (spe,dark)
length=667s

# shorter tests
if [ $test == "test" ]
then
    length=0.1s
    pause=0.1s
elif [ $test == "noise" ] || [ $test == "gain" ]
then
    length=100s
elif [ $test == "after" ]
then
    length=500s
elif [ $test == "cool" ]
then
    length=3s
fi

input_to_wavedump(){
    # enable writing
    sleep $pause   && echo "W" && \
    # start acquisition
    sleep $pause   && echo "s" && \
    # acquire data for duration specified then stop acquisition
    sleep $length  && echo "s" && \
    # quit wavedump
    sleep $pause   && echo "q"
}

#if [ $test == "test" ]
#then
#    echo "---------------\n Testing Only \n---------------\n"
#    echo " wavedump $config_file \n"
#    head  $config_file
#    echo "...\n\n..."
#    tail  $config_file
#    input_to_wavedump 
#else
#    # pipe the commands from input_to_wavedump() into wavedump
#    echo "Running wavedump \n"
#    input_to_wavedump | wavedump $config_file
#fi

input_to_wavedump | wavedump $config_file
