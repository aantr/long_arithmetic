CC = gcc
CXX = g++

build:
	mkdir -p build 
	CC=${CC} CXX=${CXX} cmake -S . -B build
	CC=${CC} CXX=${CXX} cmake --build build

run_pi: build
	build/pi/pi

run_pi_32: build
	build/pi/pi_32

test: build
	cd build && ctest

clean:
	rm -rf build

.PHONY: run test clean build