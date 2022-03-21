#
# To run this do:
# source run_cook_analyse_locally_auto_on_full_run.sh
#
THIS_PATH=$PWD
array=(*/)

for dir in "${array[@]}"
do echo $dir
   cd $dir
subarray=(*/)
for subdir in "${subarray[@]}"
do echo "$subdir"
   cd $subdir
   #source process_analyse_locally_auto.sh &
   source cook_analyse_locally_auto.sh > output.txt &
   cd ../
done
cd ../
done
cd $THIS_PATH
