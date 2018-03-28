#!/bin/bash

if [ $# != 1 ]; then
	exit 1
fi

PRJROOT=$1

mkdir -p /tmp/ECGAlgo
cp $PRJROOT/Providers/TE3Provider/ECGAlgo/*.a /tmp/ECGAlgo
