#!/bin/sh

# Usage: run_nextraw.sh <game>

NEXTRAW_HOME=/d/Projects/spectrum/zxnext_bmp_tools/bin

GAME=games/$1

if [ -z "$GAME" ]
then
   echo "Game directory must be given"
   echo "Usage: run_nextraw.sh <game>"
   exit 1
fi

if [ ! -d $GAME ]
then
   echo "Game directory $GAME does not exist"
   exit 1
fi

if [ ! -d $GAME/images/rgb333 ]
then
   echo "Image source directory $GAME/images/rgb333 does not exist"
   exit 1
fi

if [ ! -d $GAME/gfx ]
then
   echo "Image destination directory $GAME/gfx does not exist"
   exit 1
fi

for image in $GAME/images/rgb333/??.bmp
do
   echo "Input:  $image"
   $NEXTRAW_HOME/nextraw $image $GAME/gfx/$(basename $image .bmp).nxi
   echo "Output: $GAME/gfx/$(basename $image .bmp).nxi"
   echo ""
done
