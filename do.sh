#!/bin/bash

./Llama < $1 > a.ll
llc-11 a.ll -o a.asm

# Do static linking by default cause of the machine i run the executables
clang-11 -o a.out a.asm src/library/lib.a -static