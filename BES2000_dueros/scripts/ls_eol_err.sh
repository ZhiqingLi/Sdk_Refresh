#!/bin/sh

find .. \( \( -name out -o -name .git -o -name si -o -name lib -o -name \*.txt -o -name \*.class -o -name \*.pcm -o -name \*.icf -o -name \*.bat -o -name \*.vcxproj\* -o -name \*.sbc -o -name \*.jar -o -name \*.bin \) -prune \) -o -type f -exec ./check_eol.sh {} \; -print

