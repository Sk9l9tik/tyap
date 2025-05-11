PROJECT_NAME = laba5
NPROCS ?= $(shell nproc)

.PHONY:build
build:
	cmake -B build .
	cmake --build build -j $(NPROCS) 

.PHONY:start
start:
	./build/laba5 in.txt out.txt


.PHONY:clear
clear:
	rm -rf build