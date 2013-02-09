#include "dialog.h"
#include "ui_dialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QtNetwork/QNetworkInterface>
#include <opencv2/opencv.hpp>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    QList<QHostAddress> adds = QNetworkInterface::allAddresses();
    for(int i = 0; i < adds.length(); ++i)ui->ipComboBox->addItem(adds.at(i).toString());
    detector.setStatusListener(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_fromCamera_toggled(bool checked) {
    if(checked) {
        ui->cameraList->clear();
        cv::VideoCapture cap;
        for(int i = 0; cap.open(i); ++i)
            ui->cameraList->addItem(QString::number(i));
    }
    ui->cameraList->setEnabled(checked);
}

void Dialog::on_openFileButton_clicked() {
    ui->filePath->setText(QFileDialog::getOpenFileName());
}

void Dialog::on_playButton_clicked() {
    if(detector.getStatus() == FD_STATUS_STOP) {
        //打开视频源
        bool openCap = false;
        if(ui->fromCamera->isChecked())
            openCap = detector.openCapture(ui->cameraList->currentText().toInt());
        else if(ui->fromFile->isChecked())
            openCap = detector.openCapture(ui->filePath->text().toUtf8().constData());
        else if(ui->fromNet->isChecked())
            openCap = detector.openCaptureIp(ui->netIp->text().toUtf8().constData());
        if(!openCap) {
            QMessageBox::critical(this, trUtf8("错误"), trUtf8("打开视频源失败！"));
            return;
        }

        //载入JNode Xml文件
        bool loadXml = false;
        loadXml = detector.loadJNodeFromXml((const char*)ui->xmlPath->text().toUtf8().constData());
        if(!loadXml) {
            QMessageBox::critical(this, trUtf8("错误"), trUtf8("载入JNode Xml文件失败！"));
            return;
        }
    }
    valve = ui->valve->text().toDouble();
    detector.setRecordDir(ui->recordDirCheckBox->isChecked() ? ui->recordDir->text().toUtf8().constData() : NULL);
    detector.setServer(ui->serverCheckBox->isChecked(), ui->serverExt->currentText().toUtf8().constData(), ui->serverGray->isChecked());
    detector.play();
}

void Dialog::onStatusChanged(int status) {
    switch(status) {
    case FD_STATUS_PLAY:
        ui->playButton->setEnabled(false);
        ui->pauseButton->setEnabled(true);
        ui->stopButton->setEnabled(true);
        break;
    case FD_STATUS_PAUSE:
        ui->playButton->setEnabled(true);
        ui->pauseButton->setEnabled(false);
        ui->stopButton->setEnabled(true);
        ui->label_status->setText(trUtf8("暂停"));
        break;
    case FD_STATUS_STOP:
        ui->playButton->setEnabled(true);
        ui->pauseButton->setEnabled(false);
        ui->stopButton->setEnabled(false);
        ui->label_status->setText(trUtf8("停止"));
        break;
    }
}

void Dialog::onPlayFrame(const Result& result) {
    if(result.getValue() >= valve)
        ui->label_status->setText(trUtf8("发现火灾!"));
    else
        ui->label_status->setText(trUtf8("播放中"));
}

void Dialog::on_pauseButton_clicked() {
    detector.pause();
}

void Dialog::on_stopButton_clicked() {
    detector.stop();
}

void Dialog::on_openXmlButton_clicked() {
    ui->xmlPath->setText(QFileDialog::getOpenFileName());
}

void Dialog::on_valveSlider_valueChanged(int value) {
    ui->valve->setText(QString::number((double)value/ui->valveSlider->maximum(), 'g', 2));

}

void Dialog::on_recordDirCheckBox_toggled(bool checked) {
    ui->recordDir->setEnabled(checked);
    ui->openRecordDirButton->setEnabled(checked);
}

void Dialog::on_openRecordDirButton_clicked() {
    ui->recordDir->setText(QFileDialog::getExistingDirectory());
}

void Dialog::on_fromFile_toggled(bool checked) {
    ui->filePath->setEnabled(checked);
    ui->openFileButton->setEnabled(checked);
}

void Dialog::on_fromNet_toggled(bool checked) {
    ui->netIp->setEnabled(checked);
}

void Dialog::on_serverCheckBox_toggled(bool checked) {
    ui->label_2->setEnabled(checked);
    ui->serverExt->setEnabled(checked);
    ui->serverGray->setEnabled(checked);
}
