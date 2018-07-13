#!/bin/bash

clean() {
    echo $1
    rm -f $1/.qmake.stash $1/*.user $1/Makefile $1/Makefile.* $1/object_script.*
    rm -f $1/brahman.log $1/TAGS $1/*~ 
}

cleanout() {
    echo $1
    rm -f $1/*.o $1/*.dll $1/moc_*.* $1/*.dll $1/*.exe $1/qrc_*.* $1/*.a
}

clean "."

clean "api"
cleanout "api/debug"
cleanout "api/release"

clean "app"
cleanout "app/debug"
cleanout "app/release"

clean "ifi"
cleanout "ifi/debug"
cleanout "ifi/release"

clean "xvan"
cleanout "xvan/debug"
cleanout "xvan/release"

clean "xvan/xc"
cleanout "xvan/xc/debug"
cleanout "xvan/xc/release"

clean "tools/apng/libpng_apng/libpng"
cleanout "tools/apng/libpng_apng/libpng/debug"
cleanout "tools/apng/libpng_apng/libpng/release"

clean "tools/zlib"
cleanout "tools/zlib/debug"
cleanout "tools/zlib/release"

rm -f xvan/escape/src/*.dat



