#include "gcoder_gantry.h"
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

Gantry::Gantry(const GantryConfig& gCfg) : gantryCfg(gCfg) {
	set_current_extruder_index('A');
	set_extruding(false);
	init_to_start();
}

Scalar Gantry::get_x() const {
	return x;
}

Scalar Gantry::get_y() const {
	return y;
}

Scalar Gantry::get_z() const {
	return z;
}

Scalar Gantry::get_a() const {
	return a;
}

Scalar Gantry::get_b() const {
	return b;
}

Scalar Gantry::get_feed() const {
	return feed;
}

bool Gantry::get_extruding() const {
	return extruding;
}

unsigned char Gantry::get_current_extruder_code() const {
	return ab;
}

void Gantry::set_x(Scalar nx) {
	x = nx;
}

void Gantry::set_y(Scalar ny) {
	y = ny;
}

void Gantry::set_z(Scalar nz) {
	z = nz;
}

void Gantry::set_a(Scalar na) {
	a = na;
}

void Gantry::set_b(Scalar nb) {
	b = nb;
}

void Gantry::set_feed(Scalar nfeed) {
	feed = nfeed;
}

void Gantry::set_extruding(bool nextruding) {
	extruding = nextruding;
}

void Gantry::set_current_extruder_index(unsigned char nab) {
	ab = nab;
}

void Gantry::init_to_start() {
	set_x(gantryCfg.get_start_x());
	set_y(gantryCfg.get_start_y());
	set_z(gantryCfg.get_start_z());
	set_a(gantryCfg.get_start_a());
	set_b(gantryCfg.get_start_b());
	set_feed(gantryCfg.get_start_feed());
}


/// get axis value of the current extruder in(mm)
/// (aka mm of feedstock since the last reset this print)

Scalar Gantry::getCurrentE() const {
	switch (get_current_extruder_code()) {
	case 'A':
		return get_a();
		break;
	case 'B':
		return get_b();
		break;
	default:
	{
		string msg("Illegal extruder index ");
		msg.push_back(get_current_extruder_code());
		throw GcoderException(msg.c_str());
		return 0;
		break;
	}
	}
}

