#!/usr/bin/python

import os
import os.path
import re

gettestname = re.compile('^(.*TestCase)\.cc')

pri = open('unit_tests.pri', 'w')
pri.write('TEST_BASE = src/unit_tests\n')

for filename in os.listdir('.'):
    match = gettestname.match(filename)
    if match is not None:
        testname = match.group(1)

        if not os.path.exists(testname):
            os.mkdir(testname)

        pro = open(testname+'/'+testname+'.pro', 'w')
        pro.write('''
TEMPLATE = app
ROOT = ../../..
SUBMODULES = $$ROOT/submodule

INCLUDEPATH += $$ROOT/src
LIBS += $$ROOT/lib/libmgl.a -lcppunit -lgomp
SOURCES += ../UnitTestMain.cc ../UnitTestUtils.cc
HEADERS += ../UnitTestUtils.h
QMAKE_CXXFLAGS += -fopenmp

JSON_CPP_SRC = $$SUBMODULES/json-cpp
include($$JSON_CPP_SRC/json-cpp.pri)

LIBTHING_BASE = $$SUBMODULES/libthing/src/main
include($$LIBTHING_BASE/cpp-qt/Libthing.pro.inc)

''')
        pro.write('TARGET = $$ROOT/bin/unit_tests/'+testname+'\n')
        pro.write('SOURCES += ../'+testname+'.cc\n')
        pro.write('HEADERS += ../'+testname+'.h\n')

        pri.write('SUBDIRS += $$TEST_BASE/'+testname+'\n')
        pri.write('$$TEST_BASE/'+testname+'.depends = mgl\n\n')

        pro.close()
pri.close()
        
