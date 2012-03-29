#-------------------------------------------------
#
# Project created by QtCreator 2011-05-11T22:05:25
#
#-------------------------------------------------

QT       += core gui
QT       += opengl

TARGET = miracle_gui
TEMPLATE = app

INCLUDEPATH += src \
            src/json-cpp/include \
            src/gui

SOURCES +=  src/json-cpp/src/lib_json/json_reader.cpp\
    src/json-cpp/src/lib_json/json_value.cpp\
    src/json-cpp/src/lib_json/json_writer.cpp\
    src/mgl/abstractable.cc \
    src/mgl/clipper.cc\
    src/mgl/configuration.cc\
    src/mgl/connexity.cc\
    src/mgl/gcoder.cc\
    src/mgl/infill.cc\
    src/mgl/insets.cc\
    src/mgl/JsonConverter.cc\
    src/mgl/LineSegment2.cc\
    src/mgl/mgl.cc\
    src/mgl/meshy.cc\
    src/mgl/miracle.cc\
    src/mgl/Scalar.cc\
    src/mgl/segment.cc\
    src/mgl/shrinky.cc\
    src/mgl/slicy.cc\
    src/mgl/Triangle3.cc\
    src/mgl/Vector2.cc\
    src/mgl/Vector3.cc\
    src/gui/main.cpp\
    src/gui/mainwindow.cpp \
    src/gui/gcode.cpp \
    src/gui/gcodeview.cpp \
    src/gui/gcodeviewapplication.cpp \
    src/gui/arcball.cpp \
    src/gui/quaternion.cpp \
    src/gui/algebra3.cpp


HEADERS  +=src/json-cpp/include/json/assertions.h \
    src/json-cpp/include/json/config.h\
    src/json-cpp/include/json/features.h\
    src/json-cpp/include/json/forwards.h\
    src/json-cpp/include/json/reader.h\
    src/json-cpp/include/json/value.h\
    src/json-cpp/include/json/writer.h\
    src/mgl/abstractable.h\
    src/mgl/clipper.h\
    src/mgl/configuration.h\
    src/mgl/connexity.h\
    src/mgl/Exception.h\
    src/mgl/gcoder.h\
    src/mgl/infill.h\
    src/mgl/insets.h\
    src/mgl/JsonConverter.h\
    src/mgl/limits.h\
    src/mgl/LineSegment2.h\
    src/mgl/meshy.h\
    src/mgl/mgl.h\
    src/mgl/miracle.h\
    src/mgl/scadtubefile.h\
    src/mgl/Scalar.h\
    src/mgl/segment.h\
    src/mgl/shrinky.h\
    src/mgl/slicy.h\
    src/mgl/Triangle3.h\
    src/mgl/Vector2.h\
    src/mgl/Vector3.h\
    src/gui/mainwindow.h \
    src/gui/gcode.h \
    src/gui/gcodeview.h \
    src/gui/gcodeviewapplication.h \
    src/gui/arcball.h \
    src/gui/algebra3.h \
    src/gui/quaternion.h \
    src/gui/glui_internal.h

FORMS    += src/gui/mainwindow.ui
