export PWD=`dirname $0`
export TOP=$PWD/../..
export APP=$TOP/app/debug
export LD_LIBRARY_PATH=$APP:$LD_LIBRARY_PATH
$APP/klif -d 3 -configdir . -story escape
