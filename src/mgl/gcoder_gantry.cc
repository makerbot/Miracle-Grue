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

Gantry::Gantry() : rapidMoveFeedRateXY(5000),
		rapidMoveFeedRateZ(1400),
		homingFeedRateZ(100),
		layerH(.27),
		xyMaxHoming(true),
		zMaxHoming(false),
		scalingFactor(1) {
	set_start_x(MUCH_LARGER_THAN_THE_BUILD_PLATFORM_MM);
	set_start_y(MUCH_LARGER_THAN_THE_BUILD_PLATFORM_MM);
	set_start_z(MUCH_LARGER_THAN_THE_BUILD_PLATFORM_MM);
	set_start_a(0);
	set_start_b(0);
	set_start_feed(0);
	set_current_extruder_index('A');
	init_to_start();
	set_extruding(false);
}

Scalar Gantry::get_x() const { return x; }
Scalar Gantry::get_y() const { return y; }
Scalar Gantry::get_z() const { return z; }
Scalar Gantry::get_a() const { return a; }
Scalar Gantry::get_b() const { return b; }
Scalar Gantry::get_feed() const { return feed; }
bool Gantry::get_extruding() const { return extruding; }
unsigned char Gantry::get_current_extruder_index() const { return ab; }

void Gantry::set_x(Scalar nx) { x = nx; }
void Gantry::set_y(Scalar ny) { y = ny; }
void Gantry::set_z(Scalar nz) { z = nz; }
void Gantry::set_a(Scalar na) { a = na; }
void Gantry::set_b(Scalar nb) { b = nb; }
void Gantry::set_feed(Scalar nfeed) { feed = nfeed; }
void Gantry::set_extruding(bool nextruding) { extruding = nextruding; }
void Gantry::set_current_extruder_index(unsigned char nab) { ab = nab;}

Scalar Gantry::get_start_x() const { return sx; }
Scalar Gantry::get_start_y() const { return sy; }
Scalar Gantry::get_start_z() const { return sz; }
Scalar Gantry::get_start_a() const { return sa; }
Scalar Gantry::get_start_b() const { return sb; }
Scalar Gantry::get_start_feed() const { return sfeed; }

void Gantry::set_start_x(Scalar nx) { sx = nx; }
void Gantry::set_start_y(Scalar ny) { sx = ny; }
void Gantry::set_start_z(Scalar nz) { sx = nz; }
void Gantry::set_start_a(Scalar na) { sx = na; }
void Gantry::set_start_b(Scalar nb) { sx = nb; }
void Gantry::set_start_feed(Scalar nfeed) { sfeed = nfeed; }

void Gantry::init_to_start(){
	set_x(get_start_x());
	set_y(get_start_y());
	set_z(get_start_z());
	set_a(get_start_a());
	set_b(get_start_b());
	set_feed(get_start_feed());
}

Scalar Gantry::get_rapid_move_feed_rate_xy() const { 
	return rapidMoveFeedRateXY;
}
Scalar Gantry::get_rapid_move_feed_rate_z() const {
	return rapidMoveFeedRateZ;
}
Scalar Gantry::get_homing_feed_rate_z() const{
	return homingFeedRateZ;
}
bool Gantry::get_xy_max_homing() const{
	return xyMaxHoming;
}
bool Gantry::get_z_max_homing() const{
	return zMaxHoming;
}
Scalar Gantry::get_layer_h() const{
	return layerH;
}

void Gantry::set_rapid_move_feed_rate_xy(Scalar nxyr){
	rapidMoveFeedRateXY = nxyr;
}
void Gantry::set_rapid_move_feed_rate_z(Scalar nzr){
	rapidMoveFeedRateZ = nzr;
}
void Gantry::set_homing_feed_rate_z(Scalar nhfrz){
	homingFeedRateZ = nhfrz;
}
void Gantry::set_xy_max_homing(bool mh){
	xyMaxHoming = mh;
}
void Gantry::set_z_max_homing(bool mh){
	zMaxHoming = mh;
}
void Gantry::set_layer_h(Scalar lh){
	layerH = lh;
}

