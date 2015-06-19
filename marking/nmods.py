#!/usr/bin/python

import re
import sys

m = re.match('^Found (\d+)', sys.stdin.read())
if m is None:
    print 0
else:
    print m.groups()[0]
