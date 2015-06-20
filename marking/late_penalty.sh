#!/bin/bash

SVN_USER="uqskings@uq.edu.au"
DUE_SINCE_EPOCH="`date -d 'Fri Sept 28 20:00:00 EST 2012' '+%s'`"

repos=`cd .. && ./class_list_to_repo.sh a2`
for i in $repos ; do
    std_num="`echo $i | cut -d '/' -f 5 | cut -d '-' -f 2`"

    # get the last revision on the repo
    last_rev_date="`svn info --username $SVN_USER $i 2>/dev/null | egrep 'Last Changed Date' | cut -d ' ' -f 4-6`"
    [[ -z "$last_rev_date" ]] && continue

    # parse how many seconds overdue to student is from the due datetime
    secs_since_epoch=`date -d "$last_rev_date" '+%s'`
    secs_overdue=$(($secs_since_epoch-$DUE_SINCE_EPOCH))

    # if the student is overdue, print out some information used for marking
    if [ $secs_overdue -gt 0 ] ; then
        days_overdue="`python -c "from math import ceil; print int(ceil($secs_overdue/86400.0))"`"
        echo "$std_num overdue by $days_overdue day(s) (last rev $last_rev_date)"
    fi
done
