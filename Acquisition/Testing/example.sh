#!/bin/bash

test="gain"

if [ $test = "test" ]
then
    length=0.1s
    pause=0.1s
elif [ $test = "noise" ]
then
    length=100s
elif [ $test = "gain" ]
then
    length=100s
elif [ $test = "after" ]
then
    length=100s
fi

echo $length
