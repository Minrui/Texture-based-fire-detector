#ifndef NETCAMERA_H
#define NETCAMERA_H

#include <opencv2/opencv.hpp>
#include <pthread.h>

class NetCamera {
private:
    const int       sock;
    const int       width;
    const int       height;
    IplImage*       streamFrame;
    IplImage*       curFrame;
    pthread_mutex_t streamLock;
    pthread_t       thread;
    NetCamera(int sock, int width, int height);
    static void* clientThread(NetCamera* cam);
public:
    ~NetCamera();
    int getWidth() { return width; }
    int getHeight() { return height; }
    IplImage* queryFrame();
    static NetCamera* open(const char* ip);
};

#endif // NETCAMERA_H
