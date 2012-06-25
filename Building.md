# Introduction

Miracle-Grue is a C++ project. It is a multiplatform desktop application that slices 3D models and creates GCODE files.

# Requirements

Building Miracle Grue requires that you install scons (which requres Python), cppunit and the Qt4 libraries. The Qt4 tools for scons are included in the source tree.

QT4 is only used by the miracle_gui interface.  If you only want to use the command line utility, you don't have to install QT.


## Import Submodules
 EzLogger, json-cpp and optionparser are used as submodules, as well as two other projects of ours, libthing and toolpathviz. The submodule tree is complex, so its easiest to clone the entire repository with --recursive, however if you already have it cloned you can do:
 'git submodule update --recursive --init'


**Ubuntu Requirements**

	apt-get install libqt4-dev scons libcppunit-dev libcppunit-doc
    
**Windows Requirements**

-Install Python. Version 2.7 and up should work. Add python.exe to your PATH variable.
-Install the Qt SDK. Add the included QtSDK/mingw/bin directory as well as QtSDK/Desktop/Qt/<Version>/mingw/bin directories to your path.
-For unit tests to compile, download CppUnit from sourceforge.net/apps/mediawiki/cppunit/
	-configure, make, make install, then move the created bin, lib, and include directory to QtSDK/mingw
    
-Scons: http://www.scons.org/ Scons requires python so install after that is set up
    	 
	
**OS X Requirements**

To install SCONS, we recommend that you download MacPorts at macports.org/install.php. Once installed, to download/install scons and all dependent libraries, open a terminal window and type:
	sudo port install scons

To install CPPUNIT, we recommend that you first download MacPorts at macports.org/install.php.  Once installed, to download/install cppunit and all dependent libraries, open a terminal window and type:
	sudo port install cppunit

Install Qt libraries 4.6.4 for Mac (Cocoa) (http://qt.nokia.com/downloads)

Check that qt is installed, and that the version is above 4.  In a terminal window, type 

	moc -v

Make sure the directory that contains the qt tools is in your PATH environment variable.  In a terminal window, type 

	which moc

You can also add the QT4DIR variable to your environment (this will get rid of the warning message).  In a terminal window, type

	export QT4DIR=/opt/local

If you are using an IDE (ex: Eclipse), you may find that your environment variables are different than the ones in the Terminal. You can learn how to set your global environment here: http://stackoverflow.com/questions/7501678/set-environment-variables-on-mac-os-x-lion


***OSX deployment***

Update the code if necessary, cd to the directory containing  the tree, and then:

	mkdir -p build
	cd build
	qmake -spec macx-g++ ../Miracle-Grue/miracle_gui.pro
	make

To create a deployable .dmg file:

	macdeployqt miracle_gui.app -dmg -verbose=2


**Compiling the code**
	
Once you have the code from git, don't forget to update the code from the submodules:
	
    git submodule update --recursive

Miracle-Grue does not rely on Makefiles, but instead uses scons (http://www.scons.org/) as the build tool (python required).  Once installed, you can use the following command in a terminal window to build the project from the Miracle-Grue directory:

	scons
    
To clean the project (remove .o files), in a terminal window type:
	
	scons -c

***Compiling with debug symbols ***

Pass scons the debug_build option to build with debug symbols

	scons --debug_build

***Compiling miracle_gui***

To build the QT GUI for Miracle-Grue, run scons with the gui option

    scons --gui

*** Compiling unit tests ***

To build unit tests run scons with the unit_tests option, set to build to just compile them, run to compile and run them.

    scons --unit_tests=build
    scons --unit_tests=run



