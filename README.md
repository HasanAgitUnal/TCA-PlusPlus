# TCA++ | A Custom Assembler for Your CPU

TCA++ is an assembler for all architectures, primarily developed for the game Turing Complete. If you have designed your own CPU and need an assembler, this might be it.

Licensed under the GNU Affero General Public License v3.0 (AGPLv3). See the [LICENSE](LICENSE) file for the details.

## Table of Contents

- [Features](#features)
- [Installation](#installation)
- [Building from Source](#building-from-source)
- [Usage](#usage)
- [Example](#example)
- [Labels](#labels)
- [Consts](#consts)
- [Configuration Guide](#configuration-guide)
  - [Global Architecture Settings](#global-architecture-settings)
  - [Command Opcodes (KEYWORDS)](#command-opcodes-keywords)
  - [Argument Sets (ARGS)](#argument-sets-arg_sets)


## Features

- All CPU architectures are supported
- Configured with JSON
- [Labels](#labels), [Consts](#consts), Supports Unsigned/Signed values

## Installation

To install TCA++, Go to [releases](https://github.com/HasanAgitUnal/TCA-PlusPlus/releases) and download the package for your OS.

## Building from Source

If you want to build `tca++` from source code.

###### Building Requirements:

- A Linux environment
- `make`
- `git`
- `g++`

###### Steps:

1.  First, clone the source code from GitHub:
    ```bash
    git clone https://github.com/HasanAgitUnal/TCA-PlusPlus tcapp
    cd tcapp
    ```

2. Compile the project:

    For `.exe`:
    ```bash
    make windows
    ```

    For Linux binary:
    ```bash
    make linux
    ```

## Usage

The `tca++` command usage:
```bash
tca++ FILES [-o OUTPUT] [-t binary|text-binary|hex ] [-a ARCHITECTURE_FILE]
```

Optionals:
- `-o` : Output file. Default: `program`
- `-t` : Output types accepts values: `binary`, `text-binary`, `hex`. Default: `binary`
- `-a` : Custom architecture config file path.

## Example

A code example for the default config can be found in [example.asm]:

To assemble it, run:
```asm
tca++ example.asm -o example
```

## Labels

Labels are basically unsigned [const](#consts) values. Labels contain the address of the command after them.
Defining a label:
```asm
.example
; do some stuff there
```

Using a label (I'm using the `JMP` command as an example; you can use any command)
```asm
JMP .example ; jump to some stuff
```

## Consts

Consts are the values you can use in your code by its name. Can be signed or unsigned.

Example defining:
```asm
; Signed consts

const PI 3.14
const NEGATIVE_PI -3.14

; Unsigned consts
uconst RANDOM_UNSIGNED_VALUE 78738
uconst abc = 123 
```

To use a constant value, you should add `int` or `uint` to your [command's](#command-opcodes-keywords) [argument sets](#argument-sets-arg_sets).

Usage:

```asm
MVI PI ; this will be: MVI 3.14
```
        
## Configuration Guide

You configure your CPU's architecture by creating and editing an `architecture.json` file.
There is an example configuration in [example_arch.json]

The assembler looks in these locations for the configuration file by default:

- On Windows : %APPDATA%\tcapp\architecture.json
- On Linux   : ~/.config/tcapp/architecture.json

And you can specify a custom location:
```bash
tca++ code.asm -a /path/to/custom_architecture_file.json
```

An example `architecture.json` might look like this:

```json
{
    "instruction_size": 8,
    "max_int_size": 6,
    
    "keywords": {
        "MVI": {
            "binary": "00",
            "arg_count": 1,
            "arg_sets": "uint"
        }
    },

    
    "arg_sets": {
        "reg": {
            "R0": "000",
            "R1": "001"
        }
    }
}
```

### Global Architecture Settings

These are the top-level keys in the `architecture.json` file.

- `"instruction_size"`: The total size of a single instruction, in bits (e.g., `8`).
- `"max_int_size"`: The maximum bit size for an integer argument (e.g., `6` for a range of 0-63 for unsigned and -32-31 for signed).

### Command Opcodes (`keywords`)

Define your CPU's instructions in the `keywords` object. Each key is the instruction's name.

```json
"keywords": {
    "MOV": {
        "binary": "10",
        "arg_count": 2,
        "arg_sets": "reg"
    },
    "JMP": {
        "binary": "11000100",
        "arg_count": 0
    },
    "MVI": {
        "binary": "00",
        "arg_count": 1,
        "arg_sets": "uint"
    }
}
```

- `"binary"`: The base binary opcode for the instruction. The assembler concatenates this opcode with the binary representation of the arguments to form the final instruction. For example, if the opcode is `10` and the arguments' binary is `001010`, the final instruction will be `10001010`.
- `"arg_count"`: The number of arguments the command takes. This must be `0` for commands that do not accept any arguments.
- `"arg_sets"`: Defines the allowed types of arguments. You can specify multiple argument sets by separating them with a `|` character (e.g., `"reg|addr"`). The `int` and `uint` types are built-in and do not require explicit definition in `arg_sets`; they signify that the command expects a signed/unsigned integer value.

### Argument Sets (`arg_sets`)

If your instructions use named arguments (like registers or condition codes), define them in the `arg_sets` object.

```json
"arg_sets": {
    "reg": {
        "R0": "000",
        "R1": "001",
        "R2": "010"
    },
    "cond": {
        "N": "000",
        "=": "001"
    }
}
```

- The key (e.g., `"reg"`, `"cond"`) is a unique name for the argument set. This name is then used in the `arg_sets` field of your commands in the `keywords` object.
- Inside each set, each key-value pair represents an argument's assembly name and its corresponding binary value (e.g., `"R0": "000"`).
- You can also use [labels](#labels) as arguments.

