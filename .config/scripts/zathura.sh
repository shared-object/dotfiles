#!/bin/sh


BOOK_FILE="/tmp/.book"

echo "" > $BOOK_FILE

kitty bash -c 'sh $HOME/scripts/fzf.sh'

BOOK_PATH=$(cat $BOOK_FILE | xargs)

if [ ! -z "$BOOK_PATH" ]; then
	zathura "$BOOK_PATH"
else
	exit
fi;
