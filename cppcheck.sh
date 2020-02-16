#!/bin/sh
cppcheck --quiet --enable=style --force --template=gcc -j 10 -isrc/vendor -Iinclude src
