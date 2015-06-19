#!/usr/bin/python

import datetime
import getpass
import re
import urllib2

from BeautifulSoup import BeautifulSoup as bs

URL_PREFIX = 'https://svn.itee.uq.edu.au/viewvc/comp3301-'
URL_SUFFIX = '/a2/?view=log'
DATE_FORMAT = '%a %b %d %H:%M:%S %Y UTC'
CUTOFF_DATE = datetime.datetime.strptime('Thu Oct 13 10:00:00 2011 UTC', DATE_FORMAT)

def parse_log(fp):
    parse = bs(fp)
    std = parse.find('title').contents[0].split('-')[1].split(']')[0]
    latest_rev_attr = parse.find('a', attrs={'name': re.compile('rev\d+$')})
    latest_rev = int(latest_rev_attr['name'].replace('rev', ''))
    d = datetime.datetime.strptime(latest_rev_attr.findNextSibling('em').contents[0], DATE_FORMAT)
    if d > CUTOFF_DATE:
        days_late = (d - CUTOFF_DATE).days + 1
        if days_late > 3:
            days_late -= 1 # weekend is 1 day
        print '%s rev %d: late penalty of %d days (%s)' % (std, latest_rev, days_late, d)

def check_all(student_list, username, passwd):
    with open(student_list, 'r') as student_fp:
        for std in student_fp:
            std = std.strip('\n')
            url = URL_PREFIX + std + URL_SUFFIX
            auth_handler = urllib2.HTTPBasicAuthHandler()
            auth_handler.add_password(realm='ITEE Subversion Server', uri=url, 
                    user=username, passwd=passwd)
            opener = urllib2.build_opener(auth_handler)
            urllib2.install_opener(opener)
            try:
                fp = urllib2.urlopen(url)
            except urllib2.HTTPError:
                pass
            else:
                parse_log(fp)
                fp.close()


if __name__ == '__main__':
    import sys
    assert len(sys.argv) == 2
    username = raw_input('SVN username: ')
    passwd = getpass.getpass('SVN password: ')
    check_all(sys.argv[1], username, passwd)
