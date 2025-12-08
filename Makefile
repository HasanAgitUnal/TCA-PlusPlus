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

# Compiler and Linker Flags
CXXFLAGS = -Wall -Wextra -std=c++17
LDFLAGS_TEST = -pthread -lgtest -lgtest_main

.PHONY: all clean test windows linux

# Default target
default: clean test all

# Main build target
all: windows linux
	@echo -e "> BUILD\t\t\t[   \033[0;32mCOMPLETED\033[0m   ]"
	@echo -e "\n> Files generated in $(BUILD_DIR) folder"

# Windows cross-compilation from Linux
windows:
	@echo -e "> BUILD - WINDOWS\t[    RUNNING    ]"
	@mkdir -p $(EXE_DIR)
	@$(WIN_CXX_X86) $(CXXFLAGS) $(INCLUDE_PATHS) $(SRC_FILES) -o "$(EXE_DIR)/$(APP_NAME_WIN_X86)"
	@$(WIN_CXX_X64) $(CXXFLAGS) $(INCLUDE_PATHS) $(SRC_FILES) -o "$(EXE_DIR)/$(APP_NAME_WIN_X64)"
	@echo -e "> BUILD - WINDOWS\t[    \033[0;32mSUCCESS\033[0m    ]"

# Linux native compilation
linux:
	@echo -e "> BUILD - LINUX\t\t[    RUNNING    ]"
	@mkdir -p $(LINUX_DIR)
	@$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) $(SRC_FILES) -o "$(LINUX_DIR)/$(APP_NAME_LINUX)"
	@echo -e "> BUILD - LINUX\t\t[    \033[0;32mSUCCESS\033[0m    ]"

# Test build and run
test:
	@echo -e "> BUILD - TEST\t\t[    RUNNING    ]"
	@mkdir -p $(TEST_DIR)
	@$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) $(TEST_SRC_FILES) -o $(TEST_DIR)/$(TEST_APP_NAME) $(LDFLAGS_TEST)
	@./$(TEST_DIR)/$(TEST_APP_NAME)
	@echo -e "> BUILD - TEST\t\t[    \033[32mSUCCESS\033[0m    ]"

# Clean target
clean:
	@rm -rf $(BUILD_DIR) 2>/dev/null
	@echo -e "> BUILD - CLEAN \t[    \033[0;32mSUCCESS\033[0m    ]"
