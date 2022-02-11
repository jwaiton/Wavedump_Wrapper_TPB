#!/bin/bash

echo " ---------------------------------------------- "
echo " Setting Watchman PMT Testing Environment with  "
echo " /scratch/Gary/Wavedump_Wrapper/WM.sh "

# repository  
export WM_CODE=${WM_PARENT}Wavedump_Wrapper/

# repo subdirs
export WM_DAQ=${WM_CODE}Acquisition/
export WM_ANALYSE=${WM_CODE}Analysis/
export WM_DATA=${WM_CODE}Storage/
export WM_PROCESS=${WM_CODE}Processing/
export WM_COMMON=${WM_CODE}Common/

# processing
export WM_CONVERT=${WM_PROCESS}Conversion/
export WM_COOK=${WM_PROCESS}Cooking/

# analysis
export WM_ANALYSE_COOKED=${WM_ANALYSE}Cooked/
export WM_FITTING=${WM_ANALYSE}Fitting/
export WM_SHIPPING_DATA=${WM_ANALYSE}Shipping/
export WM_WAVE_PLOT=${WM_ANALYSE}Waveform_Plotter/

# header files
export CPATH=${CPATH}:${WM_COMMON}
export CPATH=${CPATH}:${WM_FITTING}
export CPATH=${CPATH}:${WM_COOK}
export CPATH=${CPATH}:${WM_ANALYSE_COOKED}

# binaries
export PATH=${PATH}:${WM_CODE}
export PATH=${PATH}:${WM_CONVERT}
export PATH=${PATH}:${WM_CONVERT}DT_Version/
export PATH=${PATH}:${WM_COOK}
export PATH=${PATH}:${WM_ANALYSE_COOKED}
export PATH=${PATH}:${WM_SHIPPING_DATA}
export PATH=${PATH}:${WM_WAVE_PLOT}

if [[ "$OSTYPE" == "linux-gnu" ]]; then
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${WM_COOK}
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${WM_COMMON}
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${WM_FITTING}
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${WM_ANALYSE_COOKED}
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${WM_ANALYSE}Shipping_Data/
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${WM_ANALYSE}Waveform_Plotter/
fi

if [[ "$OSTYPE" == "darwin"* ]]; then
export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${WM_COOK}
export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${WM_COMMON}
export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${WM_FITTING}
export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${WM_ANALYSE_COOKED}
export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${WM_ANALYSE}Shipping_Data/
export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${WM_ANALYSE}Waveform_Plotter/
fi

nominal_HV(){
    echo $(grep " $1 " ${WM_COMMON}HVScan.txt) | cut -d " " -f 7
}

HV_step_1(){
    echo $(grep " $1 " ${WM_COMMON}HVScan.txt) | cut -d " " -f 2
}

if [[ "$OSTYPE" == "linux-gnu" ]]; then
open(){
    evince $1 &
}
fi
