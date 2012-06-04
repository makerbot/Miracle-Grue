
TEMPLATE = subdirs
SUBDIRS = src/mgl src/miracle_grue

src/miracle_grue.depends = mgl

test {
	system(cd src/unit_tests; python make_test_pro.py)
    include(src/unit_tests/unit_tests.pri)
}
