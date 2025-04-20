PROJECT_NAME = laba6
NPROCS ?= $(shell nproc)
INPUT = in
OUTPUT = out

.PHONY:build
build:
	cmake -B build .
	cmake --build build -j $(NPROCS)

.PHONY:start
start:
	./build/${PROJECT_NAME} ${INPUT} ${OUTPUT}


.PHONY:clear
clear:
	rm -rf build