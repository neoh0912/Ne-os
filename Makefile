VPATH = src:.build/initramfs/

LDFLAGS = -L./.build/initramfs/lib -I./include -lc -lutils

OBJECTS = lib/libstring.so \
		 lib/libutils.so \
		 lib/libdynamic.so \
	     bin/cat bin/mount \
		 bin/cd bin/ls \
		 init


lib/%.o: lib/%.c
	@printf "O\t"
	$(CC) -c -fPIC $(LDFLAGS) $< -o src/$@

lib/lib%.so: lib/%.o
	@printf "SO\t"
	$(CC) -shared -o .build/initramfs/$@ src/$<

bin/%: bin/%.c
	@printf "CC\t"
	$(CC) $< $(LDFLAGS) -o .build/initramfs/$@

.PHONY: build
build: initramfs.cpio.gz

initramfs.cpio.gz: $(OBJECTS)
	./scripts/build/initramfs_links
	./scripts/build/compress_initramfs

$(OBJECTS) : .build/initramfs

bin/mount: lib/libutils.so

bin/ls: lib/libutils.so lib/libdynamic.so

init:
	$(CC) src/init.c $(LDFLAGS) -lc -o .build/initramfs/init

.PHONY: .build/initramfs
.build/initramfs: is_clean
	./scripts/build/initramfs
	touch .build/dirty

.PHONY: is_clean
is_clean:
	@test ! -e .build/dirty || { echo "Cleaning before build"; make clean; make build; exit 1; }

.PHONY: clean
clean:
	rm -frd .build/initramfs
	find . -type f -name "*.o" -print -delete
	rm -f .build/dirty

