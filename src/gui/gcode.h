#ifndef GCODE_H
#define GCODE_H

#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <QString>
#include <iostream>

#include "mgl/slicy.h"

using namespace std;

// Object that represents a single parsed line of GCode.
class gcode {
private:
        class gCodeParameter {
        public:
                char code;
                double value;

                gCodeParameter(char code, double value) {
                        this->code = code;
                        this->value = value;
                }
        };

        // These are the letter codes that we understand
        static char codes[];

        // The actual GCode command string
        string command;

        // Parsed out comment
        string comment;

        // The set of parameters in this GCode
        vector<gCodeParameter> parameters;

        // Find any comments, store them, then remove them from the command
        void parseComments();

        // Find any codes, and store them
        void parseCodes();

public:
        gcode(string command);

        string getCommand();
        string getComment();
        bool hasCode(char searchCode);
        double getCodeValue(char searchCode);
};

// stab at representing all of the layers in an object
class layerMap {
private:
    // vector containing all of the layer heights we know about
    vector<float> heights;
public:
    // Record that we've seen a specific z height. If it's already in the list, it is ignored, otherwise it is added.
    void recordHeight(float height);

    // Get the height corresponding to a given layer
    float getLayerHeight(int layer);

    // Test if a height is in a given layer
    bool heightInLayer(int layer, float height);

    // Test if a height is higher than a given layer
    bool heightGreaterThanLayer(int layer, float height);

    // Test if a height is lower than a given later
    bool heightLessThanLayer(int layer, float height);

    // Return the number of layers that we know about
    int size();

    void clear();
};

template <class T>
class minMax {
private:
    T min;
    T max;
    bool initialized;
public:
    minMax() : initialized(false) {}
    void evaluate(T dataPoint) {
        if (!initialized) {
            initialized = true;
            min = dataPoint;
            max = dataPoint;
        }
        else {
            if (dataPoint < min) {
                min = dataPoint;
            }
            if (dataPoint > max) {
                max = dataPoint;
            }
        }
    }
    T getMin () { return min; }
    T getMax () { return max; }
};


enum PointKind {  unknown, travel, shell, perimeter, infill, roofing};

// TODO: Use whatever the equivalent class here should be.
// TODO: This is also unravelling the state machine into individual events- maybe it's overkill? Is there a better model?
struct point {
public:

    PointKind kind;
    int nb;


    // Destination of this instruction
    float x;
    float y;
    float z;

    // true if the toolhead is on during this move
    //bool toolEnabled;

    // Feedrate of this instruction
    float feedrate;
    // Flowrate of this instruction
    float flowrate;

    point(PointKind kind, int nb, float x, float y, float z, float feedrate, float flowrate) :
        kind(kind), nb(nb), x(x), y(y), z(z),  feedrate(feedrate), flowrate(flowrate)
    {
      //  if(kind == travel) cout << "#";
    }


};



// Object that can open a file containing GCode and turn it into a series of lines
class gcodeModel
{

    bool toolEnabled;
public:
    vector<point> points;
    layerMap map;
    unsigned int getMapSize(){return  map.size();}
    unsigned int getPointCount(){return points.size();}
    minMax<float> feedrateBounds;
    minMax<float> flowrateBounds;
    minMax<float> zHeightBounds;

public:
    gcodeModel();

    void loadSliceData(const std::vector<mgl::SliceData> &sliceData);

    void loadGCode(QString filename);
    void exportGCode(QString filename);
    float getModelZCenter();
private:
    void loadGcodeLine(const char* lineStr);

};


#endif // GCODE_H
