#!/bin/bash

usage() {
    printf "Usage: <script> <runXTimes> <testFile> <testMode> <testNumThreads>"
}

run() {
    runXTimes=$1
    testFile=$2
    testMode=$3
    testNumThreads=$4

    if [[ $runXTimes = "" || $testFile = "" || $testMode = "" ]]; then
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
        execCommand="$execPath $execBaseArgs $testFile $testMode $testNumThreads"
        echo $execCommand
	outFileName=$outFileBaseName$(basename $testFile)-$testMode$testNumThreads-run$i.out
        $execCommand > $outFileName
        i=$(( $i + 1 ))
    done
}

main() {
    run $@
}

main $@
