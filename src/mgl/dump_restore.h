#ifndef DUMP_RESTORE_H
#define DUMP_RESTORE_H

#include <string>
#include <json/value.h>

#include "loop_path.h"
#include "mgl.h"

namespace mgl {

    void dumpPoint(const Point2Type &point, Json::Value &root);
    void dumpLoop(const Loop &loop, Json::Value &root);
    void dumpLoopList(const LoopList &loops, Json::Value &root);
    void dumpLine(const Segment2Type& line, Json::Value& root);
    template <typename COLLECTION>
    void dumpLineList(const COLLECTION& lines, Json::Value& root) {
        root["type"] = "LinesCollection";
        Json::Value linesval;
        for(typename COLLECTION::const_iterator iter = lines.begin(); 
                iter != lines.end(); 
                ++iter) {
            Json::Value lineval;
            dumpLine(*iter, lineval);
            linesval.append(lineval);
        }
        root["lines"] = linesval;
    }

    void restorePoint(const Json::Value &root, Point2Type &point);
    void restoreLoop(const Json::Value &root, Loop &loop);
    void restoreLoopList(const Json::Value &root, LoopList &loops);
}

#endif
