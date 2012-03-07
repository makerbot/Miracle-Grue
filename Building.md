# Introduction

	Miracle-Grue is a C++ project. It is a multiplatform desktop application that slices 3D models and creates GCODE files.

# Requirements

	Building Miracle Grue requires that you install scons (which requres Python), cppunit and the Qt4 libraries. The Qt4 tools for scons are included in the source tree.

	QT4 is not used yet, so you can get away without installing it, if you are willing to manually edit the SConstruct file. 


**Ubuntu Requirements**

	apt-get install libqt4-dev scons libcppunit-dev libcppunit-doc
    
**Windows Requirements**

	We are still working on this. 
    
	Scons: scons requires Python, which may not be on your system.
    	 
	Miracle Grue can be built under Visual Studio 2010 (Express):
		The files are in the VS2010 directory
		cppunit include files and binary libraries (64bit) have been added.

	cppunit is only supported on Visual Studio 6, so if you need different versions of the libraries you will need to compile them.
	see http://stackoverflow.com/questions/5462852/how-to-build-cppunit-with-visual-studio-2010 for more info.
		  
**OS X Requirements**

	To install SCONS, we recommend that you download MacPorts at macports.org/install.php. Once installed, to download/install scons and all dependent libraries, open a terminal window and type:
		"sudo port install scons"

	To install CPPUNIT, we recommend that you first download MacPorts at macports.org/install.php.  Once installed, to download/install cppunit and all dependent libraries, open a terminal window and type:
		"sudo port install cppunit"	

	Install Qt libraries 4.6.4 for Mac (Cocoa) (http://qt.nokia.com/downloads)

	Check that qt is installed, and that the version is above 4.  In a terminal window, type 

		"moc -v"

	Make sure the directory that contains the qt tools is in your PATH environment variable.  In a terminal window, type 

		"which moc"

	You can also add the QT4DIR variable to your environment (this will get rid of the warning message).  In a terminal window, type

		"export QT4DIR=/opt/local"

	If you are using an IDE (ex: Eclipse), you may find that your environment variables are different than the ones in the Terminal. You can learn how to set your global environment here: http://stackoverflow.com/questions/7501678/set-environment-variables-on-mac-os-x-lion

# Compiling the code

	Miracle-Grue does not rely on Makefiles, but instead uses scons (http://www.scons.org/) as the build tool (python required).  Once installed, you can use the following command in a terminal window to build the project from the Miracle-Grue directory:

		"scons"
    
	To clean the project (remove .o files), in a terminal window type:
	
		"scons -c"

# Compiling with debug symbols:

	define MG_DEBUG=1 in your environment:

    	"export MG_DEBUG=0"

# Compiling without QT:

	define MG_QT=0 in your environment:

    	"export MG_QT=0"
