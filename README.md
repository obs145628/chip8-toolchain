
# chip8-toolchain

This is a complete toolchain for the CHIP-8, mostly written in C.  
- emulator (done).  
- toolchain (backend): almost done (expect linker, and binaries).  
- debugger: TODO.  
- compiler: Front/middle end done, backend TODO.  
- JIT x64: TODO

# Programs

## oc8-emu

Usage: `./oc8-emu <file>`

Take a CHIP-8 ROM or `.c8bin' file as input, and run the emulator.  
GUI with SDL, no sound.

## oc8-as

TODO  
Compile an assembly text file (.c8s) into object file (.c8o)

## oc8-objdump

TODO  
Print binary file (.c8o / .c8bin) into human-readable form

## oc8-ld

TODO  
Takes many object files (.c8o) and combine them in one binary ROM (.c8bin)

## oc8-bin2rom

TODO  
Take a binary file (.c8bin), strip all symbol infos, and create a native CHIP-8 ROM file.

## oc8-rom2bin

TODO  
Take a CHIP-8 ROM file, and add some genric symbol infos (empty tables) 
to save it to a binary file (.c8bin)


# Libraries

# oc8_is

- Encoder: encode ins to binary from ins struct
- Decoder: decode ins to ins struct from binary

# oc8_emu: (oc8_is)

Emulate the CHIP-8 CPU and run instructions

# oc8_as : (oc8_bin, oc8_is, oc8_smap)

Represent assembly code

- API to build as_sfile struct
- Reader: parse `.c8s` files and use API above to build as_sfile struct
- Printer: Generate string (.c8s format) from the as_sfile struct, 
that can be parsed again with the reader.
- Assembler: build bin_file struct from as_sfile struct

# oc8_bin

Represent binary data (.c8o and .c8bin files)

- API to build bin_file struct
- BinReader: Read binary `.c8o` / `.c8bin` file and build bin_file struct
- BinWriter: Write binary `.c8o` / `.c8bin` file from bin_file struct
- Printer: Generate human-readable string from bin_file struct.

TODO: write tests for reader / writer

# oc8_ld: (oc8_bin)

TODO.  
Take many object bin_file structs, and combine them in one runnable bin_file struct.  


# oc8_smap

Basic implementation of a `map<string, size_t>`.  
Implementation based on linkled lists, will try to do best later.  
Used by many libraries that need symbol tables.  
The val size_t can store any number or pointer.  

