#!/usr/bin/env bash

DIR=$PWD/.build/initramfs
BACK=$PWD

cd $DIR/bin
ln -s bash sh
cd $BACK
