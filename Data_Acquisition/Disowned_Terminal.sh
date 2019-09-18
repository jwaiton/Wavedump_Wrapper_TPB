#!/bin/sh

i=0

while [ $i -lt 12 ]
do
	gnome-terminal -- sh -c "echo 'All Work & No Play Makes Jack A Dull Boy';sleep 5s; exit"
	i=$(( $i+1 ))
	echo $i
	sleep 10s

done&
disown

