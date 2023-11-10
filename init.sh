export WD_DIR=/home/argonlab/Documents/NEXT/TPB/TPB_WD_Wrapper/
export WD_PROCESS=${WD_DIR}Processing/
export WD_ACQ=${WD_DIR}Acquisition/
export WD_SCRIPTS=${WD_DIR}Scripts/
export WD_CONVERT=${WD_PROCESS}data_to_root/
export WD_STORAGE=${WD_DIR}storage/

## dont know if these exports are needed, but may be useful?
export PATH=${PATH}:${WD_CONVERT}

if [[ "$OSTYPE" == "linux-gnu" ]]; then
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${WD_CONVERT}
fi

echo "Wavedump initialisation complete"
