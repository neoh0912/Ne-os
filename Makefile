.PHONY: init

init:
	./scripts/init.sh
	./scripts/init/*.sh

initramfs:
	./scripts/build/initramfs.sh
	./scripts/build/initramfs/*.sh
	./scripts/build/compress_initramfs.sh
