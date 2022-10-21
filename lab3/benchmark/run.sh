#!/bin/bash

exFile=$1
testFile=$2
outpFile=$3
dim=$4
maxTh=$5


for ((i = 1; i < $maxTh; i++))
do
    $1 < $2 $dim $i -t
done

cat $outpFile
rm $outpFile