
TEMPLATE = subdirs
SUBDIRS = src/mgl src/miracle_grue

src/miracle_grue.depends = mgl

test {
    include(src/unit_tests/unit_tests.pri)
}
