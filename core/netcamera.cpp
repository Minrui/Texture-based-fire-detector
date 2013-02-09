#include "netcamera.h"
#include <cstdio>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

NetCamera::NetCamera(int sock, int width, int height):sock(sock),width(width),height(height) {
    streamFrame = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
    curFrame = NULL;
    pthread_mutex_init(&streamLock, NULL);
    if(pthread_create(&thread, NULL, (void*(*)(void*))clientThread, this)) throw("pthread_create failed.\n");
}

NetCamera::~NetCamera() {
    pthread_cancel(thread);
    pthread_join(thread, NULL);
    cvReleaseImage(&streamFrame);
    cvReleaseImage(&curFrame);
    close(sock);
    pthread_mutex_destroy(&streamLock);
}

IplImage* NetCamera::queryFrame() {
    pthread_mutex_lock(&streamLock);
    cvReleaseImage(&curFrame);
    curFrame = streamFrame ? cvCloneImage(streamFrame) : NULL;
    pthread_mutex_unlock(&streamLock);
    return curFrame;
}

void* NetCamera::clientThread(NetCamera *cam) {
    while(1) {
        try {
            /* get buf size */
            int buf_size;char bufdata[sizeof(buf_size)];
            for (int i = 0, bytes; i < sizeof(buf_size); i += bytes)
                if ((bytes = recv(cam->sock, bufdata + i, sizeof(buf_size) - i, 0)) == -1) throw("recv failed\n");
            buf_size = *((int*)bufdata);

            /* get raw data */
            char sockdata[buf_size];
            for (int i = 0, bytes; i < buf_size; i += bytes)
                if ((bytes = recv(cam->sock, sockdata + i, buf_size - i, 0)) == -1) throw("recv failed\n");

            vector<uchar> buf(sockdata, sockdata+buf_size);

            pthread_mutex_lock(&cam->streamLock);
            cvReleaseImage(&cam->streamFrame);
            cam->streamFrame = cvCloneImage(&IplImage(cv::imdecode(cv::Mat(buf), -1)));
            pthread_mutex_unlock(&cam->streamLock);

            pthread_testcancel();
        }catch(const char* err_msg) {
            pthread_mutex_lock(&cam->streamLock);
            fprintf(stderr, err_msg);
            cvReleaseImage(&cam->streamFrame);
            cam->streamFrame = NULL;
            pthread_mutex_unlock(&cam->streamLock);
            pthread_exit(NULL);
        }
    }
}

NetCamera* NetCamera::open(const char *ip) {
    int sock = -1, width, height;
    try {
        /* create socket */
        if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) throw("socket() failed.\n");

        /* setup server parameters & connect to server*/
        struct 	sockaddr_in server;
        memset(&server, 0, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = inet_addr(ip/*SERVER_IP*/);
        server.sin_port = htons(9999/*PORT*/);
        if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) throw("connect() failed.\n");

        /* get inits */
        char bufdata[sizeof(width)+sizeof(height)];
        for (int i = 0, bytes; i < sizeof(bufdata); i += bytes)
            if ((bytes = recv(sock, bufdata + i, sizeof(bufdata) - i, 0)) == -1) throw("recv failed.\n");
        width = ((int*)bufdata)[0], height = ((int*)bufdata)[1];
        printf("Inits with width:%d height:%d\n", width, height);
    }catch(const char* err_msg){
        fprintf(stderr, err_msg);
        close(sock);
        return NULL;
    }
    return new NetCamera(sock, width, height);
}
