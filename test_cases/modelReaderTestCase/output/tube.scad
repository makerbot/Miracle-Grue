/*This helper library was originally created by davidIvann and 
can be found in his Parametric Space Truss http://www.thingiverse.com/thing:10642
It allows you to join two points on a plane and add rounded corners
*/
module tube(x1, y1, z1, x2, y2, z2, diameter, faces, thickness_over_width)
{
	
	length = sqrt( pow(x1 - x2, 2) + pow(y1 - y2, 2) + pow(z1 - z2, 2) );
	alpha = ( (y2 - y1 != 0) ? atan( (z2 - z1) / (y2 - y1) ) : 0 );
	beta = 90 - ( (x2 - x1 != 0) ? atan( (z2 - z1) / (x2 - x1) ) :  0 );
	gamma =  ( (x2 - x1 != 0) ? atan( (y2 - y1) / (x2 - x1) ) : ( (y2 - y1 >= 0) ? 90 : -90 ) ) + ( (x2 - x1 >= 0) ? 0 : -180 );
	// echo(Length = length, Alpha = alpha, Beta = beta, Gamma = gamma);	
	translate([x1, y1, z1])
	rotate([ 0, beta, gamma])
		scale([thickness_over_width,1,1])
			rotate([0,0,90]) cylinder(h = length, r = diameter/2, center = false, $fn = faces );
}

module corner(x, y, z, diameter){
	translate([x, y, z])
	sphere( r = diameter/2 );
}
d = 0.35; // tube diameter
f = 6;
t = 0.6;
tube(0,0,0,1,0,0, d,f,t);