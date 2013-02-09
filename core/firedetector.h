#ifndef FIREDETECTOR_H
#define FIREDETECTOR_H

#include <opencv2/opencv.hpp>
#include <pthread.h>
#include "jnode.h"
#include "streamserver.h"
#include "netcamera.h"

#define FD_STATUS_PLAY  0
#define FD_STATUS_PAUSE 1
#define FD_STATUS_STOP  2

class StatusListener {
public:
    virtual void onStatusChanged(int status) = 0;
    virtual void onPlayFrame(const Result& result) {}
};

class FireDetector {
private:
    int             status;
    pthread_mutex_t statusLock;
    CvCapture*      capture;
    StatusListener* statusListener;
    JNode*          jnode;
    bool            enableRecord;
    string          recordDir;
    StreamServer*   server;
    bool            enableNetCamera;
    NetCamera*      netcamera;

    void releaseCapture();
    void releaseJNode();
    void releaseServer();
    int  getCaptureWidth() {return enableNetCamera?netcamera->getWidth():cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);}
    int  getCaptureHeight() {return enableNetCamera?netcamera->getHeight():cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);}
public:
    FireDetector();
    ~FireDetector();
    void play();
    void stop();
    void pause();
    void setStatusListener(StatusListener* statusListener);
    int  getStatus();
    //Init functions
    bool openCapture(int index); //Init step1
    bool openCapture(const char* filename); //Init step1
    bool openCaptureIp(const char* ip); //Init step1
    bool loadJNodeFromXml(const char* filename); //Init step2
    void setRecordDir(const char* recordDir); //Init step3 NULL means disable recording function
    void setServer(bool enable, const string& ext = ".JPG", bool gray = true); //Init setp4
};

#endif // FIREDETECTOR_H
