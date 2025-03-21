PROJECT_NAME = laba3
NPROCS ?= $(shell nproc)

.PHONY:build
build:
	cmake -B build .
	cmake --build build -j $(NPROCS)

.PHONY:start
start:
	./build/laba3


.PHONY:clear
clear:
	rm -rf build