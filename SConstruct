#
# Top-level SConstruct file for Miracle Grue.
#

# env = Environment(tools=['default','qt4']

# On Mac builds, scons complains that Qt4 can't be found. By merging the PATH environment variable
# the moc tool is detected and Qt4 is detected 
import os
import commands
import datetime
import multiprocessing

jcore_count = multiprocessing.cpu_count() * 1.5 


unitTestOutputDir = './test_cases'

# Add command line option '--unit_test to build command
AddOption('--unit_test')
run_unit_tests = False
if ( GetOption('unit_test') != None):
	print "running unit test"
	run_unit_tests = True;

#AddOption('--valgrind')
#run_valgrind = False
#if ( GetOption('valgrind') != None):
#	print "run valgrind"
#	run_valgrind = True;


def get_environment_flag(flag_name, default):
    flag = default
    try:
        flag = os.environ[flag_name]
        if int(flag):
             flag = True
    except:
        flag = False 
    return flag   

def runThisTest(program, run_unit_tests):
	if run_unit_tests:
		#print('runThisTest', program[0].path )
		sts,text = commands.getstatusoutput(program[0].path)
		print(text)


def runUnitTest(env,target,source):
	""" runs a unit test in a separate process. Not build-aware,
	but it does set a flag-file to signify the test ran to success
	@target dummy target file, writes 'passed' if tests return no error code
	@source  unit test program to run """
	import subprocess


	app = str(source[0].abspath)
	retCode = subprocess.call(app)
	if not retCode == 0:
		print "passed!"
		open(str(target[0]),'w').write("PASSED\n")
	else:
		print("runUnitTest failed testing " + str(source[0]) )
		#Message('runing unit test' + source[0].abspath)
		Exit(1)

def mix(*args):
	""" mash items/lists together into a single list, no duplicates"""
	l = []
	for arg_list in args:
		l += arg_list

	no_duplicates =  list(set(l)) # remove duplicates
	#print "***", no_duplicates
	scons_list = [env.Object(x) for x in no_duplicates]
	return scons_list

print ""
print "======================================================="
print "Miracle-Grue build script"
print " * it is now", datetime.datetime.now(), " (Qt and cppUnit are sold separately)"
print

operating_system = commands.getoutput("uname")


if operating_system == "Linux":
    print " ** CPPUNIT version checK:", commands.getoutput("dpkg -l|grep cppunit-dev")

if operating_system.find("_NT") > 0:
    print " ** CPPUNIT version checK:", "N/A"#commands.getoutput("cygcheck -l cppunit")

if operating_system == "Darwin":
    print " ** CPPUNIT version checK:", commands.getoutput("port info --line cppunit | grep ^cppunit")

debug = get_environment_flag('MG_DEBUG',False)
multi_thread = get_environment_flag('MG_MT', False)
qt =  get_environment_flag('MG_QT',False)
        
# Using just one environemt setup for now	
tools = ['default']
if qt:
	tools = tools+['qt4']
env = Environment(ENV = {'PATH' : os.environ['PATH']}, CPPPATH='src', tools=tools)
# print "os.environ['PATH']=", os.environ['PATH']


if operating_system == "Darwin":
    env.Append(CPPPATH = ['/opt/local/include'])
    env.Append(LIBPATH = ['/opt/local/lib'])

if debug:
    env.Append(CCFLAGS = '-g')

#env.Append(CCFLAGS = '-j'+ str(int(jcore_count)))

if  multi_thread:  
    env.Append(CCFLAGS = '-fopenmp -DOMPFF')      
    env.Append(LINKFLAGS = '-fopenmp')    
       

if qt:
	print "OS: ", operating_system
	print " ** QT version check:",  commands.getoutput("moc -v")
	print	
	qtModules = ['QtCore', 'QtNetwork' ]
	print "QT modules", qtModules
	env.EnableQt4Modules(qtModules)

mgl_cc = [	'src/mgl/mgl.cc',
			'src/mgl/configuration.cc', 
			'src/mgl/Vector2.cc',
			'src/mgl/Vector3.cc',
			'src/mgl/Triangle3.cc',
			'src/mgl/LineSegment2.cc',
			'src/mgl/Scalar.cc',
			'src/mgl/gcoder.cc',
			'src/mgl/shrinky.cc',
			'src/mgl/slicy.cc',
			'src/mgl/connexity.cc',
			'src/mgl/segment.cc',
			'src/mgl/miracle.cc',
			'src/mgl/infill.cc',
			'src/mgl/abstractable.cc',
			'src/mgl/JsonConverter.cc',
			'src/mgl/insets.cc',
			'src/mgl/clipper.cc']

