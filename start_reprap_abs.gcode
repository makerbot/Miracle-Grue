;**** start.gcode for RepRap Mendel
G21 ; set units to mm
G90 ; set positioning to absolute
G28 ; home
G92 X0 Y0 Z0 E0 ; reset X, Y, Z

M140 S110 ;set bed temperature
M104 S220 ;set extruder temperature
M116 ; wait for temp

G1 Z0.2 F100      (Position Height)
G1 E2.0 F20    (Create Anchor)
G92 E0 ; reset E

