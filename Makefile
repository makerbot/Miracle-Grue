#Debug flag
DBG_FLAG = --debug_build
#Build unit tests
TST_FLAG = --unit_tests=build
#Build GUI app
GUI_FLAG = --gui
#Clean flag
CLN_FLAG = -c

ifeq ($(OS),Windows_NT)
    #Windows stuff
    SCONS_CMD = cmd //c scons.bat
else
    #Linux stuff
    SCONS_CMD = scons
endif

all:
	$(MAKE) app test gui
all_debug:
	$(MAKE) app_debug test_debug gui_debug
app:
	$(SCONS_CMD)
app_debug:
	$(SCONS_CMD) $(DBG_FLAG)
test:
	$(SCONS_CMD) $(TST_FLAG)
test_debug:
	$(SCONS_CMD) $(TST_FLAG) $(DBG_FLAG)
gui:
	$(SCONS_CMD) $(GUI_FLAG)
gui_debug:
	$(SCONS_CMD) $(GUI_FLAG) $(DBG_FLAG)
clean:
	$(SCONS_CMD) $(CLN_FLAG)


