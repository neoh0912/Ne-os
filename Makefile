.PHONY: init build clean

init:
	./scripts/init.sh
	./scripts/init/*.sh


build:
	./scripts/build/initramfs.sh
	./scripts/build/initramfs/*.sh

	gcc -c -fPIC src/lib/string.c -o src/lib/string.o
	gcc -shared -o .build/initramfs/lib/libstring.so src/lib/string.o

	gcc -c -fPIC src/lib/io.c -L./.build/initramfs/lib -lstring -o src/lib/io.o
	gcc -shared -o .build/initramfs/lib/libio.so src/lib/io.o

	gcc src/bin/program.c -L./.build/initramfs/lib -lc -lio -lstring -o .build/initramfs/bin/program
	./scripts/build/compress_initramfs.sh

clean:
	rm -fdr .build/initramfs
	rm src/lib/*.o
