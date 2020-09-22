#!/bin/sh

# Create a lite Spectrum Next distro from a full distro.
# Usage: create_lite_distro.sh <spectrum-next-distro>.zip

DISTRO=$1

LITE_DISTRO=spectrum-next-lite-distro.zip

TMP=tmp

DISTRO_DIR=$TMP/distro

LITE_DISTRO_DIR=$TMP/lite-distro

if [ -z "$DISTRO" ]
then
   echo "Distro must be given"
   echo "Usage: create_lite_distro.sh <spectrum-next-distro>.zip"
   exit 1
fi

if [ ! -f $DISTRO ]
then
   echo "File $DISTRO does not exist"
   exit 1
fi

rm -rf $TMP
mkdir -p $DISTRO_DIR
unzip -q $DISTRO -d $DISTRO_DIR
mkdir -p $LITE_DISTRO_DIR
cp $DISTRO_DIR/TBBLUE.FW $LITE_DISTRO_DIR
cp $DISTRO_DIR/TBBLUE.TBU $LITE_DISTRO_DIR
cp -r $DISTRO_DIR/dot/ $LITE_DISTRO_DIR
cp -r $DISTRO_DIR/machines/ $LITE_DISTRO_DIR
cp -r $DISTRO_DIR/nextzxos/ $LITE_DISTRO_DIR
cp -r $DISTRO_DIR/sys/ $LITE_DISTRO_DIR
cp -r $DISTRO_DIR/tmp/ $LITE_DISTRO_DIR
rm -f $LITE_DISTRO_DIR/dot/CPM
rm -f $LITE_DISTRO_DIR/nextzxos/cpmbase.p3d
rm -rf $LITE_DISTRO_DIR/nextzxos/cpm/
rm -f $LITE_DISTRO
cd $LITE_DISTRO_DIR
zip -r -q ../../$LITE_DISTRO .
cd ../../
rm -rf $TMP
echo "Created lite Spectrum Next distro: $LITE_DISTRO"
