# Requirements

Building Miracle Grue requires that you install scons and the Qt4 libraries. The Qt4 tools for scons are included in the source tree.

## Ubuntu installation

    apt-get install libqt4-dev scons libcppunit-dev libcppunit-doc
		   
 
## OS X installation

Install QT4 (http://qt.nokia.com/downloads)

check that qt is installed, and that the version is above 4: 
in a terminal window, type "moc -v"

make sure the directory that contains the qt tools is in your PATH environment variable.
in a terminal window, type "which moc"

You can also add the QT4DIR variable to your environment (this will get rid of the warning message)
export QT4DIR=/opt/local

If you are using an IDE (ex: Eclipse), you may find that your environment variables are different than the ones in the Terminal.
You can learn how to set your global environment here: http://stackoverflow.com/questions/7501678/set-environment-variables-on-mac-os-x-lion



# How to build the tool pather

Our build system is SCons v2.0.1 or later. You should be able to build the pather with reasonable defaults by running "scons" with no arguments.
