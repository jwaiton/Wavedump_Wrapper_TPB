#!/bin/bash

THIS_DIR=$pwd

RUN=$1

PMT=$2

LOC=0

TEST=N

ROOT_ARG='"Run_'$RUN'_PMT_'$PMT'_Loc_'$LOC'_Test_'$TEST'"'

COMMAND='./Fitting/GetPeakToValley.C++('$ROOT_ARG')'


( sleep 10 ; echo .q  ) | root -b 'GetPeakToValley.C++("Run_30_PMT_133_Loc_3_Test_S")';

