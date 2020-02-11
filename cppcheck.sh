#!/bin/sh
cppcheck --quiet --enable=style --force --template=gcc --suppressions-list="suppressions.txt" -j 10 -isrc/vendor -Iinclude src
