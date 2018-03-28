#!/bin/bash

STRIP=strip

if [ $# -eq 0 ]; then
	QMAKE=/opt/qt-lib/qt-4.8.6/bin/qmake
elif [ $# -gt 0 ]; then
	case $1 in
		-pc)
			QMAKE=/opt/qt-lib/qt-4.8.6/bin/qmake
			;;
		-qvfb)
			QMAKE=/opt/qt-lib/qt-4.8.6-qvfb/bin/qmake
			;;
		-target)
			QMAKE=/opt/qt-lib/qt-4.8.6-arm-cortexa9hf/bin/qmake
			STRIP=arm-poky-linux-gnueabi-strip
			;;
		-h)
			echo "$0 [-pc|-qvfb|-target [-static]]"
			exit
			;;
		*)
			QMAKE=/opt/qt-lib/qt-4.8.6/bin/qmake
			;;
	esac
fi

if [ $# -gt 1 ]; then
	if [ $1 == "-target" ] && [ $2 == "-static" ]; then
		QMAKE=/opt/qt-lib/qt-4.8.6-arm-cortexa9hf-static/bin/qmake
	fi
fi

# build iDM
#make clean
$QMAKE && make

# Strip files
$STRIP iDM/bin/*
