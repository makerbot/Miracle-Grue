#
# Top-level SConstruct file for Miracle Grue.
#

env = Environment(tools=['default','qt4'])

env.EnableQt4Modules([
	'QtCore',
	'QtNetwork'
	])

env.Program('qt4Hello', 
		['hello.cc',  'src/Operation.h', 'src/SliceOperation.cc', 'src/DebugOperation.cc'])