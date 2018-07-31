#!/bin/sh
# script to run on OSX system with Qt installed

export PWD=`dirname $0`
export TOP=$PWD/../..
export APP=$TOP/app/release
export DYLD_LIBRARY_PATH=$APP
appname=klif

cp ../release/*.dylib $APP
$APP/${appname}.app/Contents/MacOS/${appname} -configdir . -story escape.dat 
