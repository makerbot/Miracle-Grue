TEMPLATE = app
ROOT = ../../..
SUBMODULES = $$ROOT/submodule

INCLUDEPATH += $$ROOT/src
LIBS += $$ROOT/lib/libmgl.a -lcppunit
SOURCES += ../UnitTestMain.cc

JSON_CPP_SRC = $$SUBMODULES/json-cpp
include($$JSON_CPP_SRC/json-cpp.pri)

LIBTHING_BASE = $$SUBMODULES/libthing/src/main
include($$LIBTHING_BASE/cpp-qt/Libthing.pro.inc)

TARGET = $$ROOT/bin/unit_tests/GCoderTestCase
SOURCES += ../GCoderTestCase.cc
HEADERS += ../GCoderTestCase.h

