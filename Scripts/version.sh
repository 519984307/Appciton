#!/bin/bash

if [ $# != 1 ]; then
	exit 1
fi

VERSION_FILE="$1/Main/Version.h"
TMP_VERSION_FILE="$1/Main/Version.h.tmp"

if [ -f ${TMP_VERSION_FILE} ];then
    rm ${TMP_VERSION_FILE}
fi

echo "#pragma once" >> ${TMP_VERSION_FILE}

cd $1
SVN_VERSION=`svnversion`

svn info 2> /dev/null 1> /dev/null
if [ $? -gt 0 ]; then
    #svn command not work, it might be a git-svn repo
    SVN_VERSION=`git svn info | grep Revision | awk '{print $2}'`
    DIRTY=`git status --porcelain | wc -l`
    if [ $DIRTY -gt 0 ]; then
        #dirty repo
        SVN_VERSION=`printf "%sM" "$SVN_VERSION"`
    else
        HEAD_COUNT=`git rev-list --count HEAD`
        if [ $SVN_VERSION != $HEAD_COUNT ]; then
            #git local version not match the remote version
            SVN_VERSION=`printf "%sM" "$SVN_VERSION"`
        fi
    fi
fi

echo "#define SVN_VERSION \"${SVN_VERSION}\"" >> ${TMP_VERSION_FILE}

BUILD_TIME=`date "+%b %d %Y %H:%M:%S"`
echo "#define BUILD_TIME \"${BUILD_TIME}\"" >> ${TMP_VERSION_FILE}

mv $TMP_VERSION_FILE $VERSION_FILE
