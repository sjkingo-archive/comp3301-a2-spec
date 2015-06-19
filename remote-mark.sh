#!/bin/bash

# wait 45 seconds for reboot (actually takes just under 40)
w=45

time (for i in `cat remark.txt` ; do
    echo "marking $i"
    time ssh -t comp3301 "cd a2/marking && ./test_indiv.sh students/$i"
    scp -p comp3301:~/a2/marking/students/$i/test-output.txt outputs/$i.txt
    ssh -t comp3301 "sudo reboot"
    echo "machine is rebooting, waiting $w seconds"
    sleep $w
    echo "waking up"
    echo "--"
    echo
done)
