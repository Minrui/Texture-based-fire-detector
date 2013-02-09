#ifndef MDNJNODE_H
#define MDNJNODE_H

#include "jnode.h"

//Morphology de-noising JNode
class MdnJNode:public JNode {
private:
    IplImage*      mask;
    IplConvKernel* structuringElement;
protected:
    virtual void judge(const Result* result);
public:
    MdnJNode(int width, int height, const char* name, bool show_info);
    virtual ~MdnJNode();
};

#endif // MDNJNODE_H
