# HV_Stepper
A HV stepping program for WATCHMAN PMT's

## Compiling:

To compile this program, simply run the make command whilst in the folder containing the makefile.

The Makefile contained here is a bit of a mess, so may not work perfectly. If it does not, run the following commands:

$ export root_lib="$ROOTSYS/lib"

$ g++ -std=c++11  -I${root_lib} readHam.cc -o readHam -L${root_lib} -Wl,-rpath,${root_lib} -lCore -lRIO -lRooFit -lRooFitCore -lRooStats -lHist -lTree -lMatrix -lPhysics -lMathCore

Where $ROOTSYS is an environment variable set by env.sh from WMUtils. If you don't have WMUtils, run:

export root_lib="path/to/root/lib"

## Issues:

Any issues will appear here.


