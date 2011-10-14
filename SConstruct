#
# Top-level SConstruct file for Miracle Grue.
#

# env = Environment(tools=['default','qt4'])

# On Mac builds, scons complains that Qt4 can't be found. By mergeing the PATH environment variable
# the moc tool is detected and Qt4 is detected 
env = Environment(ENV = {'PATH' : os.environ['PATH']}, tools=['default','qt4'])


env.EnableQt4Modules([
	'QtCore',
	'QtNetwork'
	])

env.Program('qt4Hello', 
		['hello.cc',  'src/Operation.h', 'src/SliceOperation.cc', 'src/DebugOperation.cc'])