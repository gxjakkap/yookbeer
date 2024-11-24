#!/bin/bash

dir="./src"
if [ ! -d "./bin" ]; then
    mkdir -p "./bin"
fi

if [ -f "$dir/main.c" ]; then
    compile_command="gcc \"$dir/main.c\" -o \"./bin/yookbeer\""
    echo "Compiling with command: $compile_command"
    
    eval "$compile_command"
    if [ $? -ne 0 ]; then
        echo "Error compiling. Check the source code for errors." >&2
        exit 1
    else
        echo "Yookbeer compiled successfully! binary at bin/yookbeer"
    fi
else
    echo "File does not exist!"
fi