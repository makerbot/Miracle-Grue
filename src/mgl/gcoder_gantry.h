/* 
 * File:   gcoder_gantry.h
 * Author: Dev
 *
 * Created on June 12, 2012, 5:50 PM
 */

#ifndef GCODER_GANTRY_H
#define	GCODER_GANTRY_H

#include "mgl.h"

namespace mgl{

static const Scalar SAMESAME_TOL = 1e-6;
static const Scalar MUCH_LARGER_THAN_THE_BUILD_PLATFORM_MM = 100000000;

class Extruder;
class Extrusion;

class GantryConfig
{
public:
	
	GantryConfig();

	Scalar get_start_x() const;
	Scalar get_start_y() const;
	Scalar get_start_z() const;
	Scalar get_start_a() const;
	Scalar get_start_b() const;
	Scalar get_start_feed() const;
	
	void set_start_x(Scalar nx);
	void set_start_y(Scalar ny);
	void set_start_z(Scalar nz);
	void set_start_a(Scalar na);
	void set_start_b(Scalar nb);
	void set_start_feed(Scalar nfeed);
	
	Scalar get_rapid_move_feed_rate_xy() const;
	Scalar get_rapid_move_feed_rate_z() const;
	Scalar get_homing_feed_rate_z() const;
	bool get_xy_max_homing() const;
	bool get_z_max_homing() const;
	Scalar get_layer_h() const;
	Scalar get_scaling_factor() const;
	
	void set_rapid_move_feed_rate_xy(Scalar nxyr);
	void set_rapid_move_feed_rate_z(Scalar nzr);
	void set_homing_feed_rate_z(Scalar nhfrz);
	void set_xy_max_homing(bool mh);
	void set_z_max_homing(bool mh);
	void set_layer_h(Scalar lh);
	void set_scaling_Factor(Scalar sf);
	
	Scalar segmentVolume(const Extruder &extruder, const Extrusion &extrusion,
			libthing::LineSegment2 &segment) const;

private:
	Scalar rapidMoveFeedRateXY;
	Scalar rapidMoveFeedRateZ;
	Scalar homingFeedRateZ;
	Scalar layerH;

	bool xyMaxHoming;
	bool zMaxHoming;
	Scalar scalingFactor;

	Scalar sx, sy, sz, sa, sb, sfeed;	// start positions and feed
};


class Gantry {
public:
	
	Gantry(const GantryConfig& gCfg);
	
	Scalar get_x() const;
	Scalar get_y() const;
	Scalar get_z() const;
	Scalar get_a() const;
	Scalar get_b() const;
	Scalar get_feed() const;
	bool get_extruding() const;
	unsigned char get_current_extruder_index() const;
	
	void set_x(Scalar nx);
	void set_y(Scalar ny);
	void set_z(Scalar nz);
	void set_a(Scalar na);
	void set_b(Scalar nb);
	void set_feed(Scalar nfeed);
	void set_extruding(bool nextruding);
	void set_current_extruder_index(unsigned char nab);

	void init_to_start();
	
	/// writes g1 motion command to gcode output stream
	/// TODO: make this lower level function private.
	void g1Motion(std::ostream &ss,
				  Scalar mx, Scalar my, Scalar mz,
				  Scalar me,
				  Scalar mfeed,
				  const char *comment,
				  bool doX, bool doY, bool doZ,
				  bool doE,
				  bool doFeed);

public:
	void squirt(std::ostream &ss, const libthing::Vector2 &lineStart,
				const Extruder &extruder, const Extrusion &extrusion);
	void snort(std::ostream &ss, const libthing::Vector2 &lineEnd,
			   const Extruder &extruder, const Extrusion &extrusion);
	/// Squirt at the current location
	void squirt(std::ostream& ss, const Extruder& extruder, 
			const Extrusion& extrusion){
		squirt(ss, libthing::Vector2(get_x(), get_y()), 
				extruder, extrusion);
	}
	/// Snort at the current location
	void snort(std::ostream& ss, const Extruder& extruder,
			const Extrusion& extrusion){
		snort(ss, libthing::Vector2(get_x(), get_y()), 
				extruder, extrusion);
	}

    void writeSwitchExtruder(std::ostream& ss, Extruder &extruder);

	/// public method emits a g1 command to the stream,
    /// only writing the parameters that have changed since the last g1.
	void g1(std::ostream &ss,
			const Extruder *extruder,
			const Extrusion *extrusion,
			Scalar gx,
			Scalar gy,
			Scalar gz,
			Scalar gfeed,
			const char *comment);

	/// g1 public overloaded methods to make interface simpler
	void g1(std::ostream &ss,
			Scalar gx,
			Scalar gy,
			Scalar gz,
			Scalar gfeed,
			const char *comment) {
		g1(ss, NULL, NULL, gx, gy, gz, gfeed, comment);
	};

	/// g1 public overloaded methods to make interface simpler
	void g1(std::ostream &ss,
			const Extruder &extruder,
			const Extrusion &extrusion,
			Scalar gx,
			Scalar gy,
			Scalar gz,
			Scalar gfeed,
			const char *comment) {
		g1(ss, &extruder, &extrusion, gx, gy, gz, gfeed, comment);
	};
	
	Scalar volumetricE(const Extruder &extruder, const Extrusion &extrusion,
			Scalar vx, Scalar vy, Scalar vz) const;

	/// get axis value of the current extruder in(mm)
	/// (aka mm of feedstock since the last reset this print)
	Scalar getCurrentE() const;// { if (ab == 'A') return a; else return b; };

	/// set axis value of the current extruder in(mm)
	/// (aka mm of feedstock since the last reset this print)
	void setCurrentE(Scalar e);// { if (ab == 'A') a = e; else b = e; };
	
private:
	
	const GantryConfig& gantryCfg;

	Scalar x,y,z,a,b,feed;     // current position and feed
	unsigned char ab;
	bool extruding;
};

}


#endif	/* GCODER_GANTRY_H */

