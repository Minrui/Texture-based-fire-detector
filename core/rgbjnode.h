#ifndef RGBJNODE_H
#define RGBJNODE_H

#include "jnode.h"

class RgbJNode:public JNode {
private:
    const unsigned char r_avg;
    IplImage*           mask;
    //IplConvKernel*      structuringElement;
protected:
    virtual void judge(const Result* result);
public:
    RgbJNode(int width, int height, const char* name, bool show_info, unsigned char r_avg);
    virtual ~RgbJNode();
};

#endif // RGBJNODE_H
