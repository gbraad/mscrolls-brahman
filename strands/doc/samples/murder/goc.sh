export TOP=../../..
export APP=$TOP/app/debug
export LD_LIBRARY_PATH=$APP:$LD_LIBRARY_PATH
../../cli/debug/ificonsole -d 3 -configdir . -story murder $1 $2 $3 $4
