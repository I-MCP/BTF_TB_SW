#!/bin/bash

launchDir=`pwd`
logDir=`pwd`/log
jobsDir=`pwd`/jobs
shapeFile=`pwd`/data/fitShapes_iMCP_BTF_afterRun240.root

inputDir=$1
outputDir=$2
queue=$3
version=$4
run=$5

files=`find ${inputDir} -type f -iname "run*.root" | sort -n`

mkdir -p ${logDir}
mkdir -p ${jobsDir}
mkdir -p ${outputDir}/${version}

for file in $files; do
    fileName=`basename ${file%.*}`
    echo "Producing script for recoing $fileName"
    cat > ${jobsDir}/${fileName}_job.csh <<EOF
#!/bin/tcsh
#
# Script generated by launchRecoProduction.sh @ `date`
# 
# Reco job for file $file
#

# Setup CMSSW environment (boost libraries)
cd ${CMSSW_BASE}
eval \`scram runtime -csh\`

# Setup BTF_SW environment
cd ${launchDir}
source setup.csh

scripts/runSimpleRecoTreeProducer.py -i ${file} -p ${shapeFile} -L ${launchDir}/lib -o ${outputDir}/${version}
EOF

#Launch the script in batch
if [ "$run" == "1" ]; then
    bsub -q ${queue} -o ${logDir}/${fileName}.log -e ${logDir}/${fileName}.log < ${jobsDir}/${fileName}_job.csh
fi
done

