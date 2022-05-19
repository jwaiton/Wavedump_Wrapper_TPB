#!/bin/bash 

# Setup for trigger jitter setup, doesnt analyse the trigger data 

# setting new home directory to RUN0000NN
HERE=$PWD

# First run for PMT0000
##cd PMT0000

# Plot out different folders within subarray
##subarray=(*/)
# Moves into Nominal folder for each, presumably works for more folders hence the for
##for subd in "${subarray[@]}"
##do 
##    echo "$subd"
##    cd $subd
##    source process_trig_locally_auto.sh &
##    echo "Cooking Trigger"
# Jump out of specific folder before going again
##cd ../
##done
# Jump out of PMT0000
##cd ../

# Next to rest of PMTs, can take loop
array=(*/)
# Remove first element (should always be PMT0000, if not fix this)
##array=("${array[@]:1}")

for dir in "${array[@]}"
do echo $dir
cd $dir
subarray=(*/)
for subdir in "${subarray[@]}"
do echo "$subdir"
cd $subdir
source process_analyse_locally_auto.sh &
echo "Cooking PMT Data"
cd ../
done
cd ../
done
cd $HERE

