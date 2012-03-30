#ifndef SLICINGDIALOG_H
#define SLICINGDIALOG_H

#include <QDialog>

namespace Ui {
    class SlicingDialog;
}

class SlicingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SlicingDialog(QWidget *parent = 0);
    ~SlicingDialog();

private:
    Ui::SlicingDialog *ui;
};

#endif // SLICINGDIALOG_H
