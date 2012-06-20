
!noclean {
	system(python clean_qmake_files.py)
}

# Build versioning
GIT_COMMIT = $$(GIT_COMMIT)
isEmpty(GIT_COMMIT) {
    GIT_COMMIT = $$system(git rev-parse --short HEAD)
}
GIT_BRANCH = $$(GIT_BRANCH)
isEmpty(GIT_BRANCH) {
    GIT_BRANCH = $$system(git rev-parse --abbrev-ref HEAD)
}
BUILD_NUMBER = $$(BUILD_NUMBER)
isEmpty(BUILD_NUMBER) {
    BUILD_NUMBER = 'manual_build'
}

win32 & !win32-msvc* {
        QMAKE_LFLAGS += -static -static-ligbcc -static-libstdc++
}

MAJOR=0
MINOR=05

VERSION = $${MAJOR}.$${MINOR}

CHECKIN = '$${GIT_BRANCH}-$${GIT_COMMIT}'

DEFINES += VERSION_STR=\\\"$${VERSION}\\\"
DEFINES += CHECKIN_STR=\\\"$${CHECKIN}\\\"
DEFINES += BUILD_STR=\\\"$${BUILD_NUMBER}\\\"

TEMPLATE = subdirs
SUBDIRS = src/mgl src/miracle_grue

src/miracle_grue.depends = mgl

test {
	win32 {
		system(cd src/unit_tests && python make_test_pro.py)
	}
	!win32 {
		system(cd src/unit_tests ; python make_test_pro.py)
	}
    include(src/unit_tests/unit_tests.pri)
}

mac {
    CONFIG -= app_bundle
    QT -= gui core
    LIBS -= -lQtGui -lQtCore

}
