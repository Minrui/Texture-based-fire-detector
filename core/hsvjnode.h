#ifndef HSVJNODE_H
#define HSVJNODE_H

#include <list>

#include "jnode.h"

struct HsvSample;

class HsvJNode:public JNode {
private:
    //const unsigned char r_avg;
    //IplConvKernel*      structuringElement;
    IplImage*            hsvImage;
    IplImage*            masks[2];
    std::list<HsvSample> samples;
protected:
    virtual void judge(const Result* result);
public:
    HsvJNode(int width, int height, const char* name, bool show_info);
    void addSample(int h_min, int h_max, unsigned char s_min, unsigned char s_max, unsigned char v_min, unsigned char v_max);
    virtual ~HsvJNode();
};

#endif // HSVJNODE_H
