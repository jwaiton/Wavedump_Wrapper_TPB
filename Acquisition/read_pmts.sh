#!/bin/bash

# to run first
# create the file below
# ( PMTs.txt )
# in which each new line
# is a PMT number in four
# digit format
# e.g. 
# 0015
# 0131
# ... etc
# put in order of channels/file numbers

input="../PMTs.txt"

declare -i nPMTs=0 

while IFS= read -r line
do
  PMTs+=("$line")
    
#  echo $line
#  echo $nPMTs
   echo "Channel $nPMTs PMT is ${PMTs[$nPMTs]}"

  nPMTs=nPMTs+1

done <"$input"


