#!/bin/bash

if [ "$1" = "" ] ; then
    echo "Usage: $0 submission-dir"
    exit 1
fi

pushd "$1" >/dev/null || exit 2
make clean >/dev/null || exit 2
make 2>&1 || exit 2
n=`python -c "from os import listdir; print len([f for f in listdir('.') if f.endswith('.ko')])"`
echo "Found $n kernel modules:"
echo "  `ls *.ko`"
popd >/dev/null
