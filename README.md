# Wavedump wrapper for TPB testing @ UoM
**10/11/2023**

Forked from WATCHMAN's Wavedump_Wrapper and modified for our own purposes here at UoM.

Instructions from the original repo:
( tested on Mac OS 10.13.6, 10.15.7, CentOS7, Scientific Linux 7.6, Ubuntu )

1) Access the Code. Clone this repository on your local machine.

2) Set up the environment. Modify and execute the following lines 
  ( recommended to add them to your .bashrc file ) :

$ export WD_DIR=/my/path/to/repository/parent/folder/
$ source ${WD_DIR}init.sh

Now several environment variables have been set. The environment variables 
generally start with 'WD_'. For example try:

$ clear ; echo $WD_SCRIPTS; echo ' '; ls -lrth $WD_SCRIPTS ; echo ' ';

3) Build all the binaries
  ( requires root to be installed and its usual environment set ). 
$ cd $WD_DIR 
$ ./build.sh

4) Run file conversion and basic analysis to generate the analysis files and folders in the local directory.

Have a read through $WD_PROCESS/process_analyse_locally_auto.sh

Cooking is not currently integrated in this system due to no input metadata or folder naming scheme

Wiki for the upstream repo is not exactly the same but may be a useful resource
https://github.com/Watchman-PMT/Wavedump_Wrapper/wiki
