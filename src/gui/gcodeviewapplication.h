#ifndef GCODEVIEWAPPLICATION_H
#define GCODEVIEWAPPLICATION_H

#include <QApplication>

class GCodeViewApplication : public QApplication
{
public:
    GCodeViewApplication(int argc, char *argv[]);

    // Load a file into the correct place (a new window if it's not already open, etc)
    static void LoadFile(QString fileName);
};

#endif // GCODEVIEWAPPLICATION_H
