#!/bin/bash

inputDir=$1
outDir=$2

mkdir -p ${outDir}
for run in `find ${inputDir} -maxdepth 1 -type d | sed '1d' | sort`; do
    echo $run
    channels=`find $run -type f | xargs -I {} basename {}  | egrep '^C' | awk -F 'Trace' '{print $1}' | sort | uniq | sed -e 's/^C//g' | awk '{printf "%d ",$0}'`
    runName=`echo $run | sed -e 's%/$%%g' | awk -F '/' '{print $NF}'`
    bin/convertLeCroy ${outDir}/${runName}_waves.root ${run} ${channels}
#    find $run -type f 
done
