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
APP_NAME_WININST= $(APP_NAME)_win_installer
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
# Portable binaries
	@printf "> BUILD - WINDOWS\t[    RUNNING    ]\n"
	@mkdir -p $(WINX86_DIR)
	@mkdir -p $(WINX64_DIR)
	@$(WIN_CXX_X86) $(INCLUDE) $(SRC_FILES) -o "$(WINX86_DIR)/$(APP_NAME_WIN)"
	@$(WIN_CXX_X64) $(INCLUDE) $(SRC_FILES) -o "$(WINX64_DIR)/$(APP_NAME_WIN)"
	@cp -r libs/* build/windows
	@cd $(WINX86_DIR) && zip tca++_x86_win_portable.zip * >/dev/null
	@cd $(WINX64_DIR) && zip tca++_x64_win_portable.zip * >/dev/null
	@printf "> BUILD - WINDOWS\t[    \033[0;32mSUCCESS\033[0m    ]\n"

# Installer
	@mkdir -p $(BUILD_DIR)/windows/installer
	@WINEDEBUG=-all wine "C:\\Program Files (x86)\\inno_setup\\ISCC.exe" Installer.iss /O"build/windows/installer" /F"$(APP_NAME_WININST)" >/tmp/tcapp_build.log || printf "> See full log at: /tmp/tcapp_build.log\n"

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
