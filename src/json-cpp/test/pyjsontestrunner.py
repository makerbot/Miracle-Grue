# Simple implementation of a json test runner to run the test against json-py.

import sys
import os.path
import json
import types

def valueTreeToString( fout, value, path = '.' ):
    ty = type(value) 
    if ty  is types.DictType:
        fout.write( '%s={}\n' % path )
        suffix = path[-1] != '.' and '.' or ''
        names = value.keys()
        names.sort()
        for name in names:
            valueTreeToString( fout, value[name], path + suffix + name )
    elif ty is types.ListType:
        fout.write( '%s=[]\n' % path )
        for index, childValue in zip( xrange(0,len(value)), value ):
            valueTreeToString( fout, childValue, path + '[%d]' % index )
    elif ty is types.StringType:
        fout.write( '%s="%s"\n' % (path,value) )
    elif ty is types.IntType:
        fout.write( '%s=%d\n' % (path,value) )
    elif ty is types.FloatType:
        fout.write( '%s=%.16g\n' % (path,value) )
    elif value is True:
        fout.write( '%s=true\n' % path )
    elif value is False:
        fout.write( '%s=false\n' % path )
    elif value is None:
        fout.write( '%s=null\n' % path )
    else:
        assert False and "Unexpected value type"
        
def parseAndSaveValueTree( input, actual_path ):
    """ loads the input string into a json object,
    and write that object to 'actual path', returns
    the json object """
    root = json.loads( input )
    fout = file( actual_path, 'wt' )
    valueTreeToString( fout, root )
    fout.close()
    return root

def writeValueTree( value, output_path ):
    """ Takes a dict of values @value and a output
    path you want to write them into as json values @output_path"""
    writeme= json.dumps( value )
    file( output_path, 'wt').write( writeme + '\n' )
    return writeme 

def main(argv):    
    if len(argv) != 2:
        print "Usage: %s input-json-file", sys.argv[0]
        sys.exit(3)
    
    input_path = sys.argv[1]
    base_path = os.path.splitext(input_path)[0]
    actual_path = base_path + '.actual'
    rewrite_path = base_path + '.rewrite'
    rewrite_actual_path = base_path + '.actual-rewrite'

    input = file( input_path, 'rt' ).read()
    root = parseAndSaveValueTree( input, actual_path )
    rewrite = writeValueTree( root , rewrite_path )
    rewrite_root = parseAndSaveValueTree( rewrite, rewrite_actual_path )
    sys.exit( 0 )

if __name__ == '__main__':    
    main(sys.argv)

