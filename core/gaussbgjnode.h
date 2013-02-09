#ifndef GAUSSBGJNODE_H
#define GAUSSBGJNODE_H

#include "jnode.h"

class GaussBgJNode:public JNode {
private:
    CvBGStatModel* bgModel;
    IplImage*      infoImage;
protected:
    virtual void judge(const Result* result);
public:
    GaussBgJNode(int width, int height, const char* name, bool show_info);
    virtual ~GaussBgJNode();
};

#endif // GAUSSBGJNODE_H
