#!/bin/bash

for j in 2 3 4 10 11 12 20 21 22 23
do
    ./BinToRoot ${j} < Gain.txt
    ./BinToRoot ${j} < Dark.txt
    ./BinToRoot ${j} < AP.txt
done

