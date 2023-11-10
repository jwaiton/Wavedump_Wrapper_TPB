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

for dir in "${FILE_NAME[@]}"
do 
	echo ${dir}
	FILE_PATH=${DIR_PATH}${dir}

	echo "**************************"
	echo $FILE_PATH
	echo $FILE_NAME
	echo "*************************"

	dat_to_root ${FILE_PATH}
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
