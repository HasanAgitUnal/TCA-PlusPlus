# Compilers
CXX = g++
WIN_CXX_X64 = x86_64-w64-mingw32-g++
WIN_CXX_X86 = i686-w64-mingw32-g++

# Directories
BUILD_DIR = build
WINX86_DIR = $(BUILD_DIR)/windows/x86
WINX64_DIR = $(BUILD_DIR)/windows/x64
LINUX_DIR = $(BUILD_DIR)/linux
TEST_DIR = $(BUILD_DIR)/test

# Source Files
SRC_FILES = src/functions.cpp src/main.cpp
TEST_SRC_FILES = src/functions.cpp test/test_functions.cpp

# App names
APP_NAME = tca++
APP_NAME_LINUX = $(APP_NAME)
APP_NAME_WIN= $(APP_NAME).exe
TEST_APP_NAME = unittest

# Flags and Settings
LDFLAGS_TEST = -pthread -lgtest -lgtest_main
INCLUDE = -Iinclude

.PHONY: all clean test windows linux

# Default target
default: clean test all

# Main build target
all: windows linux
	@printf "> BUILD\t\t\t[   \033[0;32mCOMPLETED\033[0m   ]\n"
	@printf "\n> Files generated in $(BUILD_DIR) folder\n"

# Windows cross-compilation from Linux
windows:
	@printf "> BUILD - WINDOWS\t[    RUNNING    ]\n"
	@mkdir -p $(WINX86_DIR)
	@mkdir -p $(WINX64_DIR)
	@$(WIN_CXX_X86) $(INCLUDE) $(SRC_FILES) -o "$(WINX86_DIR)/$(APP_NAME_WIN)"
	@$(WIN_CXX_X64) $(INCLUDE) $(SRC_FILES) -o "$(WINX64_DIR)/$(APP_NAME_WIN)"
	@printf "> BUILD - WINDOWS\t[    \033[0;32mSUCCESS\033[0m    ]\n"

windows-installer: windows
	@printf "> BUILD - WIN INSTALLER\t[    RUNNING    ]\n"
	@WINEDEBUG=-all wine "C:\\Program Files (x86)\\inno_setup\\ISCC.exe" installer.iss /O"build/windows" /F"tca++_setup.exe" >build/windows/installer.log
	@printf "> BUILD - WIN INSTALLER\t[    \033[0;32mSUCCESS\033[0m    ]\n"

# Linux native compilation
linux:
	@printf "> BUILD - LINUX\t\t[    RUNNING    ]\n"
	@mkdir -p $(LINUX_DIR)
	@$(CXX) $(INCLUDE) $(SRC_FILES) -o "$(LINUX_DIR)/$(APP_NAME_LINUX)"
	@printf "> BUILD - LINUX\t\t[    \033[0;32mSUCCESS\033[0m    ]\n"

# Test build and run
test:
	@printf "> BUILD - TEST\t\t[    RUNNING    ]\n\n"
	@mkdir -p $(TEST_DIR)
	@$(CXX) $(INCLUDE) $(TEST_SRC_FILES) -o $(TEST_DIR)/$(TEST_APP_NAME) $(LDFLAGS_TEST)
	@./$(TEST_DIR)/$(TEST_APP_NAME)
	@printf "\n> BUILD - TEST\t\t[    \033[32mSUCCESS\033[0m    ]\n"

# Clean target
clean:
	@rm -rf $(BUILD_DIR) 2>/dev/null
	@printf "> BUILD - CLEAN \t[    \033[0;32mSUCCESS\033[0m    ]\n"
