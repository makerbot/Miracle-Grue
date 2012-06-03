TEST_BASE = src/unit_tests

SUBDIRS += $$TEST_BASE/ClipperTestCase
$$TEST_BASE/ClipperTestCase.depends = mgl

SUBDIRS += $$TEST_BASE/GCoderTestCase
$$TEST_BASE/GCoderTestCase.depends = mgl
