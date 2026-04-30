#!/usr/bin/env bash

LINUX_BUILD_DIR=$PWD/build/linux
INITRAMFS_DIR=$PWD/build/initramfs.cpio.gz

qemu-system-x86_64 \
-m 512 \
-kernel $LINUX_BUILD_DIR/arch/x86/boot/bzImage \
-nographic \
-append "earlyprintk=serial,ttyS0 console=ttyS0" \
--initrd $INITRAMFS_DIR
