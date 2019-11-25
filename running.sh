#!/bin/bash

# process binary file
# output .root version
# output cooked version
# simple analysis of cooked version

# recommended to instead use
# process_analyse_locally.sh

echo " ------------------------------"
date  
echo " ------------------------------"

echo " Enter full path to binary file, e.g.: " 
echo "/home/user1/Watchman/Wavedump_Wrapper/Data_Storage/Binary_Data/Setup/RUN000002/PMT0015/Nominal/" 

read DIR_PATH

echo " Enter file name e.g " 
echo " wave_0.dat" 

read FILE_NAME

FILE_PATH=${DIR_PATH}${FILE_NAME}

dat_to_root ${FILE_PATH}

echo " ------------------------------"
date 
echo " ------------------------------"

cd $WM_COOK
./make_plot_directories.sh
cook_raw ${FILE_PATH}.root

if [[ "$OSTYPE" == "linux-gnu" ]]; then
evince ${WM_COOK}Plots/DAQ/hEventRate.pdf &
elif [[ "$OSTYPE" == "darwin"* ]]; then
open ${WM_COOK}Plots/DAQ/hEventRate.pdf &
fi

echo " ------------------------------"
date 
echo " ------------------------------"

cd $WM_ANALYSE_COOKED
./make_plot_directories.sh
analyse_cooked  ${DIR_PATH}/Run*

echo " ------------------------------"
date 
echo " ------------------------------"

cd $WM_CODE

if [[ "$OSTYPE" == "linux-gnu" ]]; then
evince ${WM_ANALYSE_COOKED}Plots/Noise/hPeak_Cooked.pdf &
evince ${WM_ANALYSE_COOKED}Plots/Waveforms/hWaveFFT.pdf &
elif [[ "$OSTYPE" == "darwin"* ]]; then
open ${WM_ANALYSE_COOKED}Plots/Noise/hPeak_Cooked.pdf &
open ${WM_ANALYSE_COOKED}Plots/Waveforms/hWaveFFT.pdf & 
fi

