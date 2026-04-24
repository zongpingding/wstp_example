#!/usr/bin/env zsh

local WOLFRAM="/usr/local/Wolfram/Wolfram/14.3" 

local source_file="$1"
local source_file_noext="${source_file%.*}"

# compile
gcc "$source_file" -o "$source_file_noext" \
  -I$WOLFRAM/SystemFiles/Links/WSTP/DeveloperKit/Linux-x86-64/CompilerAdditions \
  -L$WOLFRAM/SystemFiles/Links/WSTP/DeveloperKit/Linux-x86-64/CompilerAdditions \
  -lWSTP64i4 \
  -lpthread \
  -Wl,-rpath,$WOLFRAM/SystemFiles/Links/WSTP/DeveloperKit/Linux-x86-64/CompilerAdditions

# run
./"$source_file_noext"
