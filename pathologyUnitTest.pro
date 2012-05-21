#-------------------------------------------------
#
# Project created by QtCreator  
#
#-------------------------------------------------

 
TARGET = roofingUnitTest
SOURCES +=  src/unit_tests/PathologyTestCase.cc \
            src/unit_tests/UnitTestMain.cc


HEADERS  += src/unit_tests/PathologyTestCase.h \
            src/unit_tests/UnitTestUtils.h

QT       += core 

TEMPLATE = app

CPPUNIT_SRC = submodule/cppunit
MGL_SRC = src/mgl
JSON_CPP_SRC = submodule/json-cpp
LIBTHING_BASE = submodule/libthing/src/main


include($$JSON_CPP_SRC/json-cpp.pro.inc)
include($$CPPUNIT_SRC/cppunit.pro.inc)
include($$LIBTHING_BASE/cpp-qt/Libthing.pro.inc)
include($$MGL_SRC/mgl.pro.inc)



INCLUDEPATH += src



