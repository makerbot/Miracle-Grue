
#-------------------------------------------------
#
# QtCreator project include file
#
#-------------------------------------------------

TEMPLATE=lib

mac {
    INCLUDEPATH += /System/Library/Frameworks/CoreFoundation.framework/Versions/Current/Headers
	LIBS += -framework CoreFoundation
}

QMAKE_CXXFLAGS += -fopenmp

INCLUDEPATH += $$MGL_SRC/..

SOURCES +=     $$MGL_SRC/abstractable.cc \
    $$MGL_SRC/clipper.cc\
    $$MGL_SRC/configuration.cc\
    $$MGL_SRC/gcoder.cc\
    $$MGL_SRC/gcoder_gantry.cc\	
    $$MGL_SRC/insets.cc\
    $$MGL_SRC/JsonConverter.cc\
    $$MGL_SRC/mgl.cc\
    $$MGL_SRC/meshy.cc\
    $$MGL_SRC/miracle.cc\
    $$MGL_SRC/segment.cc\
    $$MGL_SRC/shrinky.cc\
    $$MGL_SRC/ScadDebugFile.cc \
    $$MGL_SRC/log.cc\
    $$MGL_SRC/grid.cc\
    $$MGL_SRC/regioner.cc\
    $$MGL_SRC/slicer.cc\
    $$MGL_SRC/pather.cc\

HEADERS +=     $$MGL_SRC/abstractable.h\
    $$MGL_SRC/clipper.h\
    $$MGL_SRC/configuration.h\
    $$MGL_SRC/connexity.h\
    $$MGL_SRC/Exception.h\
    $$MGL_SRC/gcoder.h\
    $$MGL_SRC/infill.h\
    $$MGL_SRC/insets.h\
    $$MGL_SRC/JsonConverter.h\
    $$MGL_SRC/meshy.h\
    $$MGL_SRC/mgl.h\
    $$MGL_SRC/miracle.h\
    $$MGL_SRC/segment.h\
    $$MGL_SRC/shrinky.h\
    $$MGL_SRC/ScadDebugFile.h \
    $$MGL_SRC/log.h \
    $$MGL_SRC/grid.h \
    $$MGL_SRC/pather.h \
    $$MGL_SRC/regioner.h \
