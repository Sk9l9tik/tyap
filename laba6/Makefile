PROJECT_NAME = laba6
NPROCS ?= $(shell nproc)

.PHONY:build
build:
	cmake -B build .
	cmake --build build -j $(NPROCS)

.PHONY:start
start:
	./build/laba4


.PHONY:clear
clear:
	rm -rf build