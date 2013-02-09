#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "core/firedetector.h"

namespace Ui {
    class Dialog;
}

class Dialog : public QDialog, public StatusListener
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    virtual void onStatusChanged(int status);
    virtual void onPlayFrame(const Result& result);

private:
    Ui::Dialog *ui;
    FireDetector detector;
    double valve;

private slots:
    void on_serverCheckBox_toggled(bool checked);
    void on_fromNet_toggled(bool checked);
    void on_fromFile_toggled(bool checked);
    void on_openRecordDirButton_clicked();
    void on_recordDirCheckBox_toggled(bool checked);
    void on_valveSlider_valueChanged(int value);
    void on_openXmlButton_clicked();
    void on_stopButton_clicked();
    void on_pauseButton_clicked();
    void on_playButton_clicked();
    void on_openFileButton_clicked();
    void on_fromCamera_toggled(bool checked);
};

#endif // DIALOG_H
