#!/bin/bash
time (for s in s* ; do
    pushd $s >/dev/null || exit 2
    rm -f a2_module test-output.txt
    make clean >/dev/null
    popd >/dev/null
done)
