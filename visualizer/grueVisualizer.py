#/usr/bin/python3
import optparse
import sys
import xml.dom.minidom
import logging
import json

def svgIsContour(srcFileName):
    "returns true of the file we have is a proper cslice as svg file, false otherwise"
    print(" srcFileName: ", srcFileName)
    fh = open(srcFileName)
    data = fh.read()
    fh.close()
    if data.find("""svg/cslice""") !=  -1:
        print("svg is cslice ")
        return True
    print("svg is not region")
    return False


def jsonContourListFromSvgPath(rawText):
    """ takes a raw contour svg text and returns a contour json list (including subtracted regions
    as a list """
    
    def parsePoints(pointsStr):
        toks = pointsStr.split(",")
        x0 = float(toks[0])
        y0 = float(toks[1])
        return (x0, y0)
        
    contourLists = []
    verifiedEnd = False
    entries = rawText.split(' ')
    for e in entries:
        if(len(e) == 0 ):
            continue
        if e[0] == 'M': #Move directive, new loop                
            #print("start of loop")
            contourLists.append([])
            pointsStr = e.strip('M')
            contourLists[-1].append(parsePoints(pointsStr))
            #print contourLists[-1]
        elif e[0] == 'L': #Move directive, new loop
            contourLists[-1].append(parsePoints(e.strip('L')))
            #print contourLists[-1]
        elif e[0] == 'Z': #Move directive, new loop
            #print ('end of loop')
            verifiedEnd = True #debugging only
            
    if not verifiedEnd:
            print("WARNING: No verified end to this contour!")
    
    return contourLists


def jsonnsFromXmlns(xmlns):
    """ converts known xml namespaces to known json namespaces. Returns 'namespace v 0.0.0.0 if 
    unknown namespace is entered
    """
    if xmlns == "http://spec.makerbot.com/svg/cslice/v_1.0.0.0":
        return "http://spec.makerbot.com/cslice/v_1.0.0.0"
    print("Error: Unknown xml namespace!")
    return "http://spec.makerbot.com/2011/cslice/v_0.0.0.0"

def xmlNsFromJsonns(jsonns):
    """ converts known xml namespaces to known json namespaces. Returns 'namespace v 0.0.0.0 if 
    unknown namespace is entered
    """
    if jsonns == "http://spec.makerbot.com/cslice/v_1.0.0.0":
        return "http://spec.makerbot.com/svg/cslice/v_1.0.0.0"
    print("Error: Unknown json namespace!")
    return "http://spec.makerbot.com/2011/svg/cslice/v_0.0.0.0"


def csliceDictFromSvgFile(srcFileName):
    """ Creates an entire valid cslice Dict from a cslice svg file """
    print('doing: svgFileToRegionDict')
    dom = xml.dom.minidom.parse(srcFileName)
    
    contoursList = []
    materialsList = []
    materials = dom.getElementsByTagName("material")
    for materialElement in materials:
        name = materialElement.getAttribute('name')
        support = True if materialElement.getAttribute('support') == 'true' else False
        materialsList.append({'name':name, 'support':support})
    paths = dom.getElementsByTagName("path" )
    for pathElement in paths:
            d = {}
            #print("path element:" + str(element)) 
            contourPathString = pathElement.getAttribute('d')
            materialString = pathElement.getAttribute("material")
            #print("raw path" + str(wholePath) )
            contourPointsList = jsonContourListFromSvgPath(contourPathString)
            #print(list)
            #print("subpaths: " +str(len(list)))
            d['material'] = materialString
            d['contours'] = contourPointsList
        # -- contourPointsList is a complete list of contour points
            contoursList.append(d)
#    print("----")
#    print(" contoursList", contoursList)
#    print("===")
    svgElement = dom.getElementsByTagName('svg')[0]
    jsonns = jsonnsFromXmlns(svgElement.getAttribute('xmlns') )
   
    sliceElement = dom.getElementsByTagName('slice')[0]
    zStr = sliceElement.getAttribute('z_position')
    zPosition = float(zStr)
    return {'materials':materialsList, 'contours':contoursList, 'jsonns':jsonns, 'z_position':zPosition}
    # do materials
    
