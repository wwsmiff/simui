#!/bin/sh

set -xe

CC="clang"
CFLAGS="-Wall -O2 -Wextra -std=c17"
LFLAGS="-lSDL2"
SRCDIR="./src"
INCLUDEDIR="./include"
BUILDDIR="./build"
EXE="main"

$CC -I$INCLUDEDIR $CFLAGS $SRCDIR/*.c -o "$BUILDDIR/$EXE" $LFLAGS
