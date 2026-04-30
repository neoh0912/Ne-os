#!/usr/bin/env bash

OUTPUT_DIR=$PWD/build/initramfs
DIR=$PWD

CORE_UTILS_DIR=$PWD/build/coreutils
BASH_DIR=$PWD/build/bash
GREP_DIR=$PWD/build/grep

rm -frd $OUTPUT_DIR

cp -a $PWD/src/initramfs/. $OUTPUT_DIR/

cp -a $CORE_UTILS_DIR/share/. $OUTPUT_DIR/usr/share
cp -a $CORE_UTILS_DIR/bin/. $OUTPUT_DIR/bin

cp -a $BASH_DIR/share/. $OUTPUT_DIR/usr/share
cp -a $BASH_DIR/bin/. $OUTPUT_DIR/bin

cp -a $GREP_DIR/share/. $OUTPUT_DIR/usr/share
cp -a $GREP_DIR/bin/. $OUTPUT_DIR/bin

./scripts/initramfs_builders/*

cd $OUTPUT_DIR
find . -print0 | cpio --null --create --verbose --format=newc | gzip --best > ../initramfs.cpio.gz
cd $DIR
