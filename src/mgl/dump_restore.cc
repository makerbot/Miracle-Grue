#include "dump_restore.h"
#include <json/reader.h>
#include <json/writer.h>

using namespace std;
using namespace libthing;

namespace mgl {

void dumpPoint(const Point2Type &point, Json::Value &root) {
    root["type"] = "Point";
    root["x"] = point.x;
    root["y"] = point.y;
}

void dumpLoop(const Loop &loop, Json::Value &root) {
    root["type"] = "Loop";

    Json::Value points;

    for (Loop::const_finite_cw_iterator pn = loop.clockwiseFinite();
         pn != loop.clockwiseEnd(); ++pn) {
        Json::Value point;
        dumpPoint(pn->getPoint(), point);
        points.append(point);
    }

    root["points"] = points;
}

void dumpLoopList(const LoopList &loops, Json::Value &root) { 
    root["type"] = "LoopList";

    Json::Value loopsval;

    for (LoopList::const_iterator loop = loops.begin();
         loop != loops.end(); ++loop) {
        Json::Value loopval;

        dumpLoop(*loop, loopval);
        loopsval.append(loopval);
    }

    root["loops"] = loopsval;
}

void dumpLine(const Segment2Type& line, Json::Value& root) {
    root["type"] = "Line";
    
    Json::Value points, a, b;
    dumpPoint(line.a, a);
    dumpPoint(line.b, b);
    points.append(a);
    points.append(b);
    
    root["points"] = points;
}

void restorePoint(const Json::Value &root, Point2Type &point) {
    assert(root["type"] == string("Point"));

    point.x = root["x"].asDouble();
    point.y = root["y"].asDouble();
}

void restoreLoop(const Json::Value &root, Loop &loop) {
    assert(root["type"] == string("Loop"));

    loop.clear();
    const Json::Value &points = root["points"];

    for (Json::Value::const_iterator pointval = points.begin();
         pointval != points.end(); ++pointval) {
        Vector2 point;
        restorePoint(*pointval, point);
        loop.insertPointBefore(point, loop.clockwiseEnd());
    }
}

void restoreLoopList(const Json::Value &root, LoopList &looplist) {
    assert(root["type"] == string("LoopList"));

    looplist.clear();
    const Json::Value &loopsval = root["loops"];

    for (Json::Value::const_iterator loopval = loopsval.begin();
         loopval != loopsval.end(); ++loopval) {
        looplist.push_back(Loop());
        Loop &loop = looplist.back();

        restoreLoop(*loopval, loop);
    }
}

}
