#!/bin/sh

# Usage: run_nextbmp.sh <game>

NEXTBMP_HOME=/d/Projects/spectrum/zxnext_bmp_tools/bin

GAME=games/$1

if [ -z "$GAME" ]
then
   echo "Game directory must be given"
   echo "Usage: run_nextbmp.sh <game>"
   exit 1
fi

if [ ! -d $GAME ]
then
   echo "Game directory $GAME does not exist"
   exit 1
fi

if [ ! -d $GAME/images/rgb888 ]
then
   echo "Image source directory $GAME/images/rgb888 does not exist"
   exit 1
fi

if [ ! -d $GAME/images/rgb333 ]
then
   echo "Image destination directory $GAME/images/rgb333 does not exist"
   exit 1
fi

for image in $GAME/images/rgb888/??.bmp
do
   echo "Input:  $image"
   $NEXTBMP_HOME/nextbmp -ceil -min-palette $image $GAME/images/rgb333/$(basename $image)
   echo "Output: $GAME/images/rgb333/$(basename $image)"
   echo ""
done
