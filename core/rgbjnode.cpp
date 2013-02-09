#include "rgbjnode.h"
#include "imagewrapper.h"

//static const int seMat[] = {1, 1, 1, 1, 1, 1, 1, 1, 1,};

RgbJNode::RgbJNode(int width, int height, const char* name, bool show_info, unsigned char r_avg):JNode(width,height,name,show_info), r_avg(r_avg) {
    mask = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
    //structuringElement = cvCreateStructuringElementEx(3, 3, 0, 0, CV_SHAPE_CUSTOM, (int*)seMat);
}

RgbJNode::~RgbJNode() {
    cvDestroyWindow(name.c_str());
    //cvReleaseStructuringElement(&structuringElement);
    cvReleaseImage(&mask);
}

void RgbJNode::judge(const Result* result) {
    Result& myResult = this->result;
    myResult.setImage(result->getImage());

    BgrImage image = (IplImage*)result->getImage();
    BwImage  mask  = this->mask;

    for(int i = 0; i < height; ++i) for(int j = 0; j < width; ++j) {
        BgrPixel &p = image[i][j];
        if(p.r >= r_avg && p.r >= p.g && p.g >= p.b)
            mask[i][j] = 255;
        else
            mask[i][j] = 0;
    }

    //cvMorphologyEx(tempFireRegion, tempFireRegion, NULL, structuringElement, CV_MOP_OPEN);
    //cvMorphologyEx(tempFireRegion, tempFireRegion, NULL, structuringElement, CV_MOP_CLOSE);

    myResult.setMask(this->mask);

    if(show_info) {
        cvShowImage(name.c_str(), this->mask);
    }
}
