#!/usr/bin/env zsh

set -e 

local WOLFRAM="/usr/local/Wolfram/Wolfram/14.3" 
local WSTP_PATH="$WOLFRAM/SystemFiles/Links/WSTP/DeveloperKit/Linux-x86-64/CompilerAdditions"

local source_file="$1"
local source_file_noext="${source_file%.*}"

echo "Compiling $source_file ..."
gcc "$source_file" -o "$source_file_noext" \
  -std=gnu11 \
  -O2 \
  -Wall -Wextra \
  -I"$WSTP_PATH" \
  -L"$WSTP_PATH" \
  -lWSTP64i4 \
  -lm -lpthread -lrt -luuid \
  -Wl,-rpath,"$WSTP_PATH" \
  -Wl,--as-needed

# run
echo "Running $source_file_noext ..."
echo "----------------------------------------"
./"$source_file_noext"
echo "----------------------------------------"
