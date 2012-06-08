# Introduction

	Miracle-Grue is a C++ project. It is a multiplatform desktop application that slices 3D models and creates GCODE files.

# Requirements

	Building Miracle Grue requires that you install scons (which requres Python), cppunit and the Qt4 libraries. The Qt4 tools for scons are included in the source tree.

	QT4 is not used yet, so you can get away without installing it, if you are willing to manually edit the SConstruct file. 


## Import Submodules
 EzLogger and json-cpp are used as submodules. On your first checkout before you
 build, you should run 
 'git submodule init' to setup the submodules
 'git submodule update' to do an initial fetch 


**Ubuntu Requirements**

	apt-get install libqt4-dev scons libcppunit-dev libcppunit-doc
	apt-get install libboost-dev libboost-filesystem-dev libboost-regex-dev libboost-system-dev
    
**Windows Requirements**

	We are still working on this. 
	
	-Install Python. Version 2.7 and up should work. Add python.exe to your PATH variable.
	-Install the Qt SDK. Add the included QtSDK/mingw/bin directory as well as QtSDK/Desktop/Qt/<Version>/mingw/bin directories to your path.
	-For unit tests to compile, download CppUnit from sourceforge.net/apps/mediawiki/cppunit/
		-configure, make, make install, then move the created bin, lib, and include directory to QtSDK/mingw
    
	Scons: scons requires Python, which may not be on your system.
    	 
	Miracle Grue can be built under Visual Studio 2010 (Express):
		The files are in the VS2010 directory
		cppunit include files and binary libraries (64bit) have been added.

	cppunit is only supported on Visual Studio 6, so if you need different versions of the libraries you will need to compile them.
	see http://stackoverflow.com/questions/5462852/how-to-build-cppunit-with-visual-studio-2010 for more info.

	
	boost exists as a binary on jenkins for makerbot developers
	for people outside of makerbot, joseph.sadusk@makerbot.com is 
 	the one to email until he makes a download available.FAILSAUCE

		  
**OS X Requirements**

	To install SCONS, we recommend that you download MacPorts at macports.org/install.php. Once installed, to download/install scons and all dependent libraries, open a terminal window and type:
		"sudo port install scons"

	To install CPPUNIT, we recommend that you first download MacPorts at macports.org/install.php.  Once installed, to download/install cppunit and all dependent libraries, open a terminal window and type:
		"sudo port install cppunit"	

	Install Qt libraries 4.6.4 for Mac (Cocoa) (http://qt.nokia.com/downloads)

	Install boot binaries
		"sudo port install boost"

	Check that qt is installed, and that the version is above 4.  In a terminal window, type 

		"moc -v"

	Make sure the directory that contains the qt tools is in your PATH environment variable.  In a terminal window, type 

		"which moc"

	You can also add the QT4DIR variable to your environment (this will get rid of the warning message).  In a terminal window, type

		"export QT4DIR=/opt/local"

	If you are using an IDE (ex: Eclipse), you may find that your environment variables are different than the ones in the Terminal. You can learn how to set your global environment here: http://stackoverflow.com/questions/7501678/set-environment-variables-on-mac-os-x-lion


***OSX deployment***

Update the code if necessary, cd to the directory containing  the tree, and then:

	"mkdir -p build"
	"cd build"
	"qmake -spec macx-g++ ../Miracle-Grue/miracle_gui.pro"
	"make"

To create a deployable .dmg file:

	"macdeployqt miracle_gui.app -dmg -verbose=2"


# Compiling the code
	
	Once you have the code from git, don't forget to update the code from the submodules:
	
	"git submodule init"
	"git submodule update"

	Miracle-Grue does not rely on Makefiles, but instead uses scons (http://www.scons.org/) as the build tool (python required).  Once installed, you can use the following command in a terminal window to build the project from the Miracle-Grue directory:

		"scons"
    
	To clean the project (remove .o files), in a terminal window type:
	
		"scons -c"

# Compiling with debug symbols:

	define MG_DEBUG=1 in your environment:

    	"export MG_DEBUG=0"

