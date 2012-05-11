
#-------------------------------------------------
#
# QtCreator project include file
#
#-------------------------------------------------


win32 {
    BOOST_LIB_PATH=-Lc:\Boost\lib
    BOOST_INCLUDE=-Ic:\Boost\include\boost-1_49
    BOOST_LIBS=-lboost_system-mgw46-mt-1_49 \
               -lboost_filesystem-mgw46-mt-1_49 \
               -lboost_regex-mgw46-mt-1_49
}

linux {
    BOOST_LIB_PATH=
    BOOST_LIBS=-lboost_system -lboost_filesystem -lboost_regex
    BOOST_INCLUDE=    
}

mac {
	BOOST_PATH=/opt/local/lib
	BOOST_LIBS=$$BOOST_PATH/libboost_system.a \
	           $$BOOST_PATH/libboost_filesystem.a \
			   $$BOST_PATH/libboost_regex.a
}

LIBS += $$BOOST_LIB_PATH $$BOOST_LIBS -fopenmp
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
    $$MGL_SRC/slicy.cc\
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
    $$MGL_SRC/slicy.h\
    $$MGL_SRC/ScadDebugFile.h \
    $$MGL_SRC/log.h \
    $$MGL_SRC/grid.h \
