#!/bin/bash

if [ "$1" = "" ] ; then
	echo "Usage: $0 submission-dir"
	exit 1
fi
top_dir="`pwd`"

pushd "$1" >/dev/null || exit 2

b="test-output.txt"
if [ ! -f "$b" ] ; then
	echo "$b doesn't exist - did you forget to run build.sh?"
	exit 2
fi

n="`grep -A 1 'Found ' $b | $top_dir/nmods.py`"
if [ $n -ne 1 ] ; then
	echo "Multiple kernel modules were built, manually check this.."
	exit 3
fi

# find the module name
mod="`egrep -A 1 'Found [[:digit:]] kernel modules' $b | tail -n1 | awk '{ print $1 }'`"
name="`echo $mod | cut -d '.' -f 1`"

# if it's loaded, try and unload it
lsmod | grep -q $name
if [ $? -eq 0 ] ; then
    sudo rmmod $name || exit 4
fi

# try to load the module
echo "Going to load $mod into kernel..."
sudo insmod $mod || exit 4
lsmod | grep $name || exit 4
echo "$mod" >a2_module

# find its major/minor
mm="`$top_dir/maj_min.sh`"
if [ "$mm" = "" ] ; then
	echo "Module didn't register itself or didn't print major/minor to dmesg"
	exit 5
fi
maj=`echo $mm | awk '{ print $1 }'`
min=`echo $mm | awk '{ print $2 }'`
echo "Found driver registered at maj=$maj min=$min"

# make a device node - specify full path to not use shell builtin
sudo rm -f /dev/crypto
sudo /bin/mknod -m 666 /dev/crypto c $maj $min || exit 5
echo "Created device node at /dev/crypto: `ls -l /dev/crypto`"

popd >/dev/null
