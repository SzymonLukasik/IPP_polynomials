#!/bin/bash

if [[ ! -e CMakeLists.txt ]]
  then echo "Please execute this script from repository root"
  exit 2
fi


for file in src/*.c
do
  gcc -c -O2 -std=c11 -Wall -Wextra -Wno-implicit-fallthrough "$file" -o /dev/null 2>&1
done