#!/usr/bin/env bash

BZIMAGE=$PWD/bzImage
INITRAMFS=$PWD/.build/initramfs.cpio.gz

qemu-system-x86_64 \
-m 512 \
-kernel $BZIMAGE \
-nographic \
-append "console=ttyS0" \
--initrd $INITRAMFS
