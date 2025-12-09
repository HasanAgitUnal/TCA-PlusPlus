# TCA++ | A Custom Assembler for Your CPU

TCA++ is an assembler for all architectures, primarily developed for the game Turing Complete. If you have designed your own CPU and need an assembler, this might be it.

Licensed under the GNU Affero General Public License v3.0 (AGPLv3). See the [LICENSE](LICENSE) file for the details.

## Content Table

- [Installation](#installation)
- [Building from Source](#building-from-source)
- [Usage](#usage)
- [Example](#example)
- [Configuration Guide](#configuration-guide)
  - [Global Architecture Settings](#global-architecture-settings)
  - [Command Opcodes (KEYWORDS)](#command-opcodes-keywords)
  - [Argument Sets (ARGS)](#argument-sets-arg_sets)


## Installation

To install TCA++, Go to [releases](https://github.com/HasanAgitUnal/TCA-PlusPlus/releases) and download the package for your os.

## Building from Source

If you want to build `tca++` from source code.

###### Build requirements:

- A linux environment
- `make`
- `git`
- `g++`

###### Steps:

1.  First, clone the source code from GitHub:
    ```bash
    git clone https://github.com/HasanAgitUnal/TCA-PlusPlus tcapp
    cd tcapp
    ```

3.  Compile the project:

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
- `-t` : Output types excepts values: `binary`, `text-binary`, `hex`. Default: `binary`
- `-a` : Custom architecture config file path.

## Example

[There is](example.asm) a code example for default config:

For assembling it run:
```asm
tca++ example.asm -o example
```

## Configuration Guide

You configure your CPU's architecture by creating and editing an `architecture.json` file.
[There is a example configuration](example_arch.json)

Assembler look these locations for configuration file by default:

- On Windows : %APPDATA%\TCAPP\architecture.json
- On Linux   : ~/.config/TCAPP/architecture.json

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
            "arg_sets": "int"
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
- `"max_int_size"`: The maximum bit size for an integer argument (e.g., `6` for a range of 0-63).

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
        "arg_sets": "int"
    }
}
```

- `"binary"`: The base binary opcode for the instruction. The assembler concatenates this opcode with the binary representation of the arguments to form the final instruction. For example, if the opcode is `10` and the arguments' binary is `001010`, the final instruction will be `10001010`.
- `"arg_count"`: The number of arguments the command takes. This must be `0` for commands that do not accept any arguments.
- `"arg_sets"`: Defines the allowed types of arguments. You can specify multiple argument sets by separating them with a `|` character (e.g., `"reg|addr"`). The `int` type is built-in and does not require explicit definition in `arg_sets`; it signifies that the command expects an integer value.

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
