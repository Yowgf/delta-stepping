#!/bin/bash

usage() {
    printf "Usage: <script> <runXTimes> <testFile> <testMode> <delta> <testNumThreads>"
}

run() {
    runXTimes=$1
    testFile=$2
    testMode=$3
    testDelta=$4
    testNumThreads=$5

    if [[ $runXTimes = "" || $testFile = "" || $testMode = "" || $testDelta = "" ]]; then
        echo $(usage)
        exit 1
    fi

    execPath="./build/delta-stepping"
    testDir="time-tests"
    outFileBaseName=$testDir"/"
    execBaseArgs=""

    if [[ ! -d $testDir ]]; then
	mkdir $testDir
    fi
    
    i=0
    while (( i < $runXTimes )); do
        execCommand="$execPath $execBaseArgs $testFile $testMode $testDelta $testNumThreads"
        echo $execCommand
	outFileName=$outFileBaseName$(basename $testFile)-$testMode$testNumThreads-d$testDelta-run$i.out
        $execCommand > $outFileName
        i=$(( $i + 1 ))
    done
}

main() {
    run $@
}

main $@
