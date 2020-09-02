#!/bin/sh

# Usage: convert_text.sh <text-input-file>

JAVAHOME=C:/Java/jdk-11.0.5

TOOLHOME=`dirname $0`

if [ -z "$1" ]
then
   echo "Text input file must be given"
   echo "Usage: convert_text.sh <text-input-file>"
   exit 1
fi

$JAVAHOME/bin/java -jar $TOOLHOME/convert_text.jar $1
