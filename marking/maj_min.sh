#!/bin/bash
dmesg | tail -n 1 | cut -d ']' -f 2 | python -c "
import re
import sys
m = re.search('(\d+).*(\d+)', sys.stdin.read())
if m is not None:
    g = m.groups()
    print g[0], g[1]"
