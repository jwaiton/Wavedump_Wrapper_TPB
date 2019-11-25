#!/bin/bash

# process binary file
# output .root version
# output cooked version
# simple analysis of cooked version

# copy this file to the location 
# at which processing and analysis 
# are to be performed

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

echo " Convert Binary to ROOT file? " 
echo " (Y/N)" 

read ANSWER

echo " ------------------------------"
date 
echo " ------------------------------"

if [[ "$ANSWER" == "Y" || "$ANSWER" == "y" ]]; then
    dat_to_root ${FILE_PATH}
    mkdir -p ./Plots/DAQ/

    echo " ------------------------------"
    date  
    echo " ------------------------------"
fi

echo " Cook Raw ROOT file ? " 
echo " (Y/N)" 

read ANSWER

echo " ------------------------------"
date  
echo " ------------------------------"

if [[ "$ANSWER" == "Y" || "$ANSWER" == "y" ]]; then
    
    if [[ "$OSTYPE" == "darwin"* ]]; then
	export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${WM_COOK}
    fi
    
    cook_raw ${FILE_PATH}.root
    
    
    if [[ "$OSTYPE" == "linux-gnu" ]]; then
	evince ./Plots/DAQ/hEventRate.pdf &
    elif [[ "$OSTYPE" == "darwin"* ]]; then
	open ./Plots/DAQ/hEventRate.pdf &
    fi
    
    echo " ------------------------------"
    date  
    echo " ------------------------------"
    
fi

echo " Analyse cooked file ? " 
echo " (Y/N)" 

read ANSWER

echo " ------------------------------"
date 
echo " ------------------------------"

if [[ "$ANSWER" == "Y" || "$ANSWER" == "y" ]]; then
    
    mkdir -p ./Plots/Noise/
    mkdir -p ./Plots/Timing/
    mkdir -p ./Plots/Dark/
    
    if [[ "$OSTYPE" == "darwin"* ]]; then
	export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${WM_ANALYSE_COOKED}
    fi 
    
    analyse_cooked  ${DIR_PATH}/Run*

    if [[ "$OSTYPE" == "linux-gnu" ]]; then
	evince ./Plots/Noise/hPeak_Cooked.pdf &
    elif [[ "$OSTYPE" == "darwin"* ]]; then
	open ./Plots/Noise/hPeak_Cooked.pdf &
    fi
    
    echo " ------------------------------"
    date  
    echo " ------------------------------"
    
fi


echo " Plot Waveform ? " 
echo " (Y/N)" 

read ANSWER

echo " ------------------------------"
date 
echo " ------------------------------"

if [[ "$ANSWER" == "Y" || "$ANSWER" == "y" ]]; then

    mkdir -p ./Plots/Waveforms/

    if [[ "$OSTYPE" == "darwin"* ]]; then
	export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${WM_ANALYSE}Waveform_Plotter/
    fi 
    
    (echo "b" && echo "R" && echo "X") | waveform_plotter ${DIR_PATH}/Run*
    
    if [[ "$OSTYPE" == "linux-gnu" ]]; then
	evince ./Plots/Waveforms/hWaveFFT.pdf &
    elif [[ "$OSTYPE" == "darwin"* ]]; then
	open ./Plots/Waveforms/hWaveFFT.pdf & 
    fi
    
    echo " ------------------------------"
    date  
    echo " ------------------------------"
    
fi



