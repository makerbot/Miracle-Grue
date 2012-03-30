#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gcodeview.h"
#include "gcodeviewapplication.h"

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QFileDialog>
#include <QSettings>

#include <iostream>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    // Build the 'recent files' menu
    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }

    QMenu* recentFilesMenu = ui->menuBar->findChild<QMenu *>("menuRecent_Files");

    if (recentFilesMenu != NULL) {
        for (int i = 0; i < MaxRecentFiles; ++i)
            recentFilesMenu->addAction(recentFileActs[i]);
    }

    // Reload the recent file menu
    updateRecentFileActions();

    // Reload the 'windows' menu
    updateWindowMenu();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionLoad_GCode_triggered()
{
    QString fileName;
    {
        fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                "",//"/home",
                                                    tr("3D Models (*.stl);;GCode (*.gcode)")// tr("GCode (*.gcode)")
                                                    );
    }
    GCodeViewApplication::LoadFile(fileName);
    setCurrentFile(fileName);
}

// TODO: Move this to the app class
void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    setWindowFilePath(curFile);

    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

    settings.setValue("recentFileList", files);

    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
        if (mainWin)
            mainWin->updateRecentFileActions();
    }
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);

    // TODO: Disable 'recent files' menu if there aren't any.
//    separatorAct->setVisible(numRecentFiles > 0);
}

void MainWindow::updateWindowMenu()
{
    QMenu* windowMenu = ui->menuBar->findChild<QMenu *>("menuWindow");

    if (windowMenu) {
        // TODO: Does this cause a memory leak?
        windowMenu->clear();

        // Re-add the windows
        foreach (QWidget *widget, QApplication::topLevelWidgets()) {
            MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
            if (mainWin) {
                QAction* action = new QAction(this);
                action->setText(mainWin->windowTitle());
                windowMenu->addAction(action);
 //               windowMenu->addAction(recentFileActs[i]);
            }
        }
    }
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        GCodeViewApplication::LoadFile(action->data().toString());
}

void MainWindow::loadFile(const QString &fileName) {
    setWindowTitle(strippedName(fileName));

    QAction* closeMenu = ui->menuBar->findChild<QAction *>("actionClose");
    if (closeMenu) {
        closeMenu->setText("Close \"" + fileName + "\"");
    }
    else {
        std::cout << "no menu?" << std::endl;
    }

    // TODO: How to back off here if model load failed? Should we close the window, etc?
    // TODO: Do loading in background task...
    ui->graphicsView->loadModel(fileName);
    ui->LayerHeight->setMaximum(ui->graphicsView->model.getMapSize() );
}

bool MainWindow::hasFile() {
    return ui->graphicsView->hasModel();
}

void MainWindow::on_LayerHeight_sliderMoved(int position)
{
    // TODO: where /should/ this signal go?
    ui->graphicsView->setCurrentLayer(position);

    // display the current layer height.
}


void MainWindow::on_zoomIn_clicked()
{
    ui->graphicsView->zoom(1.1);
}

void MainWindow::on_zoomOut_clicked()
{
    ui->graphicsView->zoom(.9);
}

void MainWindow::on_actionClose_triggered()
{
    // Close this window.
    this->close();

    // TODO: Can the application get a signal when this happens, instead of sending it explicitly here?
}

void MainWindow::on_actionExport_Gcode_File_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Export GCode"), QDir::currentPath(),  tr("GCode File (*.gcode)"));
    ui->graphicsView->exportModel(filename);

}



void MainWindow::on_panLeft_clicked()
{
    ui->graphicsView->panX(1.0);
}

void MainWindow::on_panRight_clicked()
{
    ui->graphicsView->panX(-1.0);
}

void MainWindow::on_panUp_clicked()
{
    ui->graphicsView->panY(-1.0);
}

void MainWindow::on_panDown_clicked()
{
    ui->graphicsView->panY(1.0);
}
