#!/bin/sh

# Usage: trim_prog.sh <prog-input-file>

JAVAHOME=C:/Java/jdk-11.0.5

TOOLHOME=`dirname $0`

if [ -z "$1" ]
then
   echo "Prog input file must be given"
   echo "Usage: trim_prog.sh <prog-input-file>"
   exit 1
fi

$JAVAHOME/bin/java -jar $TOOLHOME/trim_prog.jar $1
