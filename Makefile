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

# App names
APP_NAME = tcapp
APP_NAME_LINUX = $(APP_NAME)
APP_NAME_WIN_X86 = $(APP_NAME)_x86.exe
APP_NAME_WIN_X64 = $(APP_NAME)_x64.exe
TEST_APP_NAME = test_runner

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
	@mkdir -p $(EXE_DIR)
	@$(WIN_CXX_X86) $(INCLUDE) $(SRC_FILES) -o "$(EXE_DIR)/$(APP_NAME_WIN_X86)"
	@$(WIN_CXX_X64) $(INCLUDE) $(SRC_FILES) -o "$(EXE_DIR)/$(APP_NAME_WIN_X64)"
	@printf "> BUILD - WINDOWS\t[    \033[0;32mSUCCESS\033[0m    ]\n"

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
