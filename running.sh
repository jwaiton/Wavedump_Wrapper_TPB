#!/bin/bash

echo " ------------------------------"
date  
echo " ------------------------------"

export FILE_PATH=/Users/gsmith23/Desktop/Watchman/Testing/Wavedump_Wrapper/Data_Storage/Binary_Data/RUN000001/PMT0130/SPEtest/

dat_to_root ${FILE_PATH}wave_0.dat

echo " ------------------------------"
date 
echo " ------------------------------"

cd $WM_COOK
./make_plot_directories.sh
cook_raw ${FILE_PATH}wave_0.dat.root

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
analyse_cooked  ${FILE_PATH}/Run*

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

