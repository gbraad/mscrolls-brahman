export PWD=`dirname $0`
export TOP=$PWD/../..
export APP=$TOP/app/release
export LD_LIBRARY_PATH=$APP:$LD_LIBRARY_PATH
$APP/klif -configdir . -story escape
