#!/bin/sh

# Check end of line (eol). Return 1 if eol exists, and return 0 otherwise

if [ $(tail -c1 "$1" | wc -l) -gt 0 ]; then
	exit 1
else
	exit 0
fi
