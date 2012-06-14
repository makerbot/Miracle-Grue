echo Removing .o files...
find . | grep "\.o$" | xargs rm -v
echo Removing Makefiles
find src/ | grep "Makefile" | xargs rm -v
rm -v Makefile*