#!/usr/bin/env bash

ROOT=$PWD/.build/initramfs

rm -frd $ROOT

cp -a $PWD/src/initramfs/. $ROOT/

mkdir -p $ROOT/{bin,boot,dev,etc,lib,mnt,opt,proc,root,run,sbin,sys,usr,var}
mkdir -p $ROOT/usr/{bin,include,lib,libexec,local,sbin,share,src}

