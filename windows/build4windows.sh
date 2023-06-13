#!/bin/bash

SDL_INCLUDE_PATH=/usr/i686-w64-mingw32/include/SDL

i686-w64-mingw32-gcc \
  -o jumpingsquare.exe \
  ../jumpingsquare.c \
  -std=c99 \
  -l SDL \
  -I "$SDL_INCLUDE_PATH"
if [[ $? == 0 ]]; then
	echo
	echo "SUCCESS!"
fi
read -n1 -t300

