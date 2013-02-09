#include "jset.h"

JSet::JSet(int width, int height, const char *name, bool show_info, int type):JNode(width,height,name,show_info), type(type) {
    mask = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
}

void JSet::judge(const Result *result) {
    for(std::list<JNode*>::iterator t = nodes.begin(); t != nodes.end(); ++t) (*t)->performJudge(result);

    Result& myResult = this->result;
    myResult.setImage(result->getImage());

    if(type == JSET_AND_PIXEL) {
        cvSet(mask, cvScalar(255));
        for(std::list<JNode*>::iterator t = nodes.begin(); t != nodes.end(); ++t) cvAnd((*t)->getResult().getMask(), mask, mask);
        myResult.setMask(mask);
    }else if(type == JSET_OR_PIXEL) {
        cvSet(mask, cvScalar(0));
        for(std::list<JNode*>::iterator t = nodes.begin(); t != nodes.end(); ++t) cvOr((*t)->getResult().getMask(), mask, mask);
        myResult.setMask(mask);
    }else if(type == JSET_OR) {
        double value = 0;
        for(std::list<JNode*>::iterator t = nodes.begin(); t != nodes.end(); ++t) value = 1-(1-value)*(1-(*t)->getResult().getValue());
        myResult.setValue(value);
    }else if(type == JSET_AND) {
        double value = 1;
        for(std::list<JNode*>::iterator t = nodes.begin(); t != nodes.end(); ++t) value = value*(*t)->getResult().getValue();
        myResult.setValue(value);
    }

    if(show_info && (type == JSET_AND_PIXEL || type == JSET_OR_PIXEL)) {
        cvShowImage(name.c_str(), myResult.getMask());
    }
}

JSet::~JSet() {
    for(std::list<JNode*>::iterator t = nodes.begin(); t != nodes.end(); ++t) delete *t;
    cvDestroyWindow(name.c_str());
    cvReleaseImage(&mask);
}

void JSet::addNode(JNode* node) {
    nodes.push_back(node);
}
