#!/bin/sh
# script to run on OSX system with Qt installed

export PWD=`dirname $0`
export TOP=$PWD/../..
export APP=$TOP/app/debug
export DYLD_LIBRARY_PATH=$APP
appname=klif

# make sure correct game lib is there
cp ../debug/*.dylib $APP

$APP/${appname}.app/Contents/MacOS/${appname} -d 3 -configdir . -story escape.dat 
