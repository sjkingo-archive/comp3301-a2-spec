#!/bin/bash

SUBMISSIONS="./submissions"
SVN_USER="uqskings@uq.edu.au"
MISSING_FILE="$SUBMISSIONS/missing.txt"
GROUPWORK_FILE="./groupwork.csv"

if [ -d $SUBMISSIONS ] ; then
    echo "$SUBMISSIONS already exists, please remove" 1>&2
    exit 2
fi

mkdir $SUBMISSIONS || exit 3
rm -f $MISSING_FILE && touch $MISSING_FILE || exit 3

echo "Starting checkout. Note you may receive 1 password request from svn."

repos=`cd .. && ./class_list_to_repo.sh a2`
for i in $repos ; do
    repo_user="`echo $i | cut -d '/' -f 5`"
    std_num="`echo $repo_user | cut -d '-' -f 2`"
    wc_dir="$SUBMISSIONS/$repo_user"

    # check if this student is part of a group
    l="`grep $std_num $GROUPWORK_FILE`"
    if [ $? -eq 0 ] ; then
        # yes
        if [ "`echo $std_num $l | awk '{ print index($2, $1) }'`" = "10" ] ; then
            # this student partnered with another, so symlink to the correct repo
            # note for now, this may create broken symlinks - once script is finished
            # all symlinks should be valid
            master_std_num="`echo $l | cut -d ',' -f 1`"
            master_repo_user="`echo $repos | sed -e 's/ /\n/g' | grep $master_std_num | cut -d '/' -f 5`"
            (cd $SUBMISSIONS && ln -s $master_repo_user $repo_user)
            echo "Symlinked $repo_user to $master_repo_user (partner)"
            continue
        fi
    fi

    # otherwise, check this repo out
    echo "Checking out $repo_user to $wc_dir"
    svn checkout -q --username $SVN_USER $i $wc_dir
    [[ $? -eq 0 ]] || echo $i >> $MISSING_FILE
done
