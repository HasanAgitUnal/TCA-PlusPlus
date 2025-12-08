WINCC_X64=x86_64-w64-mingw32-g++
WINCC_X86=i686-w64-mingw32-g++
CC=g++
INCLUDE_PATHS=-I<nlohmann_klasorunun_bulundugu_yol>

.PHONY: all clean test build-all windows-build linux-build

############################################################################################################################

default: clean test all

############################################################################################################################

build: all

all: windows linux
	@printf "> BUILD\t\t\t[   \033[0;32mCOMPLETED\033[0m   ]\n"
	@printf "\n> Files generated in build folder\”"

#--------------------------------------------------------------------------------------------------------------------------#

windows:
	@printf "> BUILD - WINDOWS\t[    RUNNING    ]\n"
	@mkdir -p build/exe

ifeq ($(OS),Windows_NT)
	-@g++ $(INCLUDE_PATHS) src/functions.cpp src/main.cpp -m32 -o "build/exe/tca++_x86.exe" || echo "> BUILD - WINDOWS    [    SKIPPING   ] : Failed to build 32 bit exe. Your gcc may doesn't support -m32 flag"
	@g++ $(INCLUDE_PATHS) src/functions.cpp src/main.cpp -o "build/exe/tca++_x64.exe"
else
	@$(WINCC_X86) $(INCLUDE_PATHS) src/functions.cpp src/main.cpp -o "build/exe/tca++_x86.exe"
	@$(WINCC_X64) $(INCLUDE_PATHS) src/functions.cpp src/main.cpp -o "build/exe/tca++_x64.exe"
endif

	@printf "> BUILD - WINDOWS\t[    \033[0;32mSUCCESS\033[0m    ]\n"

#--------------------------------------------------------------------------------------------------------------------------#

linux:

ifeq ($(OS),Windows_NT)
	@echo "> BUILD - LINUX    [    SKIPPING   ] : Can't build for Linux in Windows. Skipping Linux build.\n"
else
	@printf "> BUILD - LINUX\t\t[    RUNNING    ]"
	@mkdir -p "build/linux"
	@$(CC) $(INCLUDE_PATHS) src/functions.cpp src/main.cpp -o "build/linux/tca++"
	@printf "> BUILD - LINUX\t\t[    \033[0;32mSUCCESS\033[0m    ]\n"
endif

############################################################################################################################

test:
	@printf '> BUILD - TEST\t\t[    RUNNING    ]\n'

	@mkdir -p build/test

ifeq ($(OS),Windows_NT)
	@g++ $(INCLUDE_PATHS) src/functions.cpp test/test_functions.cpp -o build/test/unittest.exe -pthread -lgtest -lgtest_main
	@build/test/unittest.exe
else
	@g++ $(INCLUDE_PATHS) src/functions.cpp test/test_functions.cpp -o build/test/unittest -pthread -lgtest -lgtest_main
	@./build/test/unittest
endif

	@printf '> BUILD - TEST\t\t[    \033[32mSUCCESS\033[0m    ]\n'

############################################################################################################################

clean:
ifeq ($(OS),Windows_NT)
	-@rd /s /q build 2>NUL
else
	@rm -rf build 2>/dev/null
endif
	@printf "> BUILD - CLEAN \t[    \033[0;32mSUCCESS\033[0m    ]\n"
