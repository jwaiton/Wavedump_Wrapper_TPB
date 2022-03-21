echo " -------------------------------"
date  
echo " running  "
echo " process_analyse_locally_auto.sh "
echo " -------------------------------"

DIR_PATH=${PWD}/

FILE_NAME=$(find . -maxdepth 1 -name "wave_*.dat" )

FILE_PATH=${DIR_PATH}${FILE_NAME}

echo "**************************"
echo $FILE_PATH
echo $FILE_NAME
echo "*************************"

dat_to_root ${FILE_PATH}

echo " ------------------------------"
date 
echo " ------------------------------"

cook_raw ${FILE_PATH}.root

echo " ------------------------------"
date 
echo " ------------------------------"

analyse_cooked  ${DIR_PATH}/Run*

echo " ------------------------------"
date 
echo " ------------------------------"
