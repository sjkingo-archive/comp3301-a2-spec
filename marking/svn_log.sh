#!/bin/bash

SVN_USER="uqskings@uq.edu.au"
repo="https://svn.itee.uq.edu.au/repo/$1/a2"
svn info --username $SVN_USER $repo
svn log --username $SVN_USER $repo | less
