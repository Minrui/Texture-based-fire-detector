#ifndef STREAMSERVER_H
#define STREAMSERVER_H

#include <opencv2/opencv.hpp>
#include <pthread.h>

class StreamServer {
private:
    const string    ext;
    const bool      grayscale;
    int             serversock;
    IplImage*       streamFrame;
    pthread_mutex_t streamLock;
    pthread_t       thread;
    void reset();
    bool exception(const char* msg);
    static void* serverThread(StreamServer* server);
public:
    StreamServer(const string& ext, bool grayscale);
    ~StreamServer();
    bool start();
    void stop();
    void stream(const IplImage* image);
};

#endif // STREAMSERVER_H
