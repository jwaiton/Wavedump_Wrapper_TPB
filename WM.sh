#!/bin/bash

echo " ---------------------------------------------- "
echo " Setting Watchman PMT Testing Environment with  "

# repository  
export WM_CODE=${WM_PARENT}Wavedump_Wrapper/

echo " ${WM_CODE}WM.sh"

# repo subdirs
export WM_DAQ=${WM_CODE}Acquisition/
export WM_ANALYSIS=${WM_CODE}Analysis/

export WM_DATA=${WM_CODE}Storage/
export WM_PROCESS=${WM_CODE}Processing/
export WM_COMMON=${WM_CODE}Common/

# scripts
export WM_SCRIPTS=${WM_CODE}Scripts/

# processing
export WM_CONVERT=${WM_PROCESS}Conversion/
export WM_COOK=${WM_PROCESS}Cooking/

# analysis
export WM_ANALYSE=${WM_ANALYSIS}Analyse/
export WM_FITTING=${WM_ANALYSIS}Fitting/
export WM_SHIPPING=${WM_ANALYSIS}Shipping/
export WM_WAVEPLOT=${WM_ANALYSIS}Waveplotter/

# header files
export CPATH=${CPATH}:${WM_COMMON}
export CPATH=${CPATH}:${WM_FITTING}
export CPATH=${CPATH}:${WM_COOK}
export CPATH=${CPATH}:${WM_ANALYSE}
export CPATH=${CPATH}:${WM_SHIPPING}

# binaries
export PATH=${PATH}:${WM_CODE}
export PATH=${PATH}:${WM_CONVERT}
export PATH=${PATH}:${WM_COOK}
export PATH=${PATH}:${WM_ANALYSE}
export PATH=${PATH}:${WM_SHIPPING}
export PATH=${PATH}:${WM_WAVEPLOT}
export PATH=${PATH}:${WM_SCRIPTS}

if [[ "$OSTYPE" == "linux-gnu" ]]; then
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${WM_COOK}
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${WM_COMMON}
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${WM_FITTING}
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${WM_ANALYSE}
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${WM_SHIPPING}
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${WM_WAVEPLOT}
fi

if [[ "$OSTYPE" == "darwin"* ]]; then
export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${WM_COOK}
export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${WM_COMMON}
export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${WM_FITTING}
export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${WM_ANALYSE}
export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${WM_SHIPPING}
export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${WM_WAVEPLOT}
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
