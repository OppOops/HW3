#!/bin/bash

./util/baksmali $1.dex
./util/smali out/ --output $1-move73.dex
