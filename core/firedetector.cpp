#include "firedetector.h"
#include "jnodefactory.h"
#include <fstream>

#define STATUS_LOCK pthread_mutex_lock(&statusLock);
#define STATUS_UNLOCK pthread_mutex_unlock(&statusLock);

class MockStateListener:public StatusListener {
public:
    virtual void onStatusChanged(int status) {status = status;}
};
static MockStateListener mockStateListener;

FireDetector::FireDetector() {
    status = FD_STATUS_STOP;
    capture = NULL;
    jnode = NULL;
    server = NULL;
    enableNetCamera = false;
    netcamera = NULL;
    statusListener = &mockStateListener;
    pthread_mutex_init(&statusLock, NULL);
}

FireDetector::~FireDetector() {
    if(status != FD_STATUS_STOP) stop();
}

bool FireDetector::openCapture(int index) {
    STATUS_LOCK;
    assert(status == FD_STATUS_STOP);
    releaseCapture();
    capture = cvCaptureFromCAM(index);
    bool ret = capture != NULL;
    STATUS_UNLOCK;
    return ret;
}

bool FireDetector::openCapture(const char *filename) {
    STATUS_LOCK;
    assert(status == FD_STATUS_STOP);
    releaseCapture();
    capture = cvCaptureFromFile(filename);
    bool ret = capture != NULL;
    STATUS_UNLOCK;
    return ret;
}

bool FireDetector::openCaptureIp(const char* ip) {
    STATUS_LOCK;
    assert(status == FD_STATUS_STOP);
    releaseCapture();
    netcamera = NetCamera::open(ip);
    enableNetCamera = netcamera != NULL;
    STATUS_UNLOCK;
    return enableNetCamera;
}

bool FireDetector::loadJNodeFromXml(const char *filename) {
    releaseJNode();
    JNodeFactory jnf(getCaptureWidth(), getCaptureHeight());
    return (jnode = jnf.createFromXml(filename)) != NULL;
}

void FireDetector::setServer(bool enable, const string &ext, bool gray) {
    releaseServer();
    if(enable) {
        server = new StreamServer(ext, gray);
    }
}

void FireDetector::setRecordDir(const char* recordDir) {
    if(recordDir == NULL)
        enableRecord = false;
    else {
        this->recordDir = recordDir;
        enableRecord = true;
    }
}

void FireDetector::releaseCapture() {
    if(enableNetCamera) {
        delete netcamera;
        netcamera = NULL;
        enableNetCamera = false;
    } else if(capture != NULL) {
        cvReleaseCapture(&capture);
        capture = NULL;
    }
}

void FireDetector::releaseJNode() {
    if(jnode != NULL) {
        delete jnode;
        jnode = NULL;
    }
}

void FireDetector::releaseServer() {
    if(server != NULL) {
        server->stop();
        delete server;
        server = NULL;
    }
}

CvVideoWriter* openVideoWriter(const char* recordDir, int width, int height) {
    string baseDir = recordDir, fullPath;
    if(baseDir == "")baseDir = ".";
    if(baseDir[baseDir.length()-1] != '/' && baseDir[baseDir.length()-1] != '\\') baseDir += '/';
    for(int idx = 1;;++idx) {
        stringstream ss;
        ss << baseDir << idx << ".avi";
        ifstream ifile(ss.str().c_str());
        if(!ifile){fullPath = ss.str(); break;}
    }
    try {
        return cvCreateVideoWriter(fullPath.c_str(), CV_FOURCC('M','J','P','G'), 15, cvSize(width,height));
    }catch(...){
        return NULL;
    }
}

void FireDetector::play() {
    STATUS_LOCK;
    assert(status == FD_STATUS_STOP || status == FD_STATUS_PAUSE);
    statusListener->onStatusChanged(status = FD_STATUS_PLAY);
    STATUS_UNLOCK;
    int width = getCaptureWidth(), height = getCaptureHeight();
    IplImage* image = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
    CvVideoWriter* videoWriter = enableRecord ? openVideoWriter(recordDir.c_str(), width, height) : NULL;
    if(server != NULL)server->start();
    while(cvWaitKey(2)) {
        int status;
        STATUS_LOCK;
        status = this->status;
        STATUS_UNLOCK;
        if(status == FD_STATUS_STOP) {
            cvDestroyWindow("原始视频");
            break;
        }
        if(status == FD_STATUS_PAUSE) break;
        IplImage *imageRo = enableNetCamera?netcamera->queryFrame():cvQueryFrame(capture);
        if(imageRo == NULL) {pause(); break;}
        cvCopy(imageRo, image);
        /*
    if(show_info) {
        CvFont font = cvFont(1.5, 1);
        char buf[256]; sprintf(buf, "Local calm:%f %s", stable, stable < stable_max ? "!FIRE!" : "|---|");
        cvPutText(grayImage, buf, cvPoint(0, height - 8), &font, cvScalarAll(255));
        cvShowImage(name.c_str(), grayImage);
    }
    */

        Result result(image);
        jnode->performJudge(&result);
        CvFont font = cvFont(1.5, 1);
        time_t rawtime;
        time(&rawtime);
        char buf[256]; sprintf(buf, "Valve:%f %s", jnode->getResult().getValue(), ctime(&rawtime));
        cvPutText(image, buf, cvPoint(0, height - 8), &font, cvScalarAll(255));
        cvShowImage("原始视频", image);
        if(server)server->stream(image);
        if(enableRecord && videoWriter)cvWriteFrame(videoWriter, image);
        statusListener->onPlayFrame(jnode->getResult());
    }
    cvReleaseImage(&image);
    if(videoWriter)cvReleaseVideoWriter(&videoWriter);
}

void FireDetector::pause() {
    STATUS_LOCK;
    assert(status == FD_STATUS_PLAY);
    statusListener->onStatusChanged(status = FD_STATUS_PAUSE);
    STATUS_UNLOCK;
}

void FireDetector::stop() {
    STATUS_LOCK;
    assert(status == FD_STATUS_PLAY || status == FD_STATUS_PAUSE);
    cvDestroyWindow("原始视频");
    releaseCapture();
    releaseJNode();
    releaseServer();
    statusListener->onStatusChanged(status = FD_STATUS_STOP);
    STATUS_UNLOCK;
}

void FireDetector::setStatusListener(StatusListener *statusListener) {
    STATUS_LOCK;
    assert(status == FD_STATUS_STOP);
    this->statusListener = statusListener;
    STATUS_UNLOCK;
}

int FireDetector::getStatus() {
    int status;
    STATUS_LOCK;
    status = this->status;
    STATUS_UNLOCK;
    return status;
}

/*
    //打开视频源
    CvCapture *cap = NULL;
    if(ui->fromCamera->isChecked())
        cap = cvCaptureFromCAM(ui->cameraList->currentText().toInt());
    else if(ui->fromFile->isChecked())
        cap = cvCaptureFromFile(ui->filePath->text().toStdString().c_str());
    if(cap == NULL) {
        QMessageBox::critical(this, trUtf8("错误"), trUtf8("打开视频源失败！"));
        return;
    }
    */
