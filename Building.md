# Miracle-Grue

Miracle-Grue is a C++ project. It is a multiplatform desktop application that slices 3D models and creates GCODE files.

## Requirements
* Python
* Scons
* QT 4.0 (or greater)
* CPPUnit

## Build Instructions

### Import Submodules
To update all submodules, in a terminal window, issue

	git submodule init
	git submodule update


###Ubuntu Requirements
Ubuntu dependencies can be obtained using apt-get.  In a terminal window, issue:

	apt-get install libqt4-dev scons libcppunit-dev libcppunit-doc
	apt-get install libboost-dev libboost-filesystem-dev libboost-regex-dev libboost-system-dev
    
###Windows Requirements

* Python 
	* Python 2.7 and greater should work fine 
 	* Add python.exe to your PATH variable.
* Qt SDK. 
	* Download the QtSDK from http://qt.nokia.com/downloads 
	* Add the QtSDK/mingw/bin directory to your path variable
 	* Add QtSDK/Desktop/Qt/<Version>/mingw/bin directory to your path variable
* CppUnit (for unittests)
	* Download from sourceforge.net/apps/mediawiki/cppunit/
	* configure, make and make install it, then move the created bin, lib and include directory to QtSDK/mingw
* Scons
	* Download and install scons from http://www.scons.org/
    	 
Miracle Grue can be built VIA Visual Studio 2010 (Express).  Files associated with building Miracle Grue in VS2010 are available in the VS2010 directory.  Additionally contained within that directory are Cppunit and associated binary files used for 64bit operating systems.

Cppunit is only supported in Visual Studio 6.  See http://stackoverflow.com/questions/5462852/how-to-build-cppunit-with-visual-studio-2010 for information on how to build Cppunit with Visual Studio 2010.
	
Boost exists as a binary on jenkins for makerbot developers. 
For non makerboters, email joseph.sadusk@makerbot.com until he makes the download available.
		  
###OS X Requirements
We recommend acquiring dependencies Miracle Grue's dependencies VIA MacPorts.

* Scons:

		port install scons

* CPPUNIT

		port install cppunit

* QT
	* Goto http://qt.nokia.com/downloads and download the QT SDK
 	* To check that your version of QT is 4.0 or greater, in a terminal window, issue:
  
			moc -v 

 	* To configure QT, goto http://doc-snapshot.qt-project.org/4.8/install-x11.html 
  	* If you are using an IDE (ex: Eclipse), you may find that your environment variables are different than the ones in the Terminal. You can learn how to set your global environment here: http://stackoverflow.com/questions/7501678/set-environment-variables-on-mac-os-x-lion

* Boost

		port install boost

###OS X Deployment
To update all files, in a terminal, issue:

		git pull
		git submodule init
		git submodule update

To generate makefiles and invoke make, 'cd' to the root directory of the file tree, and issue:

		mkdir -p build
		cd build
		qmake -spec macx-g++ ../miracle_gui.pro
		make

To create a deployable .dmg file:

		macdeployqt miracle_gui.app -dmg -verbose=2


### Compiling the code

To compile, 'cd' to the root directory of the file tree and issue:

		scons
    
To clean the project (remove .o files), issue:
	
		scons -c

To compile with debug symbols, issue:

		define MG_DEBUG=1
		export MG_DEBUG=0

