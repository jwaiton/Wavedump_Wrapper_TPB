#!/bin/bash 

HOME=$PWD

array=(*/)

for dir in "${array[@]}"
do echo $dir
cd $dir
subarray=(*/)
for subdir in "${subarray[@]}"
do echo "$subdir"
cd $subdir
#source process_analyse_locally_auto.sh &
source process_analyse_locally_auto.sh > output.txt &
cd ../
done
cd ../
done
cd $HOME

