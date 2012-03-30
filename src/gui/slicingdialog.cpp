#include "slicingdialog.h"
#include "ui_slicingdialog.h"

SlicingDialog::SlicingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SlicingDialog)
{
    ui->setupUi(this);
}

SlicingDialog::~SlicingDialog()
{
    delete ui;
}
