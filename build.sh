#! /usr/bin/bash

SRC=src/*.c
OUT=bin/app

# compile
compile() {
    mkdir -p $(dirname $OUT)
    gcc $SRC -o $OUT
}

# run
run() {
    ./$OUT
}

compile
run
