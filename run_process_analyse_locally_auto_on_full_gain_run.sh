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
      subsubarray=(*/)
      for subsubdir in "${subsubarray[@]}"
      do echo "$subsubdir"
	 cd $subsubdir
	 ls
	 #source process_analyse_locally_auto.sh &
	 #source process_analyse_locally_auto.sh > output.txt &
	 source cook_analyse_locally_auto.sh > output.txt &
	 cd ../
      done
      cd ../
   done
   cd ../
done
cd $HOME

