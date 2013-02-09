#include "gaussbgjnode.h"

GaussBgJNode::GaussBgJNode(int width, int height, const char* name, bool show_info):JNode(width,height,name,show_info) {
    infoImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
    cvSetZero(infoImage);
    bgModel = cvCreateGaussianBGModel(infoImage);
}

GaussBgJNode::~GaussBgJNode() {
    cvDestroyWindow(name.c_str());
    cvReleaseBGStatModel(&bgModel);
    cvReleaseImage(&infoImage);
}

void GaussBgJNode::judge(const Result* result) {
    Result& myResult = this->result;
    myResult.setImage(result->getImage());
    cvUpdateBGStatModel((IplImage*)result->getImage(), bgModel);
    myResult.setMask(bgModel->foreground);

    if(show_info) {
        //cvSetZero(infoImage);
        //cvCopy(myResult.getImage(), infoImage, myResult.getMask());
        cvShowImage(name.c_str(), myResult.getMask());
    }
}
