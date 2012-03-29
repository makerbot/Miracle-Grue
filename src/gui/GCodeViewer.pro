#-------------------------------------------------
#
# Project created by QtCreator 2011-05-11T22:05:25
#
#-------------------------------------------------

QT       += core gui
QT       += opengl

TARGET = GCodeViewer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    gcode.cpp \
    gcodeview.cpp \
    gcodeviewapplication.cpp \
    arcball.cpp \
    quaternion.cpp \
    algebra3.cpp

HEADERS  += mainwindow.h \
    gcode.h \
    gcodeview.h \
    gcodeviewapplication.h \
    arcball.h \
    algebra3.h \
    quaternion.h \
    glui_internal.h

FORMS    += mainwindow.ui
