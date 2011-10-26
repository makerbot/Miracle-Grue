#
# Top-level SConstruct file for Miracle Grue.
#

# env = Environment(tools=['default','qt4'])

# On Mac builds, scons complains that Qt4 can't be found. By mergeing the PATH environment variable
# the moc tool is detected and Qt4 is detected 
import os
import commands



print ""
print "======================================================="
print "So you think you can build Miracle-Grue on your machine?"
print ""
print "* Qt detector: '%s'" % commands.getoutput("moc -v")
print "* Looking for cppunit"

cppunit_lib_dir = ""
cppunit_inc_dir = ""

try:
	print "    MG_CPPUNIT_LIB_DIR environment variable:"
	cppunit_lib_dir = os.environ['CPPUNIT_LIB_DIR']
	print "         '%s'" % cppunit_lib_dir
	print "    MG_CPPUNIT_INC_DIR environment variable:"
	cppunit_inc_dir = os.environ['CPPUNIT_INC_DIR']
	print "         '%s'" % cppunit_inc_dir
	print 
except:
	print "WARNING: "
	print "Expected environment variables for libraries not found. Continuning anyway"
	
# Using just one environemt setup for now	
env = Environment(ENV = {'PATH' : os.environ['PATH']}, tools=['default','qt4'])
env.EnableQt4Modules(['QtCore', 'QtNetwork' ])

#env.Program('qt4Hello', ['hello.cc',  'src/Operation.h', 'src/SliceOperation.cc', 'src/DebugOperation.cc'])

# env.Program('gcoder_test',['src/GCoderOperation.cc', 'src/Configuration.cc', 'gcoder_test_main.cc']) 

# /usr/local/include/cppunit/
# Program('prog.c', LIBS = 'm',
#                       LIBPATH = ['/usr/lib', '/usr/local/lib'])

#env.Program('unit_operation',['src/ExampleTestCase.cc', 'src/unit_operation_main.cc'],
#		 CPPPATH=[cppunit_inc_dir], LIBS = 'cppunit', LIBPATH = ['/usr/lib', '/usr/local/lib',cppunit_lib_dir]) 

env.Program( 'gcoderUnitTest',['src/GCoderOperation.cc', 'src/Configuration.cc',
							'src/unit_tests/UnitTestMain.cc', 
							'src/unit_tests/GCoderTestCase.cc', 
							'src/PathData.cc',
							'src/FileWriterOperation.cc',
							'src/PatherOperation.cc',
							'src/RegionData.cc'],
		  LIBS = 'cppunit', 
		  LIBPATH = ['/usr/lib', '/usr/local/lib',cppunit_lib_dir],
		  CPP_PATH= ['..']) 

env.Program( 'modelReaderUnitTest',['src/ModelFileReaderOperation.cc', 'src/Configuration.cc',
							'src/unit_tests/UnitTestMain.cc', 'src/unit_tests/ModelReaderTestCase.cc', 'src/MeshData.cc'],
		  LIBS = 'cppunit', 
		  LIBPATH = ['/usr/lib', '/usr/local/lib',cppunit_lib_dir]) 

env.Program( 'slicerUnitTest',['src/SliceOperation.cc', 'src/Configuration.cc',
							'src/unit_tests/UnitTestMain.cc', 
							'src/unit_tests/SlicerTestCase.cc', 
							'src/SliceOperation.cc',
							'src/RegionData.cc',
							'src/MeshData.cc'],
		  LIBS = 'cppunit', 
		  LIBPATH = ['/usr/lib', '/usr/local/lib',cppunit_lib_dir]) 


env.Program( 'regionerUnitTest',['src/RegionerOperation.cc', 'src/Configuration.cc',
							'src/unit_tests/UnitTestMain.cc', 
							'src/unit_tests/RegionerTestCase.cc', 
							'src/RegionerOperation.cc',
							'src/SliceData.cc',
							'src/RegionData.cc'],
		  LIBS = 'cppunit', 
		  LIBPATH = ['/usr/lib', '/usr/local/lib',cppunit_lib_dir]) 

env.Program( 'patherUnitTest',['src/PatherOperation.cc', 'src/Configuration.cc',
							'src/unit_tests/UnitTestMain.cc', 
							'src/unit_tests/PatherTestCase.cc', 
							'src/PathData.cc',
							'src/RegionData.cc'],
		  LIBS = 'cppunit', 
		  LIBPATH = ['/usr/lib', '/usr/local/lib',cppunit_lib_dir],
		  CPP_PATH= ['..']) 
