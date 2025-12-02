MINGW_X86=x86_64-w64-mingw32-g++
MINGW_X64=i686-w64-mingw32-g++

ifeq ($(OS),Windows_NT)
    SHELL := "C:\Program Files\Git\bin\bash.exe" 
else
    SHELL := /bin/bash
endif

default: build

build: windows-build linux-build
	@echo COMPLETED
	@echo Files generated in build folder

windows-build: main.cpp ArgParser.hpp
	$(SHELL) -c "mkdir -p build/exe"
	$(SHELL) -c '$(MINGW_X86) src/main.cpp -o "build/nt/tca++_x86.exe"'
	$(SHELL) -c '$(MINGW_X64) src/main.cpp -o "build/nt/tca++_x64.exe"'

linux-build: main.cpp ArgParser.hpp
	ifeq ($(OS),Windows_NT)
		@echo "W: Cant build for Linux in Windows. Skipping Linux build."
		@exit 0
	
	@mkdir -p "build/linux"
	g++ src/main.cpp -o "build/linux/tca++"
