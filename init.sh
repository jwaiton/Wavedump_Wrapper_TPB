export WD_DIR=/home/e78368jw/Documents/TPB_Project/Wavedump_Wrapper_TPB/
export WD_PROCESS=${WD_DIR}Processing/
export WD_ACQ=${WD_DIR}Acquisition/
export WD_SCRIPTS=${WD_DIR}Scripts/
export WD_CONVERT=${WD_PROCESS}data_to_root/
export WD_STORAGE=${WD_DIR}storage/
export WD_ANALYSE=${WD_DIR}Analyse/

## dont know if these exports are needed, but may be useful?
export PATH=${PATH}:${WD_CONVERT}

if [[ "$OSTYPE" == "linux-gnu" ]]; then
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${WD_CONVERT}
fi

## set pythonpath to parent

export PYTHONPATH=${WD_DIR}

echo "Wavedump initialisation complete"
