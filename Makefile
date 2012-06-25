ifeq ($(OS),Windows_NT)
    #Windows stuff
    SCONS_CMD = cmd /c scons.bat
    SCONS_CLN = cmd /c scons.bat -c
else
    #Linux stuff
    SCONS_CMD = scons
    SCONS_CLN = scons -c
endif

all:
	$(SCONS_CMD)
clean:
	$(SCONS_CLN)