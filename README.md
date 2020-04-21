
# chip8-toolchain

This is a complete toolchain for the CHIP-8, mostly written in C.  

# Status

A big part of the project is already done and tested.  
But there is still a lot left to implement.  

- emulator (DONE).  
- toolchain backend: as, ld, objdump, bin2rom, rom2bim (DONE)
- debugger (TODO).  
- compiler: Front/middle end done, backend (TODO).  
- JIT x64 (TODO)

# Environment

I only tested my programs on the following environment:
- Ubuntu 18.04 x64 
- GCC 7.5 
- python 3.6.9
- cmake 3.10.2

SDL2 needed to run the emulator application.

# Build

```
git submodule update --init
mkdir _build
cd _build
cmake ..
make
```

# Programs

## oc8-emu

Usage: `./oc8-emu <file> [--no-gui]` 

Take a CHIP-8 ROM or `.c8bin' file as input, and run the emulator.  
GUI with SDL2, no sound.  
No GUI mode disable SDL. Used for debugging purposes.

## oc8-as

Usage: `./oc8-as <input-file> [-o <output-file>]`.  

Compile an assembly text file (.c8s) into object file (.c8o)

## oc8-objdump

Usage: `./oc8-objdump <input-file>`.  

Print binary file (.c8o / .c8bin) into human-readable form

## oc8-ld

Usage: `./oc8-ld <input-files> -o <output-file>`.  
Takes many object files (.c8o) and combine them in one binary ROM (.c8bin)

## oc8-bin2rom

Usage: `./oc8-bin2rom <input-bin-file> -o <output-rom-file>`.  
Take a binary file (.c8bin), strip all symbol infos, and create a native CHIP-8 ROM file.

## oc8-rom2bin

Usage: `./oc8-rom2bin <input-rom-file> -o <output-bin-file>`.  
Take a CHIP-8 ROM file, and add some genric symbol infos (empty tables) 
to save it to a binary file (.c8bin)

# Debugger

@TODO

# Libraries

## oc8_is

- Encoder: encode ins to binary from ins struct
- Decoder: decode ins to ins struct from binary

## oc8_emu

Emulate the CHIP-8 CPU and run instructions.  
Can execute step by step, or with a loop and an editable clock speed.

## oc8_as

Represent assembly code.

- API to build `as_sfile` struct
- Reader: parse `.c8s` files and use API above to build `as_sfile` struct
- Printer: Generate string (`.c8s` format) from the `as_sfile` struct, 
that can be parsed again with the reader.
- Assembler: build `bin_file `struct from `as_sfile` struct

## oc8_bin

Represent binary data (`.c8o` and `.c8bin` files)

- API to build `bin_fil`e struct
- BinReader: Read binary `.c8o` / `.c8bin` file and build `bin_file` struct
- BinWriter: Write binary `.c8o` / `.c8bin` file from `bin_file` struct
- Printer: Generate human-readable string from `bin_file` struct.

## oc8_debug

Use ODB library to add debugging features to `oc8_emu`.  
More infos in Debugger section.

## oc8_ld

Take many object `bin_file` structs, and combine them in one runnable `bin_file` struct.

## Other util libraries

### args_parser

Really basic arguments parsers for CLI programs.  
Used by most of the binaries of this projects

### oc8_smap

Basic implementation of a `map<string, size_t>`.  
Implementation based on linkled lists, will try to do best later.  
Used by many libraries that need symbol tables.  
The val `size_t` can store any number or pointer.

# Testing

```
make check
```

Python required
