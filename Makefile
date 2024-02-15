CC = g++

build:
	mkdir -p build
	cmake -S . -B build -Wno-dev
	cmake --build build 

run_pi: build
	build/pi/pi

run_pi_32: build
	build/pi/pi_32

test: build
	cd build && ctest

clean:
	rm -rf build

.PHONY: run test clean build