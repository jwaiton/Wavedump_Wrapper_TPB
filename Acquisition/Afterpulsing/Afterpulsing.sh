#!/bin/bash

# Shell script for acquiring data for
#   Afterpulsing test

# wavedump settings (including e.g. no. samples per waveform)   
export config_file="${WM_DAQ}Config/Conf_After.txt"

#export test="test"
export test="after"

# run common acquisition program which
# will utilise the settings above
source ${WM_DAQ}Acquire.sh
