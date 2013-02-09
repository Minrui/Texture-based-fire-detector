#ifndef JNODE_H
#define JNODE_H

#include "core/result.h"

class JNode {
private:
    JNode* next;
protected:
    const int    width;
    const int    height;
    const string name;
    const bool   show_info;
    Result       result;
    JNode(int width, int height, const char *name, bool show_info);
    virtual void judge(const Result* result) = 0;
public:
    virtual ~JNode();
    void performJudge(const Result* result);
    void setNext(JNode* next);
    const Result& getResult() const;
};

#endif // JNODE_H
