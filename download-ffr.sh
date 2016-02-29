#!/bin/bash

FFR="http://www.flashflashrevolution.com/R2"

if [ "$1" == "" ] || [ "$2" == "" ]; then echo "Usage: $0 [name] [ID]"; exit 1; fi

NAME="$1"
NUM="$2"

# get the game data
wget "${FFR}/${NAME}.jpg"
wget "${FFR}/${NAME}-bg.jpg"
wget "${FFR}/${NAME}.mp3"


DIFFICULTIES="single_beginner single_basic single_another single_maniac"

# get the game files
for DIFFICULTY in $DIFFICULTIES; do
	wget "${FFR}/ffrload.php?id=${NUM}&P1=${DIFFICULTY}&P2=" -O ${DIFFICULTY}.ffr
done
