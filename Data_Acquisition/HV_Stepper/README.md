# HV_Stepper
A HV stepping program for WATCHMAN PMT's

## Compiling:

To compile this program, simply run the make command whilst in the folder containing the makefile.

The Makefile contained here is a bit of a mess, so may not work perfectly. If it does not, run the following commands:

$ export root_lib="path/to/root/lib"

$ g++ -std=c++11  -I${root_lib} readHam.cc -o readHam -L${root_lib} -Wl,-rpath,${root_lib} -lCore -lRIO -lRooFit -lRooFitCore -lRooStats -lHist -lTree -lMatrix -lPhysics -lMathCore

## Issues:

An issue was encountered where the program could execute perfectly when run from one folder, then fail to execute when run from a different folder.

## Usage

The command to use this program is as follows:
./readHam <options> [args]
  
Options:

-i <filepath>: the path to the input file
-o <filepath>: the path to the output file
-r <no argument>: recreate the program.
-g <no argument>: create voltage steps flat in gain rather than voltage.


