#include "gcodeviewapplication.h"
#include "mainwindow.h"

GCodeViewApplication::GCodeViewApplication(int argc, char *argv[]) :
    QApplication(argc, argv)
{
    setOrganizationName("MakerBot Industries");
    setApplicationName("Toolpath Helper");
}


void GCodeViewApplication::LoadFile(QString fileName) {
    // TODO: Check if this file has been loaded before attempting to load it again.

    MainWindow *targetWindow = NULL;

    // First, check to see if we have an empty window (this should only happen at start?)
    // If we do, just load the file into that one.
    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
        if (mainWin) {
            if (!(mainWin->hasFile())) {
                targetWindow = mainWin;
            }
        }
    }

    // If we weren't successful, just create a new window.
    if (targetWindow == NULL) {
        targetWindow = new MainWindow();
    }
    targetWindow->show();
    targetWindow->loadFile(fileName);

    // Update the window menus across the app.
    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
        if (mainWin)
            mainWin->updateWindowMenu();
    }
}
