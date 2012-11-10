#!/bin/bash

QTSIMDIR=bits_miosix/mxgui/tools/qtsimulator/build
BITSDIR=bitsd/src/build

[ -d $QTSIMDIR ] || mkdir $QTSIMDIR
pushd $QTSIMDIR
cmake ..
make
popd

[ -d $BITSDIR ] mkdir $BITSDIR
cd $BITSDIR
cmake ..
make

trap ctrl_c INT

ctrl_c() {
	killall bitsd qtsimulator
	rm deleteme
	exit
}

mkfifo deleteme
./bitsd < deleteme | ./../../../bits_miosix/mxgui/tools/qtsimulator/build/qtsimulator > deleteme
rm deleteme
