.PHONY: linux init

init:
	./scripts/init.sh
	./scripts/init/*.sh

linux:
	./scripts/build/linux.sh
