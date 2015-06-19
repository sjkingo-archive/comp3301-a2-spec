#!/bin/bash
time (for s in s* ; do
    echo
    echo "Testing $s"
    time ./test_indiv.sh $s
    echo "--"
done)
