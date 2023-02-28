#!/bin/bash

./Llama < $1 > a.ll
llc-11 a.ll -o a.asm
clang-11 -o a.out a.asm src/library/lib.a