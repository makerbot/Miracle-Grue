echo Removing .o files...
find . | grep "\.o$" | xargs rm -v
echo Removing Makefiles
find . | grep "Makefile" | xargs rm -v