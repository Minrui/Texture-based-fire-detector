#ifndef GLCMJNODE_H
#define GLCMJNODE_H

#include "jnode.h"

class GlcmJNode:public JNode {
private:
    const int    R; //归一化系数
    const double stable_max;
    IplImage*    grayImage;
    CvMat*       grayMat;
protected:
    virtual void judge(const Result* result);
public:
    GlcmJNode(int width, int height, const char* name, bool show_info, double stable_max);
    virtual ~GlcmJNode();
};

#endif // GLCMJNODE_H
