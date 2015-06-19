#!/bin/bash

if [ "$1" = "" ] ; then
    echo "Usage: $0 submission-dir [test-criteria]"
    exit 1
fi

sub_dir="$1"
log="$sub_dir/test-output.txt"
namef="$sub_dir/a2_module"

criteria="all"
if [ "$2" != "" ] ; then
    criteria="$2"
    shift 1
fi

function die {
    echo >>$log
    echo "=== FAILURE: aborting marking script with status $1 ===" >>$log
    exit $1
}

function load {
    dir="$1"
    if [ "$2" = "q" ] ; then
        out="/dev/null"
    else
        out="$log"
    fi
    ./load.sh "$1" >>$out 2>&1 || die $?
}

function unload {
    sudo rmmod "$1" || die $?
}

# clean up after last test
sudo dmesg -c >/dev/null
rm -f $log $namef

echo "=== a2marker test output for $sub_dir; run on `hostname` at `date` with pid $$ ===" >>$log
echo >>$log

echo "--- Building and loading module ---" >>$log
./build.sh "$sub_dir" >>$log 2>&1 || die $?
load "$sub_dir"
mod="`cat $namef`"
echo >>$log

echo "--- Running tests ---" >>$log
failed=0
for i in tests/test_*.c ; do
    t="`basename $i .c`"
    c="`echo $t | cut -d '_' -f 2`"

    if [ "$criteria" != "all" ] ; then
        if [ "$criteria" != "$c" ] ; then
            continue
        fi
    fi

    echo >>$log
    echo -n "$c: $t - " >>$log
    (cd tests && ./$t /dev/crypto) >>$log 2>&1
    if [ $? -ne 0 ] ; then
        echo ".. FAILED" >>$log
	failed=$((failed+1))
    fi

    if [ "$criteria" != "all" ] ; then
        continue
    fi

    tail -n1 $log | grep -q 'device has been marked as dirty'
    if [ $? -eq 0 ] ; then
        echo "--- Reloading module ---" >>$log
        unload "$mod"
        load "$sub_dir" q
    fi
done
echo >>$log

echo "--- Unloading module ---" >>$log
unload "$mod"
echo >>$log
echo "!! kernel is dirty, reboot required !!" >>$log

if [ $failed -eq 0 ] ; then
    echo "=== SUCCESS ===" >>$log
    echo " (all tests passed)" >>$log
    r=0
else
    echo "=== FAILED ===" >>$log
    echo " ($failed tests failed)" >>$log
    r=10
fi

exit $r
