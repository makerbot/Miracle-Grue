
#-------------------------------------------------
#
# QtCreator project include file
#
#-------------------------------------------------

win32 {
    isEmpty(JENKINS) {
        isEmpty(BOOST_HOME):BOOST_HOME=C:\Boost
        isEmpty(BOOST_LIB_PATH):BOOST_LIB_PATH=-L"$$BOOST_HOME\lib"
        isEmpty(BOOST_INCLUDE):BOOST_INCLUDE=$$BOOST_HOME\include\boost-1_49
        isEmpty(BOOST_LIBS):BOOST_LIBS=-lboost_system-mgw46-mt-1_49 \
               -lboost_filesystem-mgw46-mt-1_49 \
               -lboost_regex-mgw46-mt-1_49
    } else {
        BOOST_HOME=C:\Boost-1.41.0
        BOOST_LIB_PATH=-LC:\Boost-1.41.0\lib
        BOOST_INCLUDE="$$BOOST_HOME\include\boost-1_41"
        BOOST_LIBS=$${BOOST_LIB_PATH}/libboost_system-mgw34-mt.lib \
               $${BOOST_LIB_PATH}\libboost_filesystem-mgw34-mt.lib \
               $${BOOST_LIB_PATH}\libboost_regex-mgw34-mt.lib
    }
}

unix {
    isEmpty(BOOST_HOME) BOOST_HOME=/usr
    isEmpty(BOOST_LIB_PATH):BOOST_LIB_PATH=$$BOOST_HOME/lib
    isEmpty(BOOST_INCLUDE):BOOST_INCLUDE=$$BOOST_HOME/include
    isEmpty(BOOST_LIBS):BOOST_LIBS=-lboost_system -lboost_filesystem -lboost_regex
}

mac {
    isEmpty(BOOST_HOME):BOOST_HOME=/opt/local
    isEmpty(BOOST_STATIC_PATH):BOOST_LIB_PATH=$$BOOST_HOME/lib
    isEmpty(BOOST_INCLUDE):BOOST_INCLUDE=$$BOOST_HOME/include
    isEmpty(BOOST_LIBS):BOOST_LIBS=$$BOOST_STATIC_PATH/libboost_system.a \
	           $$BOOST_STATIC_PATH/libboost_filesystem.a \
		   $$BOOST_STATIC_PATH/libboost_regex.a
}

message(Boost settings:)
message( Include: $$BOOST_INCLUDE)
message( Lib path: $$BOOST_LIB_PATH)
message( Libs: $$BOOST_LIB)


QMAKE_CXXFLAGS += -fopenmp

LIBS += $$BOOST_LIB_PATH $$BOOST_LIBS

INCLUDEPATH += $$MGL_SRC/.. $$BOOST_INCLUDE

SOURCES +=     $$MGL_SRC/abstractable.cc \
    $$MGL_SRC/clipper.cc\
    $$MGL_SRC/configuration.cc\
    $$MGL_SRC/gcoder.cc\
    $$MGL_SRC/infill.cc\
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
