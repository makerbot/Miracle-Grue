#include <QFileDialog>
#include <QMessageBox>

#include "slicingdialog.h"
#include "ui_slicingdialog.h"


#include "slicingdialog.h"
#include "mgl/configuration.h"
#include "mgl/miracle.h"

using namespace mgl;
using namespace std;

SlicingDialog::SlicingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SlicingDialog)
{
    ui->setupUi(this);
    ui->label_version->setText("Miracle-Grue");
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(0);
}


SlicingDialog::~SlicingDialog()
{
    delete ui;
}

void SlicingDialog::on_pushButtonModelFile_2_clicked()
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Slicer configurations (*.config)") );
    ui->lineEditModelFile_2->setText(fileName);

    //SlicingDialog::configFile = fileName.toStdString();
    //ui->progressBar->setValue(0);
}

void SlicingDialog::on_pushButtonModelFile_clicked()
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("3D model files (*.stl)") );
    ui->lineEditModelFile->setText(fileName);

    //SlicingDialog::modelFile = fileName.toStdString();
}

void SlicingDialog::on_pushButtonGo_clicked()
{

    class Progress : public ProgressBar
    {
        QLabel &taskLabel; QProgressBar& progress;
    public:
        Progress(QLabel *taskLabelp, QProgressBar* progressp)
            :taskLabel(*taskLabelp), progress(*progressp)
        {

        }

        void onTick(const char* taskName, unsigned int count, unsigned int tick)
        {
            if(tick==0)
            {
                taskLabel.setText(taskName);
                progress.setMinimum(0);
                progress.setMaximum(count);

            }
            progress.setValue(tick+1);
            // cout << taskName << " tick: " << tick << "/" << count << endl;
        }
    };

    try
    {
        cout << "Output file: ";
        MyComputer computer;

        string configFileName = ui->lineEditModelFile_2->text().toStdString();
        string filename = ui->lineEditModelFile->text().toStdString();

        string gcodeFile = computer.fileSystem.ChangeExtension(computer.fileSystem.ExtractFilename(filename.c_str()).c_str(), ".gcode" );
        cout << gcodeFile << endl;
        string scadFile = computer.fileSystem.ChangeExtension(computer.fileSystem.ExtractFilename(filename.c_str()).c_str(), ".scad" );
        cout << scadFile << endl;



        //configFileName += "/miracle.config";
        mgl::Configuration config;
        cout << "loading config: " << configFileName << endl;
        config.readFromFile(configFileName.c_str());

        GCoder gcoder;
        loadGCoderData(config, gcoder);
        Slicer slicerCfg;
        loadSlicerData(config, slicerCfg);

        cout << "slicing" << endl;
        std::vector<mgl::SliceData> slices;

        Progress progress(ui->label_task, ui->progressBar);
        miracleGrue(gcoder, slicerCfg, filename.c_str(), NULL, gcodeFile.c_str(), -1, -1, slices, &progress);


    }
    catch(mgl::Exception &mixup)
    {
        cout << "ERROR: " << mixup.error << endl;
        QMessageBox box;
        box.setText(mixup.error.c_str());
        box.show();

    }
}
