#include "mdnjnode.h"

static const int seMat[] = {1, 1, 1, 1, 1, 1, 1, 1, 1,};

MdnJNode::MdnJNode(int width, int height, const char* name, bool show_info):JNode(width,height,name,show_info) {
    mask = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
    structuringElement = cvCreateStructuringElementEx(3, 3, 0, 0, CV_SHAPE_CUSTOM, (int*)seMat);
}

MdnJNode::~MdnJNode() {
    cvDestroyWindow(name.c_str());
    cvReleaseStructuringElement(&structuringElement);
    cvReleaseImage(&mask);
}

void MdnJNode::judge(const Result* result) {
    Result& myResult = this->result;
    myResult.setImage(result->getImage());

    cvCopy(result->getMask(), mask);

    //cvMorphologyEx(mask, mask, NULL, structuringElement, CV_MOP_OPEN);
    //cvMorphologyEx(mask, mask, NULL, structuringElement, CV_MOP_CLOSE);
    cvMorphologyEx(mask, mask, NULL, structuringElement, CV_MOP_CLOSE);
    cvMorphologyEx(mask, mask, NULL, structuringElement, CV_MOP_OPEN);

    myResult.setMask(mask);

    if(show_info) {
        cvShowImage(name.c_str(), mask);
    }
}
