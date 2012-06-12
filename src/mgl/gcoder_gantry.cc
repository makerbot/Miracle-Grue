#include "gcoder.h"
#include <cmath>
#include <iostream>
#include <sstream>

namespace mgl {

using std::ostream;
using std::endl;
using std::string;
using std::stringstream;
using libthing::LineSegment2;
using libthing::Vector2;

Gantry::Gantry() : x(MUCH_LARGER_THAN_THE_BUILD_PLATFORM_MM),
		 y(MUCH_LARGER_THAN_THE_BUILD_PLATFORM_MM),
		 z(MUCH_LARGER_THAN_THE_BUILD_PLATFORM_MM),
		 a(0),
         b(0),
		 feed(0),
		 rapidMoveFeedRateXY(5000),
		 rapidMoveFeedRateZ(1400),
		 homingFeedRateZ(100),
		 layerH(.27),
		 xyMaxHoming(true),
		 zMaxHoming(false),
		 extruding(false),
		 scalingFactor(1),
		 ab('A') {}

Scalar Gantry::get_x() const { return x; }
Scalar Gantry::get_y() const { return y; }
Scalar Gantry::get_z() const { return z; }
Scalar Gantry::get_a() const { return a; }
Scalar Gantry::get_b() const { return b; }
Scalar Gantry::get_feed() const { return feed; }
unsigned char Gantry::get_current_extruder() const { return ab; }

void Gantry::set_x(Scalar nx) { x = nx; }
void Gantry::set_y(Scalar ny) { y = ny; }
void Gantry::set_z(Scalar nz) { z = nz; }
void Gantry::set_a(Scalar na) { a = na; }
void Gantry::set_b(Scalar nb) { b = nb; }
void Gantry::set_feed(Scalar nfeed) { feed = nfeed; }
void Gantry::set_current_extruder(unsigned char nab) { ab = nab;}

void Gantry::writeSwitchExtruder(ostream& ss, Extruder &extruder) {
	ss << "( extruder " << extruder.id << " )" << endl;
	ss << "( GSWITCH T" << extruder.id << " )" << endl;
	ss << "( TODO: add offset management to Gantry )" << endl;
	ab = extruder.code;
	ss << endl;
}

Scalar Gantry::segmentVolume(const Extruder &, // extruder,
		const Extrusion &extrusion,
		LineSegment2 &segment) const {
	Scalar cross_area = extrusion.crossSectionArea(layerH);
	Scalar length = segment.length();
	return cross_area * length;
}
Scalar Gantry::volumetricE(	const Extruder &extruder,
		const Extrusion &extrusion,
		Scalar x, Scalar y, Scalar z ) const {
	//There isn't yet a LineSegment3, so for now I'm assuming that only 2d
	//segments get extruded
	LineSegment2 seg(Vector2(this->x, this->y), Vector2(x, y));
	Scalar seg_volume = segmentVolume(extruder, extrusion, seg);

	Scalar feed_cross_area = extruder.feedCrossSectionArea();

	Scalar feed_len = seg_volume / feed_cross_area;

	return feed_len + getCurrentE();
}
/*if extruder and extrusion are null we don't extrude*/
void Gantry::g1(std::ostream &ss,
		const Extruder *extruder, const Extrusion *extrusion,
		Scalar x, Scalar y, Scalar z, Scalar feed,
		const char *comment = NULL) {

	bool doX = true;
	bool doY = true;
	bool doZ = true;
	bool doFeed = true;
	bool doE = false;
	Scalar e = getCurrentE();

	if(!libthing::tequals(this->x, x, SAMESAME_TOL))
	{
		doX = true;
	}
	if(!libthing::tequals(this->y, y, SAMESAME_TOL))
	{
		doY=true;
	}
	if(!libthing::tequals(this->z, z, SAMESAME_TOL))
	{
		doZ=true;
	}

	if(!libthing::tequals(this->feed, feed, SAMESAME_TOL))
	{
		doFeed=true;
	}

	if(extruding && extruder != NULL && extrusion != NULL
	   && extruder->isVolumetric()) {
		doE = true;
		e = volumetricE(*extruder, *extrusion, x, y, z);
	}		

	g1Motion(ss, x, y, z, e, feed, comment,
			 doX, doY, doZ, doE, doFeed);
}

void Gantry::squirt(std::ostream &ss, const Vector2 &lineStart,
		const Extruder &extruder, const Extrusion &extrusion) {
	if (extruder.isVolumetric()) {
		g1Motion(ss, x, y, z,
				 getCurrentE() + extrusion.retractDistance
				 + extrusion.restartExtraDistance, extrusion.retractRate,
				 "squirt", false, false, false, true, true); //only E and F
	}
	else {
		ss << "M108 R" <<  extrusion.squirtFlow << " (squirt)" << endl;
		ss << "M101" << endl;
		g1(ss, extruder, extrusion,
		   lineStart.x, lineStart.y, z, extrusion.squirtFeedrate, NULL);
		ss << "M108 R" << extrusion.flow << " (good to go)" << endl;
	}

	extruding = true;
}

void Gantry::snort(std::ostream &ss, const Vector2 &lineEnd,
		const Extruder &extruder, const Extrusion &extrusion) {
	if (extruder.isVolumetric()) {
		g1Motion(ss, x, y, z,
				 getCurrentE() - extrusion.retractDistance,
				 extrusion.retractRate, "snort",
				 false, false, false, true, true); //only E and F
	}
	else {
		ss << "M108 R" << extrusion.snortFlow << "  (snort)" << endl;
		ss << "M102" << endl;
		g1(ss, extruder, extrusion, lineEnd.x, lineEnd.y, z,
		   extrusion.snortFeedrate, NULL);
		ss << "M103" << endl;
	}

	extruding = false;
}

void Gantry::g1Motion(std::ostream &ss, Scalar x, Scalar y, Scalar z, Scalar e,
		Scalar feed, const char *g1Comment, bool doX,
		bool doY, bool doZ, bool doE, bool doFeed) {

	// not do something is not an option .. under certain conditions
	#ifdef STRONG_CHECKING
	if( !(doX || doY || doZ || doFeed )   )
	{
		stringstream ss;
		ss << "G1 without moving where x=" << x << ", y=" << y << ", z=" << z << ", feed=" << feed ;
		GcoderException mixup(ss.str().c_str());
		throw mixup;
	}
	#endif

	// our moto: don't be bad!
	bool bad = false;
	if(fabs(x) > MUCH_LARGER_THAN_THE_BUILD_PLATFORM_MM) bad = true;
	if(fabs(y) > MUCH_LARGER_THAN_THE_BUILD_PLATFORM_MM) bad = true;
	if(fabs(z) > MUCH_LARGER_THAN_THE_BUILD_PLATFORM_MM) bad = true;
	if(feed <= 0 || feed > 100000) bad = true;

	if(bad)
	{
		stringstream ss;
		ss << "Illegal G1 move where x=" << x << ", y=" << y << ", z=" << z << ", feed=" << feed ;
		GcoderException mixup(ss.str().c_str());
		throw mixup;
	}


	ss << "G1";
	if(doX) ss << " X" << x;
	if(doY) ss << " Y" << y;
	if(doZ) ss << " Z" << z;
	if(doFeed) ss << " F" << feed;
	if(doE) ss << " " << ab << e;
	if(g1Comment) ss << " (" << g1Comment << ")";
	ss << endl;

	// if(feed >= 5000) assert(0);

	// update state machine
	if (doX) this->x = x;
	if (doY) this->y = y;
	if (doZ) this->z = z;
	if (doFeed) this->feed = feed;
	if (doE) setCurrentE(e);
}



}


