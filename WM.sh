#!/bin/bash

echo " Setting Watchman Environment "
 
# testing code
export WM_CODE=${WM_PARENT}Wavedump_Wrapper/

export DATA_DIR=${WM_CODE}Data_Storage/
export BINARY_DATA_DIR=${DATA_DIR}Binary_Data/
export RAW_ROOT_DATA_DIR=${DATA_DIR}/Raw_Root_Data/

export DAT_TO_ROOT=${WM_CODE}Data_Processing/Binary_Conversion/
export COOKING=${WM_CODE}Data_Processing/Cooking/
export WM_ANALYSIS=${WM_CODE}Data_Analysis/

export CALIBRATION=${WM_ANALYSIS}Calibration/
export BIN_TO_ROOT=${WM_ANALYSIS}BinToRoot/
export GAIN_TEST_DIR=${WM_ANALYSIS}Gain_Test/
export SHIPPING_DATA=${WM_ANALYSIS}Shipping_Data/

# header files
export CPATH=$WM_CODE/Common_Tools/
export CPATH=${CPATH}:$WM_CODE/Data_Processing/Cooking/

# binaries
export PATH=${PATH}:$WM_CODE/Data_Processing/Binary_Conversion/

if [[ "$OSTYPE" == "linux-gnu" ]]; then
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${COOKING}
fi

nominal_HV(){
    echo $(grep " $1 " ${WM_ANALYSIS}Documents/HVScan.txt) | cut -d " " -f 7
}
