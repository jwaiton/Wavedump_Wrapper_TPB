#!/bin/bash

PARENT=18mar22
RUN=000003
FOLDER=${WM_DATA}/${PARENT}/RUN${RUN}/
GAIN_HISTODIR=${FOLDER}/Histograms

echo $FOLDER

mkdir -pv ${GAIN_HISTODIR}

source ../read_pmts.sh

for i in 4 5 6 7
do
    for j in 1 2 3 4 5
    do    
        cp ${FOLDER}/PMT${PMTs[$i]}/Gain/STEP0${j}/hQ*.root ${GAIN_HISTODIR}/
    done
    # This bit I am not sure how to do
    cd ${WM_ANALYSIS}/Gain/
    ./Gain_Fit_singlePMT
    echo "${PMTs[$i]}"
    echo "${i}"
    echo "${RUN}"
    mv ./*.png ${GAIN_HISTODIR}
    cd ${FOLDER}
done

