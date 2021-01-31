export TOP=../../..
export APP=$TOP/app/debug
export LD_LIBRARY_PATH=../../debug:$APP:$LD_LIBRARY_PATH
$APP/klif -configdir . -d 3 -story murder $1 $2 $3
