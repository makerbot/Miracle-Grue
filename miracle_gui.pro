#-------------------------------------------------
#
# Project created by QtCreator 2012
#
#-------------------------------------------------

QT       += core gui

TARGET = miracle_gui
TEMPLATE = app


GUI_SRC =  submodule/toolpathviz
include($$GUI_SRC/toolpathviz.pro.inc)

MGL_SRC = src/mgl
include($$MGL_SRC/mgl.pro.inc)

JSON_CPP_SRC = submodule/json-cpp
include($$JSON_CPP_SRC/json-cpp.pro.inc)