void Gantry::setCurrentE(Scalar e) {
	switch (get_current_extruder_code()) {
	case 'A':
		set_a(e);
		break;
	case 'B':
		set_b(e);
		break;
	default:
	{
		string msg("Illegal extruder index ");
		msg.push_back(get_current_extruder_code());
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

Scalar Gantry::volumetricE(const Extruder &extruder,
		const Extrusion &extrusion,
		Scalar vx, Scalar vy, Scalar /*vz*/,
		Scalar h, Scalar w) const {
	//There isn't yet a LineSegment3, so for now I'm assuming that only 2d
	//segments get extruded
	LineSegment2 seg(Vector2(get_x(), get_y()), Vector2(vx, vy));
	Scalar seg_volume = gantryCfg.segmentVolume(extruder, extrusion, seg, h, w);

	Scalar feed_cross_area = extruder.feedCrossSectionArea();

	Scalar feed_len = seg_volume / feed_cross_area;

	return feed_len + getCurrentE();
}

/*if extruder and extrusion are null we don't extrude*/
void Gantry::g1(std::ostream &ss,
		const Extruder *extruder, const Extrusion *extrusion,
		Scalar gx, Scalar gy, Scalar gz, Scalar gfeed,
		Scalar h, Scalar w,
		const char *comment = NULL) {

	bool doX = true;
	bool doY = true;
	bool doZ = true;
	bool doFeed = true;
	bool doE = false;
	Scalar me = getCurrentE();
	
	PointType relativeVector(gx - get_x(), gy - get_y());

	if (!libthing::tequals(get_x(), gx, SAMESAME_TOL)) {
		doX = true;
	}
	if (!libthing::tequals(get_y(), gy, SAMESAME_TOL)) {
		doY = true;
	}
	if (!libthing::tequals(get_z(), gz, SAMESAME_TOL)) {
		doZ = true;
	}
	if (!libthing::tequals(get_feed(), gfeed, SAMESAME_TOL)) {
		doFeed = true;
	}
	if (get_extruding() && extruder && extrusion &&
			extruder->isVolumetric()) {
		doE = true;
		me = volumetricE(*extruder, *extrusion, gx, gy, gz, h, w);
		if(libthing::tequals(me, getCurrentE(), 0.0) || 
				relativeVector.magnitude() <= gantryCfg.get_coarseness())
			return;
	}
	g1Motion(ss, gx, gy, gz, me, gfeed, h, w, comment,
			doX, doY, doZ, doE, doFeed);
}

void Gantry::squirt(std::ostream &ss, const Vector2 &lineStart,
		const Extruder &extruder, const Extrusion &extrusion) {
	if(get_extruding())
		return;
	if (extruder.isVolumetric()) {
		g1Motion(ss, get_x(), get_y(), get_z(),
				getCurrentE() + extrusion.retractDistance
				+ extrusion.restartExtraDistance, extrusion.retractRate,
				FLUID_H, FLUID_W,
				"squirt", false, false, false, true, true); //only E and F
	} else {
		ss << "M108 R" << extrusion.squirtFlow << " (squirt)" << endl;
		ss << "M101" << endl;
		g1(ss, extruder, extrusion,
				lineStart.x, lineStart.y, z, extrusion.squirtFeedrate, 
				FLUID_H, FLUID_W, NULL);
		ss << "M108 R" << extrusion.flow << " (good to go)" << endl;
	}

	set_extruding(true);
}

void Gantry::snort(std::ostream &ss, const Vector2 &lineEnd,
		const Extruder &extruder, const Extrusion &extrusion) {
	if(!get_extruding())
		return;
	if (extruder.isVolumetric()) {
		g1Motion(ss, get_x(), get_y(), get_z(),
				getCurrentE() - extrusion.retractDistance,
				extrusion.retractRate, FLUID_H, FLUID_W, "snort",
				false, false, false, true, true); //only E and F
	} else {
		ss << "M108 R" << extrusion.snortFlow << "  (snort)" << endl;
		ss << "M102" << endl;
		g1(ss, extruder, extrusion, lineEnd.x, lineEnd.y, z,
				extrusion.snortFeedrate, FLUID_H, FLUID_W, NULL);
		ss << "M103" << endl;
	}

	set_extruding(false);
}

void Gantry::g1Motion(std::ostream &ss, Scalar mx, Scalar my, Scalar mz,
		Scalar me, Scalar mfeed, Scalar /*h*/, Scalar /*w*/,
		const char *g1Comment, bool doX,
		bool doY, bool doZ, bool doE, bool doFeed) {

	// not do something is not an option .. under certain conditions
#ifdef STRONG_CHECKING
	if (!(doX || doY || doZ || doFeed)) {
		stringstream ss;
		ss << "G1 without moving where x=" << x << ", y=" << y << ", z=" << z << ", feed=" << feed;
		GcoderException mixup(ss.str().c_str());
		throw mixup;
	}
#endif

	// our moto: don't be bad!
	bool bad = false;
	if (fabs(mx) > MUCH_LARGER_THAN_THE_BUILD_PLATFORM_MM) bad = true;
	if (fabs(my) > MUCH_LARGER_THAN_THE_BUILD_PLATFORM_MM) bad = true;
	if (fabs(mz) > MUCH_LARGER_THAN_THE_BUILD_PLATFORM_MM) bad = true;
	if (mfeed <= 0 || mfeed > 100000) bad = true;

	if (bad) {
		stringstream ss;
		ss << "Illegal G1 move where x=" << mx << ", y=" << my << ", z=" << mz << ", feed=" << mfeed;
		GcoderException mixup(ss.str().c_str());
		throw mixup;
	}

	unsigned char ss_axis =
			(gantryCfg.get_use_e_axis() ? 'E' :
			get_current_extruder_code());

	ss << "G1";
	if (doX) ss << " X" << mx;
	if (doY) ss << " Y" << my;
	if (doZ) ss << " Z" << mz;
	if (doFeed) ss << " F" << mfeed;
	if (doE) ss << " " << ss_axis << me;
	if (g1Comment) ss << " (" << g1Comment << ")";
	ss << endl;

	// if(feed >= 5000) assert(0);

	// update state machine
	if (doX) set_x(mx);
	if (doY) set_y(my);
	if (doZ) set_z(mz);
	if (doFeed) set_feed(mfeed);
	if (doE) setCurrentE(me);
}

GantryConfig::GantryConfig() {
	set_start_x(MUCH_LARGER_THAN_THE_BUILD_PLATFORM_MM);
	set_start_y(MUCH_LARGER_THAN_THE_BUILD_PLATFORM_MM);
	set_start_z(MUCH_LARGER_THAN_THE_BUILD_PLATFORM_MM);
	set_start_a(0);
	set_start_b(0);
	set_start_feed(0);
	set_rapid_move_feed_rate_xy(5000);
	set_rapid_move_feed_rate_z(1400);
	set_homing_feed_rate_z(100);
	set_layer_h(0.27);
	set_xy_max_homing(true);
	set_z_max_homing(false);
	set_scaling_Factor(1.0);
	set_coarseness(0.05);
}

Scalar GantryConfig::get_start_x() const {
	return sx;
}

Scalar GantryConfig::get_start_y() const {
	return sy;
}

Scalar GantryConfig::get_start_z() const {
	return sz;
}

Scalar GantryConfig::get_start_a() const {
	return sa;
}

Scalar GantryConfig::get_start_b() const {
	return sb;
}

Scalar GantryConfig::get_start_feed() const {
	return sfeed;
}

void GantryConfig::set_start_x(Scalar nx) {
	sx = nx;
}

void GantryConfig::set_start_y(Scalar ny) {
	sy = ny;
}

void GantryConfig::set_start_z(Scalar nz) {
	sz = nz;
}

void GantryConfig::set_start_a(Scalar na) {
	sa = na;
}

void GantryConfig::set_start_b(Scalar nb) {
	sb = nb;
}

void GantryConfig::set_start_feed(Scalar nfeed) {
	sfeed = nfeed;
}

Scalar GantryConfig::get_rapid_move_feed_rate_xy() const {
	return rapidMoveFeedRateXY;
}

Scalar GantryConfig::get_rapid_move_feed_rate_z() const {
	return rapidMoveFeedRateZ;
}

Scalar GantryConfig::get_homing_feed_rate_z() const {
	return homingFeedRateZ;
}

bool GantryConfig::get_xy_max_homing() const {
	return xyMaxHoming;
}

bool GantryConfig::get_z_max_homing() const {
	return zMaxHoming;
}

bool GantryConfig::get_use_e_axis() const {
	return useEaxis;
}

Scalar GantryConfig::get_layer_h() const {
	return layerH;
}

Scalar GantryConfig::get_scaling_factor() const {
	return scalingFactor;
}

Scalar GantryConfig::get_coarseness() const {
	return coarseness;
}

Scalar GantryConfig::segmentVolume(const Extruder &, // extruder,
		const Extrusion &extrusion,
		LineSegment2 &segment, Scalar h, Scalar w) const {
	Scalar cross_area = extrusion.crossSectionArea(h, w);
	Scalar length = segment.length();
	return cross_area * length;
}

void GantryConfig::set_rapid_move_feed_rate_xy(Scalar nxyr) {
	rapidMoveFeedRateXY = nxyr;
}

void GantryConfig::set_rapid_move_feed_rate_z(Scalar nzr) {
	rapidMoveFeedRateZ = nzr;
}

void GantryConfig::set_homing_feed_rate_z(Scalar nhfrz) {
	homingFeedRateZ = nhfrz;
}

void GantryConfig::set_xy_max_homing(bool mh) {
	xyMaxHoming = mh;
}

void GantryConfig::set_z_max_homing(bool mh) {
	zMaxHoming = mh;
}

void GantryConfig::set_use_e_axis(bool uea) {
	useEaxis = uea;
}

void GantryConfig::set_layer_h(Scalar lh) {
	layerH = lh;
}

void GantryConfig::set_scaling_Factor(Scalar sf) {
	scalingFactor = sf;
}

void GantryConfig::set_coarseness(Scalar c) {
	coarseness = c;
}



}


