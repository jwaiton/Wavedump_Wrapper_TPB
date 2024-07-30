echo " -------------------------------"
date  
echo " running  "
echo " process_analyse_locally_auto.sh "
echo " -------------------------------"

DIR_PATH=${PWD}/

##FILE_NAME=$(find . -maxdepth 1 -name "wave_*.dat" )
FILE_NAME=($(find . -maxdepth 1 -type f -name "wave_*.dat"))
echo "File paths:"
echo ${FILE_NAME}
echo ${FILE_NAME[@]}
echo ${FILE_NAME[0]}
echo ${FILE_NAME[1]}
echo "Checking dir"



# make sure there are files available
if [ ! -z "${FILE_NAME}" ]; then
	# read file type we're processing (automate this)
	read -p "Enter data-type (binary - bin, ascii - txt): " FILE_TYPE
else
	# default to bin if there are no files available, just for sanity.
	FILE_TYPE="bin" 
fi

	for dir in "${FILE_NAME[@]}"
	do 
		echo ${dir}
		FILE_PATH=${DIR_PATH}${dir}

		echo "**************************"
		echo $FILE_PATH
		echo $FILE_NAME
		echo "*************************"

		#dat_to_root ${FILE_PATH}
		python3 ${WD_PROCESS}/data_to_h5.py ${FILE_PATH} -t ${FILE_TYPE} -h5 -p 10000 -c
done
# this is edited out currently to see if we can use our python code to process the root files

##echo " ------------------------------"
##date 
##echo " ------------------------------"

##cook_raw ${FILE_PATH}.root

##echo " ------------------------------"
##date 
##echo " ------------------------------"

##analyse_cooked  ${DIR_PATH}/Run*

##echo " ------------------------------"
##date 
##echo " ------------------------------"
