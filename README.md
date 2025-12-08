# TCA++ | A Custom Assembler for Your CPU

TCA++ is an assembler for all architectures, primarily developed for the game Turing Complete. If you have designed your own CPU and need an assembler, this might be it.

Licensed under the GNU Affero General Public License v3.0 (AGPLv3). See the [LICENSE](LICENSE) file for the details.

## Content Table

- [Installation](#installation)
  - [Building from Source](#building-from-source)
  - [Adding to PATH](#adding-to-path)
- [Usage](#usage)
- [Example](#example)
- [Configuration Guide](#configuration-guide)
  - [Global Architecture Settings](#global-architecture-settings)
  - [Command Opcodes (KEYWORDS)](#command-opcodes-keywords)
  - [Argument Sets (ARGS)](#argument-sets-args)


## Installation

To install TCA++, you should compile from source, you will need the following prerequisites:

- `make`
- `git`
- A C++ compiler:
  - `g++` for Linux
  - `MinGW` for Windows

### Building from Source

1.  First, clone the source code from GitHub:
    ```bash
    git clone https://github.com/HasanAgitUnal/TCA-PlusPlus tcapp
    cd tcapp
    ```

2.  Configure the `include/Architecture.hpp` file for your specific CPU architecture. Refer to the [Configuration Guide](#configuration-guide) below for detailed instructions.

3.  Compile the project:

    For Windows:
    ```bash
    make windows
    ```

    For Linux:
    ```bash
    make linux
    ```



### Adding to PATH

For using `tca++` command you should add it to the PATH environment variable.

##### For Linux

1.  Copy the executable to a standard binary directory:
    ```bash
    mkdir -p ~/.local/bin
    cp build/linux/tca++ ~/.local/bin
    ```

2.  Add this directory to your PATH by editing your shell's configuration file (e.g., `.bashrc`, `.zshrc`):
    ```bash
    echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
    source ~/.bashrc
    ```

##### For Windows

Open CMD and execute the appropriate commands:

If your computer is 64-bit:
```bash
mkdir "C:\TCAPP-Bin"
copy "build/exe/tca++_x64.exe" "C:\TCAPP-Bin"
```

If your computer is 32-bit:
```
mkdir "C:\TCAPP-Bin"
copy "build/exe/tca++_x86.exe" "C:\TCAPP-Bin"
```

Edit PATH variable:
- Open the Start Menu, search for "Edit the system environment variables", and press Enter.
- Click the "Environment Variables..." button in the System Properties window.
- In the "System variables" section, locate and select the `Path` variable, then click "Edit...".
- Click "New" and add `C:\TCAPP-Bin` to the list.
- Click OK on all open windows to save the changes. A system restart may be required for changes to take full effect.

To verify the installation, open a **new** CMD window and run:
```bash
tca++ --help
```
If you do not receive a "command not found" error, the installation was successful.

## Usage

You can view all available options by running `tca++ --help`.

The basic usage in a terminal is:
`tca++ FILES [-o OUTPUT] [-t OTYPE]`

- `-o OUTPUT`: Specifies the output file name (default: `program`).
- `-t OTYPE`: Specifies the output type (default: `binary`).
  - **Supported OTYPEs:**
    - `binary`: Generates a raw binary file (non-human-readable).
    - `text-binary`: Creates a text file containing '0's and '1's representing the binary code.
    - `hexadecimal`: Produces a text file containing the hexadecimal representation of the binary code.

## Example

There is a code example for default config:

```asm
; Loop addition code example

; load R1 register
MVI 5 ; MVI command uses R0 as save location
MOV R0 R1

; load R2 register
MVI 3
MOV R0 R2

; add R1 and R2 register . OPPR command uses R1 and R2 as input and R3 as output
OPPR ADD

; Load R1 with result value
MOV R3 R1

; Jump for making it a loop
MVI 2 ; skip R1 loading for not resetting
JMP ; jmp to address at R0

; this will be run endless
```

For assembling it:
```asm
tca++ loop_add.asm -o loop_add
```

## Configuration Guide

You can configure your CPU's architecture by editing the `include/Architecture.hpp` file. Comprehensive comments within the file also provide guidance. Don't remove the `#include` lines or variable definitions; only edit the content inside the `{}` brackets or remove content within those brackets.
The default configuration is based on the original Turing Complete 8 bit architecture (+ RAM address).

### Global Architecture Settings

- `INSTRUCTION_SIZE`: The total size of a single instruction, in bits.
- `MAX_INT_SIZE`: The maximum bit size for an integer argument.

### Command Opcodes (`KEYWORDS`)

Define your CPU's instructions in the `KEYWORDS` map. The format is as follows:

```cpp
.... KEYWORDS = {

        {
            "NAME", // The assembly name of the command
            {
                {"binary", "01"},       // The binary opcode for this command
                {"arg_count", "1"},     // The number of arguments the command takes
                {"valid_args", "int"}   // The types of arguments it accepts
            }
        },

        // Other keywords...
}
```

- `"NAME"`: The mnemonic name of the instruction (e.g., "MOV", "ADD").
- `"binary"`: The base binary opcode for the instruction. The assembler concatenates this opcode with the binary representation of the arguments to form the final instruction. For example, if the opcode is `01` and the arguments' binary is `110110`, the final instruction will be `01110110`.
- `"arg_count"`: The number of arguments the command takes. This must be `"0"` for commands that do not accept any arguments.
- `"valid_args"`: Defines the allowed types of arguments. You can specify multiple argument sets by separating them with a `|` character (e.g., `"reg|addr"`). The `int` type is built-in and does not require explicit definition in the `ARGS` map; it signifies that the command expects an integer value as an argument.

### Argument Sets (`ARGS`)

If your instructions utilize named arguments (such as registers or condition codes), define them in the `ARGS` map.

```cpp
.... ARGS = {

        {
            "set_name", // A unique name for this argument set (e.g., "reg", "cond")
            {
                {"arg1_name", "000"}, // An argument's assembly name and its binary representation
                {"arg2_name", "001"}
            }
        },

        // Other argument sets
}
```

- `"set_name"`: A unique identifier for the argument set (e.g., "reg" for registers, "cond" for condition flags). This name is then used in the `valid_args` field of your commands in the `KEYWORDS` map.
- `{"arg1_name", "001"}`: Represents an individual argument within the set, with its assembly name and corresponding binary value.

Once defined, you can use these argument sets in your commands. For example:
```asm
EXAMPLE arg1_name
```
