#-------------------------------------------------
#
# Project created by QtCreator 2012
#
#-------------------------------------------------

QT       += core

TARGET = bin/miracle_grue
TEMPLATE = app



MGL_SRC = src/mgl
include($$MGL_SRC/mgl.pro.inc)

LIBTHING_BASE = submodule/libthing/src/main
include($$LIBTHING_BASE/cpp-qt/Libthing.pro.inc)

JSON_CPP_SRC = submodule/json-cpp
include($$JSON_CPP_SRC/json-cpp.pro.inc)


OPTIONPARSER_BASE = submodule/optionparser
include($$OPTIONPARSER_BASE/optionparser.pro.inc)

INCLUDEPATH += src \
    submodule/clp-parser

SOURCES +=  src/miracle_grue.cc



