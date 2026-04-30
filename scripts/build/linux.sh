#!/usr/bin/env bash

OUTPUT_DIR=$PWD/.build/linux
INPUT_DIR=$PWD/linux
CONFIG_DIR=$PWD/configs/linux

make -C $INPUT_DIR O=$OUTPUT_DIR mrproper

if [ ! -f $CONFIG_DIR ]; then
    make -C $INPUT_DIR O=$OUTPUT_DIR allnoconfig
    make -C $INPUT_DIR O=$OUTPUT_DIR menuconfig
    cp $OUTPUT_DIR/.config $CONFIG_DIR
else
    cp $CONFIG_DIR $OUTPUT_DIR/.config
fi

make -C $INPUT_DIR O=$OUTPUT_DIR -j $(nproc --all)
