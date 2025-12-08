# Compilers
CXX = g++
WIN_CXX_X64 = x86_64-w64-mingw32-g++
WIN_CXX_X86 = i686-w64-mingw32-g++

# Directories
BUILD_DIR = build
EXE_DIR = $(BUILD_DIR)/exe
LINUX_DIR = $(BUILD_DIR)/linux
TEST_DIR = $(BUILD_DIR)/test

# Source Files
SRC_FILES = src/functions.cpp src/main.cpp
TEST_SRC_FILES = src/functions.cpp test/test_functions.cpp

# Flags and Settings
LDFLAGS_TEST = -pthread -lgtest -lgtest_main
INCLUDE_PATHS = -I/usr/include

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
	@mkdir -p $(EXE_DIR)
	@$(WIN_CXX_X86) $(INCLUDE_PATHS) $(SRC_FILES) -o "$(EXE_DIR)/tca++_x86.exe"
	@$(WIN_CXX_X64) $(INCLUDE_PATHS) $(SRC_FILES) -o "$(EXE_DIR)/tca++_x64.exe"
	@printf "> BUILD - WINDOWS\t[    \033[0;32mSUCCESS\033[0m    ]\n"

# Linux native compilation
linux:
	@printf "> BUILD - LINUX\t\t[    RUNNING    ]\n"
	@mkdir -p $(LINUX_DIR)
	@$(CXX) $(INCLUDE_PATHS) $(SRC_FILES) -o "$(LINUX_DIR)/tca++"
	@printf "> BUILD - LINUX\t\t[    \033[0;32mSUCCESS\033[0m    ]\n"

# Test build and run
test:
	@printf "> BUILD - TEST\t\t[    RUNNING    ]\n"
	@mkdir -p $(TEST_DIR)
	@$(CXX) $(INCLUDE_PATHS) $(TEST_SRC_FILES) -o $(TEST_DIR)/unittest $(LDFLAGS_TEST)
	@./$(TEST_DIR)/unittest
	@printf "> BUILD - TEST\t\t[    \033[32mSUCCESS\033[0m    ]\n"

# Clean target
clean:
	@rm -rf $(BUILD_DIR) 2>/dev/null
	@printf "> BUILD - CLEAN \t[    \033[0;32mSUCCESS\033[0m    ]\n"
