WINCC_X64=x86_64-w64-mingw32-g++
WINCC_X86=i686-w64-mingw32-g++
CC=g++

.PHONY: all clean test build-all windows-build linux-build

############################################################################################################################

default: clean test all

############################################################################################################################

build: all

all: windows linux
	@echo -e "> BUILD\t\t\t[   \033[0;32mCOMPLETED\033[0m   ]"
	@echo -e "\n> Files generated in build folder"

#--------------------------------------------------------------------------------------------------------------------------#

windows:
	@echo -e "> BUILD - WINDOWS\t[    RUNNING    ]"
	
	@mkdir -p build/exe
	
	
ifeq ($(OS),Windows_NT)
	-@g++ src/functions.cpp src/main.cpp -m32 -o "build/exe/tca++_x86.exe" || echo - e "> BUILD - WINDOWS\t[    SKIPPING   ] : Failed to build 32 bit exe. Your gcc may doesn't support -m32 flag"
	@g++ src/functions.cpp src/main.cpp -o "build/exe/tca++_x64.exe"
else
	@$(WINCC_X86) src/functions.cpp src/main.cpp -o "build/exe/tca++_x86.exe"
	@$(WINCC_X64) src/functions.cpp src/main.cpp -o "build/exe/tca++_x64.exe"
endif

	@echo -e "> BUILD - WINDOWS\t[    \033[0;32mSUCCESS\033[0m    ]"

#--------------------------------------------------------------------------------------------------------------------------#

linux:

ifeq ($(OS),Windows_NT)
	@echo -e "> BUILD - LINUX\t\t[    SKIPPING   ] : Can't build for Linux in Windows. Skipping Linux build."
else
	@echo -e "> BUILD - LINUX\t\t[    RUNNING    ]"
	@mkdir -p "build/linux"
	@$(CC) src/functions.cpp src/main.cpp -o "build/linux/tca++"
	@echo -e "> BUILD - LINUX\t\t[    \033[0;32mSUCCESS\033[0m    ]"
endif

############################################################################################################################

test:
	@echo -e '> BUILD - TEST\t\t[    RUNNING    ]'

	@mkdir -p build/test

ifeq ($(OS),Windows_NT)
	@g++ src/functions.cpp test/test_main.cpp -o build/test/unittest.exe -pthread -lgtest -lgtest_main
	@build/test/unittest.exe
else
	@g++ src/functions.cpp test/test_main.cpp -o build/test/unittest -pthread -lgtest -lgtest_main
	@./build/test/unittest
endif

	@echo -e '> BUILD - TEST\t\t[    \033[32mSUCCESS\033[0m    ]'

############################################################################################################################

clean:
	@rm -rf build 2>nul || rm -rf build
	@echo -e "> BUILD - CLEAN \t[    \033[0;32mSUCCESS\033[0m    ]"