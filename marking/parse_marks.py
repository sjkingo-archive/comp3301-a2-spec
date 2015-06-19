#!/usr/bin/python

import re

# 1a: test_1a_open_close - Test finished: 2/2 marks
_patt = re.compile(r'^([1-9][a-z]).*(\d+)/\d+ marks$')
_patt_manual = re.compile(r'^([1-9][a-z]).*requires manual marking: (\d+) marks total$')
def parse_line(line):
    m = _patt.match(line)
    if m is not None:
        print m.groups()
    else:
        m = _patt_manual.match(line)
        if m is not None:
            print m.groups(), '- manual'

def parse_log(filename):
    with open(filename, 'r') as fp:
        for l in fp.readlines():
            parse_line(l.strip(r'\n'))


if __name__ == '__main__':
    import sys
    assert len(sys.argv) == 2
    parse_log(sys.argv[1])
