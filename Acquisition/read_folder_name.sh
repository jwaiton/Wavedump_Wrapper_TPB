#!/bin/bash

filename="../Folder.txt"
n=1
while read line; do
# reading each line
echo "Line No. $n : $line"
FOLDER_NAME=$line
n=$((n+1))
done < $filename
