#!/bin/bash

echo " Setting Watchman Environment "

# if not set in .bashrc then set here
# export WM_PARENT=/path/to/
 
# testing code
export WM_CODE=${WM_PARENT}Wavedump_Wrapper/

export WM_DATA=${WM_CODE}Data_Storage/
export WM_BINARY=${WM_DATA}Binary_Data/
export WM_ROOT=${WM_DATA}Raw_Root_Data/

export WM_DAQ=${WM_CODE}Data_Acquisition/

export WM_PROCESS=${WM_CODE}Data_Processing/
export WM_CONVERT=${WM_PROCESS}Binary_Conversion/
export WM_COOK=${WM_PROCESS}Cooking/

export WM_COMMON=${WM_CODE}Common_Tools/

export WM_ANALYSE=${WM_CODE}Data_Analysis/
export WM_ANALYSE_COOKED=${WM_ANALYSE}Analyse_Cooked/
export CALIBRATION=${WM_ANALYSE}Calibration/
export BIN_TO_ROOT=${WM_ANALYSE}BinToRoot/
export GAIN_TEST_DIR=${WM_ANALYSE}Gain_Test/
export WM_SHIPPING_DATA=${WM_ANALYSE}Shipping_Data/

# header files
export CPATH=${CPATH}:${WM_COMMON}
export CPATH=${CPATH}:${WM_COOK}
export CPATH=${CPATH}:${WM_ANALYSE_COOKED}

# binaries
export PATH=${PATH}:${WM_CONVERT}
export PATH=${PATH}:${WM_CONVERT}DT_Version/
export PATH=${PATH}:${WM_COOK}
export PATH=${PATH}:${WM_ANALYSE_COOKED}
export PATH=${PATH}:${WM_ANALYSE}Shipping_Data/

if [[ "$OSTYPE" == "linux-gnu" ]]; then
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${WM_COOK}
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${WM_COMMON}
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${WM_ANALYSE}PMT_Analysis/
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${WM_ANALYSE_COOKED}
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${WM_ANALYSE}Shipping_Data/
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${WM_ANALYSE}Waveform_Plotter/
fi

if [[ "$OSTYPE" == "darwin"* ]]; then
export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${WM_COOK}
export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${WM_COMMON}
export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${WM_ANALYSE}PMT_Analysis/
export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${WM_ANALYSE_COOKED}
export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${WM_ANALYSE}Shipping_Data/
fi

nominal_HV(){
    echo $(grep " $1 " ${WM_COMMON}HVScan.txt) | cut -d " " -f 7
}

HV_step_1(){
    echo $(grep " $1 " ${WM_COMMON}HVScan.txt) | cut -d " " -f 2
}

if [[ "$OSTYPE" == "linux-gnu" ]]; then
open(){
    evince $1
}
fi
