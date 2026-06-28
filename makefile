WENG_WLOG_ENABLE?=1
CMAKE_C_COMPILER?=clang
CMAKE_CXX_COMPILER?=clang++

all: compile-release

generate-source-files:
	@emacs --batch --quick												\
	--eval "(require 'org)"												\
	--eval "(org-babel-load-file \"$(realpath org/InitSession.org)\")"	\
	--eval "(wng/evaluate-module-org-files (wng/find-module-org-files \"$(realpath Source)\"))"

generate-release: generate-source-files
	@cmake --preset Release						\
		-DWENG_WLOG_ENABLE=${WENG_WLOG_ENABLE}	\
		-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}

generate-debug: generate-source-files
	@cmake --preset Debug						\
		-DWUNITTEST=1							\
		-DWENG_WLOG_ENABLE=${WENG_WLOG_ENABLE}	\
		-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}

compile-release: generate-release
	@cmake --build --preset Release 
	@cmake --install Build/Release --prefix Install/Release -v

compile-debug: generate-debug
	@cmake --build --preset Debug
	@cmake --install Build/Debug --prefix Install/Debug -v

clean:
	@rm -rf Build Install

help:
	@echo "Makefile options:"
	@echo "  compile-release          Compile in release mode."
	@echo "  compile-debug            Compile in debug mode."

.phony: all help clean generate-release generate-debug compile-release compile-debug
