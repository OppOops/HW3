#!/bin/bash
PROGNAME=Benchmark

make
util/dx --dex --output=${PROGNAME}.dex ${PROGNAME}.class
make clean
./convert.sh ${PROGNAME}
