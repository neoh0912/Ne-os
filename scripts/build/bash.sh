#!/usr/bin/env bash

OUTPUT_DIR=$PWD/build/bash
INPUT_DIR=$PWD/include/bash-5.3
DIR=$PWD

rm -frd $OUTPUT_DIR

cd $INPUT_DIR
./configure -C --enable-static-link --without-bash-malloc --prefix=$OUTPUT_DIR
make clean
make -j $(nproc --all)
make install -j $(nproc --all)
cd $PWD

