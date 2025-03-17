# Llama-compiler
Llama compiler for ece ntua compilers class 2021
https://courses.softlab.ntua.gr/compilers/2021a/llama2021.pdf

## Requirements

- clang: 11.1.0-6
- flex 2.6.4
- bison 3.8.2
- llvm 11.0.0
- lib.a from https://github.com/abenetopoulos/edsger_lib (Requires nasm to build)

## Build

After installing the above requirements use `make` to build the compiler.
Then you can use it as follows:

`./do.sh [path/to/llama] [...options]`

The available options are:
- `-O` run optimizations
- `-f` print .asm to stdout
- `-i` print intermediate code to stdout

## Testing

run tests with `make test`
