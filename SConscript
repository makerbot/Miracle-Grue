import os
import sys
import re
import commands
import datetime
import multiprocessing

jcore_count = multiprocessing.cpu_count() * 1.5 
unitTestOutputDir = './test_cases'

AddOption('--unit_tests', default=None, dest='unit_test')
AddOption('--test', action='store_true', dest='test')
AddOption('--gui', action='store_true', dest='gui')

debug = GetOption('debug_build')
testmode = GetOption('unit_test')
build_gui = GetOption('gui')
test_option = GetOption('test')

build_unit_tests = False
run_unit_tests = False

if test_option:
    testmode = 'run'

if testmode is not None:
    if testmode == 'run':
        build_unit_tests = True
        run_unit_tests = True
    elif testmode == 'build':
        build_unit_tests = True

print 'Targets: '+', '.join(BUILD_TARGETS)

def detectLatestQtDir(operating_system, compiler_type):
    if os.environ.get('QTDIR') is not None:
        print 'Using env variable QTDIR: '+os.environ.get('QTDIR')
        return os.environ.get('QTDIR')
    elif operating_system.startswith('linux'):
        return '/usr' #use the system qt install
    elif operating_system == 'win32':
        qtbase = 'c:\\QtSDK\\Desktop\\Qt'
    elif operating_system == 'darwin':
        qtbase = '~/QtSDK/Desktop/Qt'
    versions = re.compile('4\.(\d+)\.(\d+)')
    minor = 0
    release = 0
    for dir in os.listdir(qtbase):
        match = versions.match(dir)
        if match is not None:
            if int(match.group(1)) > minor:
                minor = int(match.group(1))
                release = int(match.group(2))
            elif (int(match.group(1)) == minor) \
                     and (int(match.group(2)) > release):
                release = int(match.group(2))

    qtver = '4.{minor}.{release}'.format(minor=minor, release=release)

    qtdir = None
    if operating_system == 'win32':
        qtdir = '{base}\\{ver}\\{compiler}'.format(base=qtbase, ver=qtver,
                                                 compiler=compiler_type)
    elif operating_system == 'darwin':
        print "Darwin!"
        qtdir = '{base}/{ver}/gcc'.format(base=qtbase, ver=qtver)

    print 'Found latest qt at {dir}'.format(dir=qtdir)
    return qtdir
#end def detectLatestQtDir(...)

def get_environment_flag(flag_name, default):
    flag = default
    try:
        flag = os.environ[flag_name]
        if int(flag):
             flag = True
    except:
        flag = False 
    return flag
#end def get_environment_flag(...)

def runThisTest(testname):
    sts,text = commands.getstatusoutput(
        'bin/unit_tests/{}UnitTest'.format(testname))
    print(text)
#end def runThisTest(...)

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
#end def runUnitTest(...)

def mix(*args):
	""" mash items/lists together into a single list, no duplicates"""
	l = []
	for arg_list in args:
		l += arg_list

	no_duplicates =  list(set(l)) # remove duplicates
	#print "***", no_duplicates
	scons_list = [env.Object(x) for x in no_duplicates]
	return scons_list
#end def mix(*)

print ""
print "======================================================="
print "Miracle-Grue build script"
print " * it is now", datetime.datetime.now(), " (Qt and cppUnit are sold separately)"
print

operating_system = sys.platform

print "Operating system: [" + operating_system + "]"

default_libs = []
default_includes = ['submodule/json-cpp/include',
		    'submodule/EzCppLog', 
		    'submodule/optionparser/src',
            'src/eigen']

tools = ['default']

if operating_system.startswith("linux"):
    print " ** CPPUNIT version checK:", commands.getoutput("dpkg -l|grep cppunit-dev")
    default_libs_path = ['/usr/lib', '/usr/local/lib', './bin/lib']

compiler_type = None



env = Environment(ENV = {'PATH' : os.environ['PATH']}, CPPPATH='src', tools=tools)

if operating_system == "darwin":
    default_includes.append('/opt/local/include')
    default_includes.append(
      '/System/Library/Frameworks/CoreFoundation.framework/Versions/Current/Headers')
    default_libs_path = ['/opt/local/lib', './bin/lib']
    env['FRAMEWORKS'] = ['CoreFoundation']

if operating_system == "win32":
    AddOption('--compiler_type', dest="compiler_type", default="mingw" )
    compiler_type = GetOption('compiler_type')
    if compiler_type == "mingw":
        env.Replace(CCFLAGS=[])
        env.Tool('mingw')
    elif compiler_type == "cl":
        pass
    else:
        print "Unknown compiler {}: Only mingw or cl supported on windows".format(compiler_type)
        exit(1)
    default_libs_path = ['.\\bin\\lib']
    if os.path.exists('c:/Program Files (x86)'):
        prefix = 'c:/Program Files (x86)/'
    else:
        prefix = 'c:/Program Files/'
    default_libs_path.append(prefix + 'MakerBotSDK/mingw/lib')
    default_includes.append(prefix + 'MakerBotSDK/mingw/include')
    

