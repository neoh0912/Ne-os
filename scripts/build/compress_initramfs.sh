#!/usr/bin/env bash

OUTPUT_DIR=$PWD/.build/initramfs
DIR=$PWD

cd $OUTPUT_DIR
find . -print0 | cpio --null --create --verbose --format=newc | gzip --best > ../initramfs.cpio.gz
cd $DIR
