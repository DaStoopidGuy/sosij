#! /usr/bin/bash

SRC=src/*.c
OUT=out/app

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