json_cc = [ 'src/json-cpp/src/lib_json/json_reader.cpp',
            'src/json-cpp/src/lib_json/json_value.cpp',
            'src/json-cpp/src/lib_json/json_writer.cpp' ]


env.Library('./bin/lib/mgl', mgl_cc )  

env.Library('./bin/lib/_json', json_cc, CPPPATH=['src/json-cpp/include'])


unit_test   = ['src/unit_tests/UnitTestMain.cc',]



default_includes = ['..','src/json-cpp/include', 'src', 'src/BGL', 'src/mgl']
default_libs = [ 'mgl', '_json',] 
default_libs_path = ['/usr/lib', '/usr/local/lib', './bin/lib', '/opt/local/lib']

debug_libs = ['cppunit',]
debug_libs_path = ["", ]


p = env.Program('./bin/miracle_grue', 
		mix(['src/miracle_grue.cc'] ),
		LIBS = ['mgl', '_json'],
		LIBPATH = default_libs_path,
		CPPPATH = default_includes)

p = env.Program(  	'./bin/unit_tests/clipperUnitTest',   
				mix(['src/unit_tests/ClipperTestCase.cc',], unit_test), 
    			LIBS = default_libs + debug_libs,
				LIBPATH = default_libs_path + debug_libs_path, 
				 )
runThisTest(p, run_unit_tests)	

p = env.Program(  	'./bin/unit_tests/jsonConverterUnitTest',   
				mix(['src/unit_tests/JsonConverterTestCase.cc'], unit_test), 
    			LIBS = default_libs + debug_libs,
				LIBPATH = default_libs_path + debug_libs_path, 
				CPPPATH= ["."] )
runThisTest(p, run_unit_tests)	


p = env.Program(  	'./bin/unit_tests/mglCoreUnitTest',   
				mix(['src/unit_tests/MglCoreTestCase.cc'], unit_test), 
    			LIBS = default_libs + debug_libs,
				LIBPATH = default_libs_path + debug_libs_path, 
				CPPPATH= [".."])
runThisTest(p, run_unit_tests)	

p = env.Program(  	'./bin/unit_tests/slicerCupUnitTest',   
				mix(['src/unit_tests/SlicerCupTestCase.cc'], unit_test), 
    			LIBS = default_libs + debug_libs,
				LIBPATH = default_libs_path + debug_libs_path, 
				CPPPATH= [".."])
runThisTest(p, run_unit_tests)	


p = env.Program( 	'./bin/unit_tests/slicerUnitTest', 
				mix(['src/unit_tests/SlicerTestCase.cc'], unit_test), 
				LIBS = default_libs + debug_libs,
				LIBPATH = default_libs_path + debug_libs_path, 
				CPPPATH= ['..'])
runThisTest(p, run_unit_tests)
	#Command('slicerUnitTest.passed','./bin/tests/slicerUnitTest',runUnitTest)


p = env.Program(  	'./bin/unit_tests/modelReaderUnitTest',   
				mix(['src/unit_tests/ModelReaderTestCase.cc'], unit_test), 
				LIBS = default_libs + debug_libs,
				LIBPATH = default_libs_path + debug_libs_path, 
				CPPPATH= ['..'])
runThisTest(p, run_unit_tests)


p = env.Program( 	'./bin/unit_tests/gcoderUnitTest', 
				mix(['src/unit_tests/GCoderTestCase.cc'], unit_test), 
				LIBS = default_libs + debug_libs,
				LIBPATH = default_libs_path + debug_libs_path, 
				CPPPATH= default_includes )
runThisTest(p, run_unit_tests)


p = env.Program( 	'./bin/unit_tests/slicerSplitUnitTest', 
				mix(['src/unit_tests/SlicerSplitTestCase.cc'], unit_test), 
				LIBS = default_libs + debug_libs,
				LIBPATH = default_libs_path + debug_libs_path, 
				CPPPATH= ['..'])
runThisTest(p, run_unit_tests)

#p = env.Program(  	'./bin/unit_tests/regionerUnitTest',   
#				mix(['src/unit_tests/RegionerTestCase.cc'], unit_test), 
#				LIBS = default_libs + debug_libs,
#				LIBPATH = default_libs_path + debug_libs_path, 
#				CPPPATH= ['..'])
#
#runThisTest(p, run_unit_tests)



