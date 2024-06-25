#!/bin/sh

set -xe

DEBUG=1

CC="clang"
if [[ $DEBUG -eq 1 ]]; then
  CFLAGS="-Wall -g -O0 -Wextra -std=c17"
else 
  CFLAGS="-Wall -march=native -O3 -DNDEBUG -Wextra -std=c17"
fi

LFLAGS="-lSDL2 -lSDL2_ttf"
SRCDIR="./src"
INCLUDEDIR="./include"
BUILDDIR="./build"
EXE="main"

$CC -I$INCLUDEDIR $CFLAGS $SRCDIR/*.c -o "$BUILDDIR/$EXE" $LFLAGS
