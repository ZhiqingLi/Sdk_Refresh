#!/bin/sh

BOARD_FILE=${1##*/}
BOARD=${BOARD_FILE%%.*}

#upper all char
BOARD=`echo $BOARD | tr [a-z] [A-Z]`
echo "Build $BOARD"

sed "0,/^CC_LINKPLAY_OPTS =.*/s//CC_LINKPLAY_OPTS = -DBOARD_${BOARD}/" ../cfg/rules.mk > tmpfile
mv tmpfile ../cfg/rules.mk