def csliceDictFromCsliceFile(srcFileName):
    """ takes a json cslice file name, and generates a dict object 
    from it """
    try:
        fh = open(srcFileName)
        data = fh.read()
        fh.close()
        d = json.loads(data)
        return d
    except IOException as e:
        print "failsauce on csliceDictFromCsliceFile"
        print e
    return None


def contourElemStringFromDict(contDict):
    """ takes a dict element of just a contour from a cslice object,
    returns an xml cslice string for that contour"""
    #example {"material": "A", "contours": 
    #[
    #[[79.506, 25.12], [79.506, 38.669], [76.129, 40.02], [75.234, 40.02], [74.68, 40.573], [74.68, 43.51], [75.234, 44.063], [75.645, 44.063], [75.645, 56.387], [75.234, 56.387], [74.68, 56.941], [74.68, 59.877], [75.234, 60.431], [76.129, 60.431], [79.506, 61.781], [79.506, 77.519], [77.519, 79.506], [61.781, 79.506],"
    
    stringQueue = []
    stringQueue.append('<path class="contour" material="' + contDict['material'] +'" ')
    stringQueue.append('fill="green" stroke="none"')

    sQ = []
    for section in contDict['contours']:
        #print (section)
        s = []
        for coors in section:
            s.append( str(coors[0]) + ',' + str(coors[1]) )
        sQ.append('M' + ' L'.join(s) )
    pathString = ' d="' + ' '.join(sQ) + ' Z"'
    stringQueue.append(pathString)
    stringQueue.append("/>")
    
    return "".join(stringQueue)

    
    
def csliceSvgStringFromCsliceDict(csliceDict):
    """ takes a cslice dict, and returns a cslice svg string from it"""
    stringQueue = []
    stringQueue.append("""<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">""")
    stringQueue.append('\n<svg xmlns="' + xmlNsFromJsonns(csliceDict['jsonns']) + '"'  )
    stringQueue.append("""xml:space="preserve" style="shape-rendering:geometricPrecision; text-rendering:geometricPrecision; image-rendering:optimizeQuality; fill-rule:evenodd; clip-rule:evenodd" fill="none">""")
    stringQueue.append('<style type="text/css"></style>')
    header = ' '.join(stringQueue)
    
    stringQueue = []
    stringQueue.append('<slice z_position="' + str(csliceDict['z_position'])+ '" thickness="none"/>')
    slice = ''.join(stringQueue)

    stringQueue = []
    for m in csliceDict['materials']:
        stringQueue.append('<material name="' + m['name'] + '" support="' + str(m['support']) +'"/>')
    materials = ''.join(stringQueue)

    stringQueue = []
    for c in csliceDict['contours']:
        contourElementString = contourElemStringFromDict(c)
        stringQueue.append(contourElementString)
    allPaths = "\n".join(stringQueue)
    #print(allPaths)

    footer = "</svg>"
    
    parts = [header, slice, materials, allPaths, footer]
    return "\n".join(parts)
    
#    materials = ''.join(stringQueue)
#    materials = ''.join(stringQueue)



def main(argList):
    
    parser = optparse.OptionParser()
    parser.add_option('-d', "--debug", action='store_true', dest='debug')
    (opts,args) = parser.parse_args()
    
    #debug logging
    print("options:" + str(opts) )
    print("args: "+ str(args) )
    
    srcFile = None
    if( len(args) > 0):
        srcFile = args[0]
    else:
        print(" cannot convert, no source file")
        parser.usage()
    
    if srcFile.endswith(".svg"):
        print('converting svg to .region or .path')
        if ( svgIsContour(srcFile) ):
            print('converting svg to .region')
        csliceDict = csliceDictFromSvgFile(srcFile)
        print json.dumps(csliceDict)

    elif srcFile.endswith(".cslice"):
        print('converting svg to .')
        csliceDict = csliceDictFromCsliceFile(srcFile)
        if (csliceDict == None):
            print("csliceDict is None!! What did you do!!!!!")
            return -5
        svgCSliceString = csliceSvgStringFromCsliceDict(csliceDict)
        fh = open('tmp.svg','w')
        fh.write(svgCSliceString)
        fh.close()
        return 0

    elif srcFile.endswith(".cslices"):
        print('Can\'t do this yet .')
        return -4
        
    elif srcFile.endswith(".path"):
        print('converting svg to .')


if __name__ == "__main__":
    main(sys.argv)
    
    

    