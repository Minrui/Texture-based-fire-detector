#ifndef JSET_H
#define JSET_H

#include <list>
#include "jnode.h"

#define JSET_OR        0
#define JSET_AND       1
#define JSET_OR_PIXEL  2
#define JSET_AND_PIXEL 3

class JSet:public JNode {
private:
    const int         type;
    IplImage*         mask;
    std::list<JNode*> nodes;
protected:
    virtual void judge(const Result* result);
public:
    JSet(int width, int height, const char* name, bool show_info, int type);
    void addNode(JNode* node);
    virtual ~JSet();
};

#endif // JSET_H
