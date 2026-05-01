.PHONY: build clean

VPATH = src:.build/initramfs

LDFLAGS = -L./.build/initramfs/lib

OBJECTS = /lib/libstring.so /lib/libio.so \
		 /bin/program \
		 /bin/cd \
		 /bin/ls \
		 /init




build: initramfs.cpio.gz


initramfs.cpio.gz: $(OBJECTS)
	./scripts/build/initramfs_links
	./scripts/build/compress_initramfs


$(OBJECTS) : .build/initramfs

/bin/cd:
	$(CC) src/bin/cd.c $(LDFLAGS) -lc -o .build/initramfs/bin/cd

/bin/ls:
	$(CC) src/bin/ls.c $(LDFLAGS) -lc -o .build/initramfs/bin/ls

/bin/program:
	$(CC) src/bin/program.c $(LDFLAGS) -lc -o .build/initramfs/bin/program

/lib/libstring.so:
	$(CC) -c -fPIC src/lib/string.c -o src/lib/string.o
	$(CC) -shared -o .build/initramfs/lib/libstring.so src/lib/string.o

/init:
	$(CC) src/init.c $(LDFLAGS) -lc -o .build/initramfs/init

.build/initramfs:
	./scripts/build/initramfs



clean:
	rm -fdr .build/initramfs
	rm src/lib/*.o
