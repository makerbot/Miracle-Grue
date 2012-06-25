#Debug flag
DBG_FLAG = --debug_build
#Build unit tests
TST_FLAG = --unit_tests
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
	$(SCONS_CMD)
debug:
	$(SCONS_CMD) $(DBG_FLAG)
test:
	$(SCONS_CMD) $(TST_FLAG)
gui:
	$(SCONS_CMD) $(GUI_FLAG)
clean:
	$(SCONS_CMD) $(CLN_FLAG)