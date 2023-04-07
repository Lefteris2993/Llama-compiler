#!/bin/bash

opimize="";
print_ir=false;
print_asm=false;
file="";

while (( "$#" )); do 
    if [[ "$1" == "-O" ]]; then
        optimize="-O"
    elif [[ "$1" == "-f" ]]; then
        print_asm=true
    elif [[ "$1" == "-i" ]]; then
        print_ir=true
    else
        file=$1
    fi
    shift 
done

file_no_ext=${file%.*}

./Llama $optimize < $file > ${file_no_ext}.imm

if [ $? -ne 0 ]; then
    echo "Error while compiling to LLVM IR"
    exit 1
fi

if [ "$print_ir" = true ] ; then
    cat ${file_no_ext}.imm
fi

llc-11 ${file_no_ext}.imm -o ${file_no_ext}.asm

if [ $? -ne 0 ]; then
    echo "Error while compiling LLVM IR to ASM"
    exit 1
fi

if [ "$print_asm" = true ] ; then
    cat ${file_no_ext}.asm
fi

# Do static linking by default cause in the machine i run the executables
# the libraries can not be found...
clang-11 -o ${file_no_ext}.out ${file_no_ext}.asm src/library/lib.a -static

if [ $? -ne 0 ]; then
    echo "Error while linking"
    exit 1
fi

exit 0
