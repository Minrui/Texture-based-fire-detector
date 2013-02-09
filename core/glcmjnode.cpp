#include "glcmjnode.h"
#include <opencv2/opencv.hpp>

#define GRAY_LEVEL 256

GlcmJNode::GlcmJNode(int width, int height, const char* name, bool show_info, double stable_max):JNode(width,height,name,show_info), R(2*(width-1)*(height-1)), stable_max(stable_max) {
    grayImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
    grayMat   = cvCreateMat(GRAY_LEVEL, GRAY_LEVEL, CV_64FC1);
}

void GlcmJNode::judge(const Result* result) {
    Result& myResult = this->result;
    myResult.setImage(result->getImage());

    //转换成256灰阶图像
    cvCvtColor(result->getImage(), grayImage, CV_BGR2GRAY);

    //计算灰度共生矩阵
    cvSetZero(grayMat);
    for(int i = 1; i < height; ++i) for(int j = 0; j + 1 < width; ++j) { //(i, j)<-->(i - 1, j + 1), 45度
        int a = cvGetReal2D(grayImage, i, j);
        int b = cvGetReal2D(grayImage, i - 1, j + 1);
        cvmSet(grayMat, a, b, cvmGet(grayMat, a, b) + 1);
        //cvmSet(grayMat, b, a, cvmGet(grayMat, b, a) + 1);
    }

    //灰度共生矩阵归一化
    for(size_t i = 0; i < GRAY_LEVEL; ++i) for(size_t j = 0; j < GRAY_LEVEL; ++j)
        cvmSet(grayMat, i, j, cvmGet(grayMat, i, j) / R);

    //计算能量
    //double energy = 0;
    //for(size_t i = 0; i < GRAY_LEVEL; ++i) for(size_t j = 0; j < GRAY_LEVEL; ++j)
    //    energy += cvmGet(grayMat, i, j) * cvmGet(grayMat, i, j);

    //计算熵
    //double entropy = 0;
    //for(size_t i = 0; i < GRAY_LEVEL; ++i) for(size_t j = 0; j < GRAY_LEVEL; ++j)
    //    if(fabs(cvmGet(grayMat, i, j)) > 0.000001)
    //        entropy += -cvmGet(grayMat, i, j) * log(cvmGet(grayMat, i, j));

    //计算局部平稳性
    double stable = 0;
    for(size_t i = 0; i < GRAY_LEVEL; ++i) for(size_t j = 0; j < GRAY_LEVEL; ++j)
        stable += cvmGet(grayMat, i, j) / (1 + (i - j) * (i - j));
    if(stable < stable_max) myResult.setValue(1); else myResult.setValue(0);

    if(show_info) {
        CvFont font = cvFont(1.5, 1);
        char buf[256]; sprintf(buf, "Local calm:%f %s", stable, stable < stable_max ? "!FIRE!" : "|---|");
        cvPutText(grayImage, buf, cvPoint(0, height - 8), &font, cvScalarAll(255));
        cvShowImage(name.c_str(), grayImage);
    }
}

GlcmJNode::~GlcmJNode() {
    cvDestroyWindow(name.c_str());
    cvReleaseImage(&grayImage);
    cvReleaseMat(&grayMat);
}