/// get axis value of the current extruder in(mm)
/// (aka mm of feedstock since the last reset this print)
Scalar Gantry::getCurrentE() const {
	switch(get_current_extruder_index()){
	case 'A':
		return get_a();
		break;
	case 'B':
		return get_b();
		break;
	default:
	{
		string msg("Illegal extruder index ");
		msg.push_back(get_current_extruder_index());
		throw GcoderException(msg.c_str());
		return 0;
		break;
	}
	}
}
void Gantry::setCurrentE(Scalar e) {
	switch(get_current_extruder_index()){
	case 'A':
		set_a(e);
		break;
	case 'B':
		set_b(e);
		break;
	default:
	{
		string msg("Illegal extruder index ");
		msg.push_back(get_current_extruder_index());
		throw GcoderException(msg.c_str());
		break;
	}
	}
}
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
		Scalar vx, Scalar vy, Scalar vz ) const {
	//There isn't yet a LineSegment3, so for now I'm assuming that only 2d
	//segments get extruded
	LineSegment2 seg(Vector2(get_x(), get_y()), Vector2(vx, vy));
	Scalar seg_volume = segmentVolume(extruder, extrusion, seg);

	Scalar feed_cross_area = extruder.feedCrossSectionArea();

	Scalar feed_len = seg_volume / feed_cross_area;

	return feed_len + getCurrentE();
}
/*if extruder and extrusion are null we don't extrude*/
void Gantry::g1(std::ostream &ss,
		const Extruder *extruder, const Extrusion *extrusion,
		Scalar gx, Scalar gy, Scalar gz, Scalar gfeed,
		const char *comment = NULL) {

	bool doX = true;
	bool doY = true;
	bool doZ = true;
	bool doFeed = true;
	bool doE = false;
	Scalar e = getCurrentE();

	if(!libthing::tequals(get_x(), gx, SAMESAME_TOL))
	{
		doX = true;
	}
	if(!libthing::tequals(get_y(), gy, SAMESAME_TOL))
	{
		doY=true;
	}
	if(!libthing::tequals(get_z(), gz, SAMESAME_TOL))
	{
		doZ=true;
	}

	if(!libthing::tequals(get_feed(), gfeed, SAMESAME_TOL))
	{
		doFeed=true;
	}

	if(get_extruding() && extruder != NULL && extrusion != NULL
	   && extruder->isVolumetric()) {
		doE = true;
		e = volumetricE(*extruder, *extrusion, gx, gy, gz);
	}		

	g1Motion(ss, gx, gy, gz, e, gfeed, comment,
			 doX, doY, doZ, doE, doFeed);
}
void Gantry::squirt(std::ostream &ss, const Vector2 &lineStart,
		const Extruder &extruder, const Extrusion &extrusion) {
	if (extruder.isVolumetric()) {
		g1Motion(ss, get_x(), get_y(), get_z(),
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

	set_extruding(true);
}
void Gantry::snort(std::ostream &ss, const Vector2 &lineEnd,
		const Extruder &extruder, const Extrusion &extrusion) {
	if (extruder.isVolumetric()) {
		g1Motion(ss, get_x(), get_y(), get_z(),
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

	set_extruding(false);
}
void Gantry::g1Motion(std::ostream &ss, Scalar mx, Scalar my, Scalar mz, 
		Scalar e, Scalar mfeed, const char *g1Comment, bool doX,
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
	if(fabs(mx) > MUCH_LARGER_THAN_THE_BUILD_PLATFORM_MM) bad = true;
	if(fabs(my) > MUCH_LARGER_THAN_THE_BUILD_PLATFORM_MM) bad = true;
	if(fabs(mz) > MUCH_LARGER_THAN_THE_BUILD_PLATFORM_MM) bad = true;
	if(mfeed <= 0 || mfeed > 100000) bad = true;

	if(bad)
	{
		stringstream ss;
		ss << "Illegal G1 move where x=" << mx << ", y=" << my << ", z=" << mz << ", feed=" << mfeed ;
		GcoderException mixup(ss.str().c_str());
		throw mixup;
	}


	ss << "G1";
	if(doX) ss << " X" << mx;
	if(doY) ss << " Y" << my;
	if(doZ) ss << " Z" << mz;
	if(doFeed) ss << " F" << mfeed;
	if(doE) ss << " " << get_current_extruder_index() << e;
	if(g1Comment) ss << " (" << g1Comment << ")";
	ss << endl;

	// if(feed >= 5000) assert(0);

	// update state machine
	if (doX) set_x(mx);
	if (doY) set_y(my);
	if (doZ) set_z(mz);
	if (doFeed) set_feed(mfeed);
	if (doE) setCurrentE(e);
}



}


