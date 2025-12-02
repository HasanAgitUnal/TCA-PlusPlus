WINCC_X86=x86_64-w64-mingw32-g++
WINCC_X64=i686-w64-mingw32-g++
CC=g++

ifeq ($(OS),Windows_NT)
    SHELL := "C:\Program Files\Git\bin\bash.exe"
else
    SHELL := /bin/bash
endif

default: clean build-all

build-all: windows-build linux-build
	@echo -e "> BUILD\t\t\t[   \033[0;32mCOMPLETED\033[0m   ]"
	@echo -e "\n> Files generated in build folder"

windows-build: src/main.cpp src/ArgParser.hpp
	@echo -e "> BUILD - WINDOWS\t[    RUNNING    ]"

	@$(SHELL) -c "mkdir -p build/exe"
	@$(SHELL) -c '$(WINCC_X86) src/main.cpp -o "build/exe/tca++_x86.exe"'
	@$(SHELL) -c '$(WINCC_X64) src/main.cpp -o "build/exe/tca++_x64.exe"'

	@echo -e "> BUILD - WINDOWS\t[    \033[0;32mSUCCESS\033[0m    ]"

linux-build: src/main.cpp src/ArgParser.hpp
	@$(SHELL) -c 'tasklist > /dev/null 2>&1; if [ $$? -eq 0 ]; then \
		echo -e "> BUILD - LINUX\t\t[    SKIPPING    ] : Cant build for Linux in Windows. Skipping Linux build."; \
	else \
		echo -e "> BUILD - LINUX\t\t[    RUNNING    ]"; \
		mkdir -p "build/linux"; \
		$(CC) src/main.cpp -o "build/linux/tca++"; \
		echo -e "> BUILD - LINUX\t\t[    \033[0;32mSUCCESS\033[0m    ]"; \
	fi'

clean:
	@rm -rf build 1>/dev/null
	@echo -e "> BUILD - CLEAN \t[    \033[0;32mSUCCESS\033[0m    ]"

.PHONY: src/main.cpp src/ArgParser.hpp
