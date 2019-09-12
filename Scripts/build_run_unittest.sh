#!/bin/bash

# save the repo directory
REPO=`pwd`

# build dir
BUILD_DIR=/run/user/$UID/PMOS_BUILD_UNITTEST

# remove exist directory 
if [ -d $BUILD_DIR ]; then
	rm -rf $BUILD_DIR 
fi

# create build directory
mkdir $BUILD_DIR && cd $BUILD_DIR 

# generate Makefile
/opt/qt-lib/qt-4.8.6/bin/qmake $REPO/UnitTest "CONFIG+=test"

# build and run
make -s




