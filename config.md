infillDensity:              decimal [0.0,1.0]
    Density of infill. 0.0 is no infill, 1.0 is solid objects. Intermediate values fill objects with a grid of approximatlely this density.
numberOfShells:             integer [0,infinity)
    Total number of shells. 0 is no shells (your object is infill only). Slicer will print up to the number of shells you specify, space permitting, and fill the rest with infill.
insetDistanceMultiplier:    decimal, ratio
    Distance between adjacent shells in multiples of the filament width
roofLayerCount:             integer [0,infinity)
    Number of solid layers of infill below roofs of the object
floorLayerCount:            integer [0,infinity)    
    Number of solid layers of infill above floors of the object
layerWidthRatio:            decimal, ratio
    Ratio between the width of the filament and the height of the layer
coarseness:                 decimal, millimeters    
    Moves below this length get combined, detail smaller than this gets smoothed
doGraphOptimizations:       boolean
    Enables processor intensive graph optimization. Takes longer to finish, but produces smarter paths. Not much impact on quality, but will avoid doing stupid moves and will generally finish printing faster.

rapidMoveFeedRateXY:        decimal, mm/sec
    Speed to move gantry between extrusions
rapidMoveFeedRateZ:         decimal, mm/sec
    Speed to move platform
    
doRaft:                     boolean
    Enables rafts. Options below are ignored if this is false
raftLayers:                 integer [0,infinity)
    Number of raft layers to print
raftBaseThickness:          decimal, mm
    Height of the first raft layer
raftInterfaceThickness:     decimal, mm
    Height of all raft layers after the first one
raftOutset:                 decimal, mm
    How far beyond the edge of the model does the raft extend
raftModelSpacing:           decimal, mm
    Vertical gap between the top of raft and bottom of model. Positive values leave gap, making model easier to remove, but increase drooping. Negative values squeeze model into raft, bonding them more tightly.
raftDensity:                decimal [0.0,1.0]
    How solid the raft is. 0.0 is no raft. 1.0 is solid slab of plastic. Intermediate values create a grid of approximately this density.

doSupport:                  boolean
    Enable support. Options below are ignored if this is false
supportMargin:              decimal, mm
    Distance between wall of the object and support.
supportDensity:             decimal, [0.0,1.0]
    How solid support is. 0.0 is no support. 1.0 is solidly filled support. Intermediate values create a grid of approximately this density.

bedZOffset:                 decimal, mm
    Distance between first layer of print and the bed. Does not modify what is printed. Use for correcting platform height errors.
layerHeight:                decimal, mm
    Height of each layer

startX:                     decimal, mm
    Assumed start position of gantry
startY:                     decimal, mm
    Assumed start position of gantry
startZ:                     decimal, mm
    Assumed start position of gantry

startGcode:                 string
    Path to start.gcode file. Inserted before model gcode.
endGcode:                   string
    Path to end.gcode file. Inserted after model gcode

doPrintProgress:            boolean
    If true, insert gcode commands to display progress on the printer's LCD.

defaultExtruder:            integer [0,1]
    Which extruder to print with? 0 is right, 1 is left.

extruderProfiles:          list of JSON blobs.
    Configuration data for each extruder. Specifications as follows

firstLayerExtrusionProfile: string
    Name of extrusion profile to use for the first layer
insetsExtrusionProfile:     string
    Name of extrusion profile to use for insets
infillExtrusionProfile:     string
    Name of extrusion profile to use for infill
outlinesExtrusionProfile:   string
    Name of extrusion profile to use for outlines and outermost insets
feedDiameter:               decimal, mm
    diameter of feedstock in use on this extruder
nozzleDiameter:             decimal, mm
    diameter of nozzle opening on this extruder
retractDistance:            decimal, mm
    Pull filament back up by this much between extrusions
retractRate:                decimal, mm/sec
    Pull filament back up at this speed
retractExtraDistance:       decimal, mm
    Move this much more when pulling back, but not when pushing out

extrusionProfiles:          named JSON blobs.
    Collection of extrusion profiles. Their names are specified in each extruder profile. Extruders and print parts (insets, infill) can share names, or can use different names. Name must appear in this list.
temperature:                decimal, Degrees Celcius
    Temperature to set extruder, NOT YET IMPLEMENTED
feedrate:                   decimal, mm/sec
    Speed to move while extruding
    