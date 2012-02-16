# Introduction

Miracle-Grue is a C++ project. It is a multiplatform desktop application that slices 3D models and creates GCODE files.

It does not rely on Makefiles, but instead uses scons (http://www.scons.org/) as the build tool (python required).
Once installed, you can use the following command to build the project from the Miracle-Grue directory:

### Compiling the code

    scons
    
To clean the project (remove .o files) type:
	
    scons -c



### Compiling with debug symbols:

define MG_DEBUG=1 in your environment:

    export MG_DEBUG=0

### Compiling without QT:

define MG_QT=0 in your environment:

    export MG_QT=0

# Requirements

Building Miracle Grue requires that you install scons, cppunit and the Qt4 libraries. 
The Qt4 tools for scons are included in the source tree.

QT4 is not used yet, so you can get away without installing it, if you are
willing to play with the SConstruct file. 


## Ubuntu installation

    apt-get install libqt4-dev scons libcppunit-dev libcppunit-doc
    
## Windows installation

    We're still working on this. 
    
    Scons: scons requires Python, which may not be on your system.
    
    cppunit is only supported on Visual Studio 6
    
	Miracle Grue can be built under eclipse with cygwin:
		install cygwin with python, cppunit, and mingw
		install scons on cygwin python
		when adding project to eclipse, select makefile project with existing code and under toolchain for indexer settings select "Cygwin GCC"
		there are some configuration files in configs/eclipse/ that may make setting up the project easier
		  
## OS X installation

Install Qt libraries 4.6.4 for Mac (Cocoa) (http://qt.nokia.com/downloads)

check that qt is installed, and that the version is above 4: 
in a terminal window, type "moc -v"

make sure the directory that contains the qt tools is in your PATH environment variable.
in a terminal window, type "which moc"

You can also add the QT4DIR variable to your environment (this will get rid of the warning message)
export QT4DIR=/opt/local

If you are using an IDE (ex: Eclipse), you may find that your environment variables are different than the ones in the Terminal.
You can learn how to set your global environment here: http://stackoverflow.com/questions/7501678/set-environment-variables-on-mac-os-x-lion


