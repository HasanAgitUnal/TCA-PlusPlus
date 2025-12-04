# TCA++ | A Custom Assembler for your CPU

TCA++ is a assembler for all architectures.
Mainly made for Turing Complete game.
If you made an cpu yourself and want to write programs for it you can use this assembler

## Install

For installing you should compiler from source code you need:

- `make`
- A C++ compiler:
        - `g++` for Linux
        - `MinGW` for Windows
- `git`

##### Building

1. First get source code
        ```bash
        git clone https://github.com/HasanAgitUnal/TCA-PlusPlus tcapp
        cd tcapp
        ```

2. Configure `include/Architecture.hpp` file for your CPU architecture
        Open `include/Architecture.hpp` with any text editor
        See [here](#configuring) for configuring guide

3. Compile it
        Make sure you downlaoded MinGW compiler on windows. Otherwise it wont work.

        Windows:
        ```bash
        make windows
        ```

        Linux:
        ```bash
        make linux
        ```

4. Adding to path
        Now you have tca++ under build folder
        For using add you should add it to the os path

        For linux just run this command:
        ```bash
        mkdir -p ~/.local/bin
        cp build/linux/tca++ ~/.local/bin
        ```

        and add this to your .bashrc
        ```bash
        export PATH='$HOME/.local/bin':$PATH
        ```

        On Windows open cmd and copy this:
        If your computer 64-bit:
        ```bash
        mkdir "C:\TCAPP-Bin"
        copy "build/exe/tca++_x64.exe" "C:\TCAPP-Bin"
        ```

        If your computer is 32-bit:
        ```
        mkdir "C:\TCAPP-Bin"
        copy "build/exe/tca++_x86.exe" "C:\TCAPP-Bin"
        ```

        For updating path windows is shittiy on terminal if command under there works run it as in a new cmd windows as **administrator**:
        ```bash
        setx /M PATH "%PATH%;C:\TCAPP-Bin"
        ```

        Then test in a new cmd window:
        ```bash
        tca++ --help
        ```
        If says command doesnt exists you should modify path from GUI
        For doing it:
        - Open start menu and search for "environment variables" and press enter.
        - At the opened popup click "Environment Variables" at bottom-right
        - Then chose Path under system variables and click edit
        - Click New and enter this: `C:\TCAPP-Bin`
        - Click OK -> OK -> Appyly -> OK
        - May need restart after that

        You should successfuly updated your path
        Test in a new terminal window:
        ```bash
        tca++ --help
        ```

        If it shouldnt say command not found or something similar

## Usage

You can also see usage with `tca++ --help`

Use in terminal: `tca++ FILES [-o OUTPUT] [-t OTYPE]` 

- `-o OUTPUT` : Output file (default: program)
- `-t OTYPE`  : Output type (default: binary)
- Supported OTYPEs:
        - `binary` : normal binary file (non-readable)
        - `text-binary` : a text file contains 0s and 1s
        - `hexadecimal` : a text file contains hexadecimal values

## Configuring

Configuring the Architecture.hpp file.
You can also read the comments in the file
Dont remove `#include` lines and variable definitions just edit varible content inside `{}` brackets or remove inside them
Default config is made according to the Turing Completes original architecture (+ RAM adress)

###### Small Varibles

- `INSTRUCTION_SIZE` : Instruction size for your commands
- `MAX_INT_SIZE` : Maximum integer argument bit size for commands

##### Command Opcodes

- Define them in `KEYWORDS` variable
- Format should be like:
        ```json
        ...... KEYWORDS = {
                {
                        "NAME",
                        {
                                {"binary", "01"},
                                {"arg_count", "1"},
                                {"valid_args", "int"}
                        }
                },

                // Other commands...
        }
        ```

        - `"NAME"` : Name of the command
        - `"binary"` : Value is your command opcode. If your command is doesnt takes arguments just enter full binary representation. or have spaces between arg enter opcode with the spaces ( as 0s ).
        - `"arg_count"` : Argument count of command
        - `"valid_args"` : Valid argument sets for arguments. see below for defining arg sets. You dont need to define int arg set it means integer already. you can use more than one arg set seperate them with `|` character. For example : "reg|int"

- You can now use NAME command like:
```asm
NAME 30  ; it will be 01011110
```

##### Argument Sets

- Define them `ARGS` variable
- Format should be like:
        ```json
        ...... ARGS = {
                {
                        "set_name",
                        {
                                {"a1", "000"},
                                {"a2", "001"}
                        }
                },

                // Other sets
        }
        ```

        - `"set_name"` : Name of the arg set
        - `{"a1", "001"}` : arg name and binary representation

- Now you can use the `set_name` arg set in your `KEYWORDS` varible. and use with commands:
```asm
EXAMPLE a1
```

