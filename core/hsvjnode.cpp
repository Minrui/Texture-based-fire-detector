#include "hsvjnode.h"
#include "imagewrapper.h"

#define INRANGE(x,min,max) ((min) <= (x) && (x) <= (max))

struct HsvSample {
    int h_min, h_max;
    unsigned char s_min, s_max;
    unsigned char v_min, v_max;
};

/*
#define CUR_MASK (mask[curMaskIdx])
#define NEXT_MASK (mask[(curMaskIdx+1)%2])
#define TOGGLE_MASK (curMaskIdx=(curMaskIdx+1)%2)
*/

HsvJNode::HsvJNode(int width, int height, const char *name, bool show_info):JNode(width,height,name,show_info) {
    hsvImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
    masks[0] = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
    masks[1] = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
}

void HsvJNode::judge(const Result *result) {
    Result& myResult = this->result;
    myResult.setImage(result->getImage());

    cvCvtColor(result->getImage(), hsvImage, CV_BGR2HSV);
    BgrImage hsvImage = this->hsvImage;

    int max_points = -1, curMaskIdx = 1;
    for(std::list<HsvSample>::iterator t = samples.begin(); t != samples.end(); ++t) {
        HsvSample &sample = *t;
        int cur_points = 0, nextMaskIdx = (curMaskIdx+1)%2;
        BwImage nextMask = masks[nextMaskIdx];

        for(int i = 0; i < height; ++i) for(int j = 0; j < width; ++j) {
            BgrPixel &p = hsvImage[i][j];
            int h = p.b * 2;
            unsigned char s = p.g, v = p.r;
            if(INRANGE(h, sample.h_min, sample.h_max) && INRANGE(s, sample.s_min, sample.s_max) && INRANGE(v, sample.v_min, sample.v_max)) {
                ++cur_points;
                nextMask[i][j] = 255;
            } else nextMask[i][j] = 0;
        }

        //cvMorphologyEx(tempFireRegion, tempFireRegion, NULL, structuringElement, CV_MOP_OPEN);
        //cvMorphologyEx(tempFireRegion, tempFireRegion, NULL, structuringElement, CV_MOP_CLOSE);

        //for(size_t i = 0; i < height; ++i) for(size_t j = 0; j < width; ++j) if(fireWrapper[i][j] != 0) ++cur_points;

        if(cur_points > max_points) {
            max_points = cur_points;
            curMaskIdx = nextMaskIdx;
            //if(fireRegion != NULL)cvReleaseImage(&fireRegion);
            //fireRegion = cvCloneImage(tempFireRegion);
        }
    }
/*
    int max_points = -1;

    for(std::list<HsvSample>::iterator t = samples.begin(); t != samples.end(); ++t) {
        HsvSample &sample = *t;
        int cur_points = 0;

        for(size_t i = 0; i < height; ++i) for(size_t j = 0; j < width; ++j) {
            BgrPixel &p = hsvWrapper[i][j];
            int h = p.b * 2;
            unsigned char s = p.g, v = p.r;
            if(INRANGE(h, sample.h_min, sample.h_max) && INRANGE(s, sample.s_min, sample.s_max) && INRANGE(v, sample.v_min, sample.v_max))
                fireWrapper[i][j] = 255;
            else
                fireWrapper[i][j] = 0;
        }

        cvMorphologyEx(tempFireRegion, tempFireRegion, NULL, structuringElement, CV_MOP_OPEN);
        cvMorphologyEx(tempFireRegion, tempFireRegion, NULL, structuringElement, CV_MOP_CLOSE);

        for(size_t i = 0; i < height; ++i) for(size_t j = 0; j < width; ++j) if(fireWrapper[i][j] != 0) ++cur_points;

        if(cur_points > max_points) {
            max_points = cur_points;
            if(fireRegion != NULL)cvReleaseImage(&fireRegion);
            fireRegion = cvCloneImage(tempFireRegion);
        }
    }
*/
    myResult.setMask(masks[curMaskIdx]);

    if(show_info) {
        cvShowImage(name.c_str(), myResult.getMask());
    }
}

HsvJNode::~HsvJNode() {
    cvDestroyWindow(name.c_str());
    cvReleaseImage(&masks[1]);
    cvReleaseImage(&masks[0]);
    cvReleaseImage(&hsvImage);
}

void HsvJNode::addSample(int h_min, int h_max, unsigned char s_min, unsigned char s_max, unsigned char v_min, unsigned char v_max) {
    HsvSample sample;
    sample.h_min = h_min;
    sample.h_max = h_max;
    sample.s_min = s_min;
    sample.s_max = s_max;
    sample.v_min = v_min;
    sample.v_max = v_max;
    samples.push_back(sample);
}
