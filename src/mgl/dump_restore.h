#ifndef DUMP_RESTORE_H
#define DUMP_RESTORE_H

#include <string>
#include <json/value.h>

#include "loop_path.h"
#include "Vector2.h"

namespace mgl {

    void dumpPoint(const libthing::Vector2 &point, Json::Value &root);
    void dumpLoop(const Loop &loop, Json::Value &root);
    void dumpLoopList(const LoopList &loops, Json::Value &root);

    void restorePoint(const Json::Value &root, libthing::Vector2 &point);
    void restoreLoop(const Json::Value &root, Loop &loop);
    void restoreLoopList(const Json::Value &root, LoopList &loops);
}

#endif
