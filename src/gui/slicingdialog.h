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

    // void init(const char* config, const char* model);

private slots:
    void on_pushButtonModelFile_2_clicked();
    void on_pushButtonModelFile_clicked();
    void on_pushButtonGo_clicked();

private:
    Ui::SlicingDialog *ui;
};

#endif // SLICINGDIALOG_H
