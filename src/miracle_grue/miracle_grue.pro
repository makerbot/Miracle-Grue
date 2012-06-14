#-------------------------------------------------
#
# Project created by QtCreator 2012
#
#-------------------------------------------------

#QT       += core

TARGET = ../../bin/miracle_grue
TEMPLATE = app
CONFIG += console
INSTALLS += target

win32 {
    DESTDIR=.
    OBJECTS_DIR=.
    TARGET=../../../bin/miracle_grue
	QMAKE_LFLAGS += -static -static-libgcc -static-libstdc++
}
target.path = /usr/bin

mac {
    INCLUDEPATH += /System/Library/Frameworks/CoreFoundation.framework/Versions/Current/Headers
    LIBS += -framework CoreFoundation
    LIBS -= QtGui QtCore
    CONFIG -= app_bundle
    CONFIG += dll
    QT -= gui core

}

INCLUDEPATH += ..

LIBTHING_BASE = ../../submodule/libthing/src/main
include($$LIBTHING_BASE/cpp-qt/Libthing.pro.inc)

JSON_CPP_SRC = ../../submodule/json-cpp
include($$JSON_CPP_SRC/json-cpp.pri)


OPTIONPARSER_BASE = ../../submodule/optionparser
include($$OPTIONPARSER_BASE/optionparser.pro.inc)

//INCLUDEPATH += src \
//    submodule/clp-parser
//DEPENDPATH += src \
//    submodule/clp-parser

SOURCES +=  miracle_grue.cc
LIBS += ../../lib/libmgl.a


