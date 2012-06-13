#!/usr/bin/python

import fnmatch
import os


for root, dirnames, filenames in os.walk('src'):
  for filename in fnmatch.filter(filenames, 'Makefile'):
      os.unlink(os.path.join(root, filename))

for testname in fnmatch.filter(os.listdir(os.path.join('src', 'unit_tests')),
                               '*TestCase'):
    testdir = os.path.join('src', 'unit_tests', testname)
    for sub in fnmatch.filter(os.listdir(testdir), '*.pro'):
        os.unlink(os.path.join(testdir, sub))

testlist = os.path.join('src', 'unit_tests', 'unit_tests.pri')
if os.access(testlist, os.R_OK):
  os.unlink(testlist)
                                  
