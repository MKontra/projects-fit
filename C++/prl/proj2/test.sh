#!/bin/bash
if [ $# -lt 1 ];then 				
    numbers=`wc -c numbers | awk '{ print ($1-3) }'`
else
    numbers=$1;
    numbers=$(($numbers*2));
    numbers=$(($numbers-1));
fi;

mpic++ --prefix /usr/local/share/OpenMPI -o ba ba.cpp	
mpirun --prefix /usr/local/share/OpenMPI -np $numbers ba
rm -rf ba