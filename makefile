all: compile-release

generate-release:
	@cmake --preset -release

generate-debug:
	@cmake --preset debug

compile-release: generate-release
	@cmake --build --preset release
	@cmake --install build/release --prefix install/release -v

compile-debug: generate-debug
	@cmake --build --preset debug
	@cmake --install build/debug --prefix install/debug -v

clean:
	@rm -rf build install

help:

.phony: all help clean generate-release generate-debug compile-release compile-debug
