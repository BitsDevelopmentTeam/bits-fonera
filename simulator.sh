#!/bin/bash

pushd bits_miosix/mxgui/tools/qtsimulator/build
cmake ..
make
popd

cd bitsd/src/build
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
