#! /bin/bash
SCRIPT_DIR=`dirname $0`
BUILD_NUMBER=`cat $SCRIPT_DIR/build_number.h | awk '{print $3}'`
echo Current build number = $BUILD_NUMBER
BUILD_NUMBER=$(($BUILD_NUMBER+1))
echo New build number = $BUILD_NUMBER
echo '#define BUILD_NUMBER' $BUILD_NUMBER > $SCRIPT_DIR/build_number.h