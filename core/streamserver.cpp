#include "streamserver.h"
#include <netinet/in.h>
#include <vector>

StreamServer::StreamServer(const string& ext, bool grayscale):ext(ext),grayscale(grayscale) {
    reset();
    streamFrame = cvCreateImage(cvSize(1, 1), IPL_DEPTH_8U, 1);
    //tmpFrame = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
    pthread_mutex_init(&streamLock, NULL);
}

StreamServer::~StreamServer() {
    stop();
    cvReleaseImage(&streamFrame);
    pthread_mutex_destroy(&streamLock);
}

void StreamServer::reset() {
    if(serversock)close(serversock);
    serversock = 0;
}

bool StreamServer::exception(const char *msg) {
    reset();
    fprintf(stderr, msg);
    return false;
}

void StreamServer::stream(const IplImage *image) {
    pthread_mutex_lock(&streamLock);
    cvReleaseImage(&streamFrame);
    streamFrame = cvCloneImage(image);
    pthread_mutex_unlock(&streamLock);
}

bool StreamServer::start() {
    /* open socket */
    if ((serversock = socket(PF_INET, SOCK_STREAM, 0)) == -1) return exception("socket() failed\n");

    /* enable reuse addr */
    int on = 1;
    if (setsockopt(serversock, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) < 0) return exception("setsockopt() failed\n");

    /* bind the socket */
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(/*PORT*/8888);
    server.sin_addr.s_addr = INADDR_ANY;
    if (bind(serversock, (const sockaddr*)&server, sizeof(server)) == -1) return exception("bind() failed\n");

    /* wait for connection */
    if (listen(serversock, 10) == -1) return exception("listen() failed.\n");

    /* run the streaming server as a separate thread */
    if (pthread_create(&thread, NULL, (void*(*)(void*))serverThread, this)) return exception("pthread_create failed.\n");

    return true;
}

void StreamServer::stop() {
    pthread_cancel(thread);
    pthread_join(thread, NULL);
    reset();
}

void* StreamServer::serverThread(StreamServer* server) {
    vector<uchar> buf;
    while(1) {
        int clientsock = accept(server->serversock, NULL, NULL);
        if(clientsock == -1) {fprintf(stderr, "accept() failed\n"); continue;}
        pthread_cleanup_push((void(*)(void*))close, (void*)clientsock);
        while(1) {
            pthread_mutex_lock(&server->streamLock);
            if(server->grayscale) {
                IplImage* image = server->streamFrame;
                image = cvCreateImage(cvGetSize(server->streamFrame), IPL_DEPTH_8U, 1);
                cvCvtColor(server->streamFrame, image, CV_BGR2GRAY);
                cv::imencode(server->ext, image, buf);
                cvReleaseImage(&image);
            } else cv::imencode(server->ext, server->streamFrame, buf);

            pthread_mutex_unlock(&server->streamLock);
            pthread_testcancel();
            int buf_size = buf.size();
            if(send(clientsock, &buf_size, sizeof(buf_size), 0) != sizeof(buf_size) || send(clientsock, buf.data(), buf_size, 0) != buf_size) {
                fprintf(stderr, "Connection closed.\n");
                break;
            }
        }
        pthread_cleanup_pop(1);
    }
}
