WINCC_X86=x86_64-w64-mingw32-g++
WINCC_X64=i686-w64-mingw32-g++
CC=g++

.PHONY: all clean test build-all windows-build linux-build


ifeq ($(OS),Windows_NT)
    SHELL := "C:\Program Files\Git\bin\bash.exe"
else
    SHELL := /bin/bash
endif

############################################################################################################################

default: clean test build-all

############################################################################################################################

build-all: windows-build linux-build
	@echo -e "> BUILD\t\t\t[   \033[0;32mCOMPLETED\033[0m   ]"
	@echo -e "\n> Files generated in build folder"

#--------------------------------------------------------------------------------------------------------------------------#

windows-build:
	@echo -e "> BUILD - WINDOWS\t[    RUNNING    ]"

	@$(SHELL) -c "mkdir -p build/exe"
	@$(SHELL) -c '$(WINCC_X86) src/functions.cpp src/main.cpp -o "build/exe/tca++_x86.exe"'
	@$(SHELL) -c '$(WINCC_X64) src/functions.cpp src/main.cpp -o "build/exe/tca++_x64.exe"'

	@echo -e "> BUILD - WINDOWS\t[    \033[0;32mSUCCESS\033[0m    ]"

#--------------------------------------------------------------------------------------------------------------------------#

linux-build:
	@$(SHELL) -c 'tasklist > /dev/null 2>&1; if [ $$? -eq 0 ]; then \
		echo -e "> BUILD - LINUX\t\t[    SKIPPING    ] : Cant build for Linux in Windows. Skipping Linux build."; \
	else \
		echo -e "> BUILD - LINUX\t\t[    RUNNING    ]"; \
		mkdir -p "build/linux"; \
		$(CC) src/functions.cpp src/main.cpp -o "build/linux/tca++"; \
		echo -e "> BUILD - LINUX\t\t[    \033[0;32mSUCCESS\033[0m    ]"; \
	fi'

############################################################################################################################

test:
	@$(SHELL) -c "mkdir -p build/test"

	@$(SHELL) -c 'g++ src/functions.cpp test/test_main.cpp -o build/test/test -pthread -lgtest -lgtest_main'

	@$(SHELL) -c './build/test/test'

############################################################################################################################

clean:
	@rm -rf build 1>/dev/null
	@echo -e "> BUILD - CLEAN \t[    \033[0;32mSUCCESS\033[0m    ]"