env.Append(CCFLAGS = ['-Wall', '-Wextra'])
debug_profile = False
if debug:
    env.Append(CCFLAGS = '-g')
    if debug_profile:
        env.Append(CCFLAGS = '-pg')
        env.Append(CCFLAGS = '-fprofile-arcs')
        env.Append(LINKFLAGS = '-pg')
        env.Append(LINKFLAGS = '-fprofile-arcs')
        env.Append(LINKFLAGS = '-fprofile-arcs')
        env.Append(LIBS = 'gcov')
else:
    env.Append(CCFLAGS = '-O2')

#env.Append(CCFLAGS = '-j'+ str(int(jcore_count)))
multi_thread = False
if  multi_thread:  
    env.Append(CCFLAGS = '-fopenmp -DOMPFF')      
    env.Append(LINKFLAGS = '-fopenmp')    

#if qt:
#	print "OS: ", operating_system
#	print " ** QT version check:",  commands.getoutput("moc -v")
#	print	
#	qtModules = ['QtCore', 'QtNetwork' ]
#	print "QT modules", qtModules
#	env.EnableQt4Modules(qtModules)

mgl_cc = Glob('src/mgl/*.cc')
toolpathviz_cc = Glob('submodule/toolpathviz/*.cpp')
toolpathviz_ui = ['submodule/toolpathviz/mainwindow.ui']

env.Library('./bin/lib/mgl', mgl_cc, CPPPATH=['src', default_includes])

unit_test   = ['src/unit_tests/UnitTestMain.cc',
	       'src/unit_tests/UnitTestUtils.cc']



default_libs.extend(['mgl', 'json'])

debug_libs = ['cppunit']
debug_libs_path = ["", ]

env.Append(CPPPATH = default_includes)
env.Append(LIBS = default_libs)
env.Append(LIBPATH = default_libs_path)


p = env.Program('./bin/miracle_grue', 
		mix(['src/miracle_grue/miracle_grue.cc'] ))

binaries = [p]

j = env.Program('./bin/get_slice',
                mix(['src/miracle_grue/get_slice.cc'] ))

if build_gui:
    print "Building miracle_gui"
    qtEnv = env.Clone()
    qtEnv['QT4DIR'] = detectLatestQtDir(operating_system, compiler_type)
    qtEnv['ENV']['PKG_CONFIG_PATH'] = '/usr/lib/pkgconfig'
    qtEnv.Tool('qt4', toolpath=[Dir('submodule/mw-scons-tools')])
    qtEnv.EnableQt4Modules(['QtGui',
                            'QtCore',
                            'QtOpenGL'])
    ui = qtEnv.Uic4(toolpathviz_ui)
    p = qtEnv.Program('bin/miracle_gui',
                    toolpathviz_cc)
    binaries.append(p)

gettestname = re.compile('^(.*)TestCase\.cc')
tests = []
for filename in os.listdir('src/unit_tests'):
    match = gettestname.match(filename)
    if match is not None:
        testname = match.group(1)
        tests.append(testname)


if build_unit_tests:
    testEnv = env.Clone()
    testEnv.Append(LIBS = debug_libs)
    testEnv.Append(LIBSPATH = debug_libs_path)
    for testname in tests:
        p = testEnv.Program('bin/unit_tests/{}UnitTest'.format(testname),
                          mix(['src/unit_tests/{}TestCase.cc'.format(testname)],
                              unit_test))

if run_unit_tests:
    for testname in tests:
        testfile = 'bin/unit_tests/{}UnitTest'.format(testname)
        testEnv.Command('runtest_'+testname, testfile, testfile)

install_prefix = ARGUMENTS.get('install_prefix', '')
config_prefix = ARGUMENTS.get('config_prefix', '')

if sys.platform == "linux2":
    if install_prefix == '': install_prefix = '/usr'

    mg_bin = install_prefix + "/bin"
    mg_conf = install_prefix + "/share/miracle-grue"
    mg_include = install_prefix + '/include'

elif sys.platform == "darwin":
    framework_dir = install_prefix + '/Library/Frameworks/MakerBot.framework/Makerbot'

    mg_bin = framework_dir + "/Miracle-Grue/bin/"
    mg_conf = framework_dir + "/Miracle-Grue"
    mg_include = install_prefix + '/Miracle-Grue'

elif sys.platform == "win32":
    if install_prefix == '':
        if os.path.exists('c:/Program Files (x86)'):
            install_prefix = 'c:/Program Files (x86)/'
        else:
            install_prefix = 'c:/Program Files/'
        install_prefix += 'MakerBotSDK/mingw'
        binaries.append(install_prefix + '/lib/json.dll')

    mg_bin = install_prefix + '/bin'
    mg_conf = install_prefix + '/..'
    mg_include = install_prefix + '/include'


install_list = map(lambda x: env.Install(mg_bin, x), binaries)
install_list += map(lambda x: env.Install(mg_conf, x), Glob("#/*.config"))
install_list += map(lambda x: env.Install(mg_include, x), [])

env.Alias('install', install_list)



