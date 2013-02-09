#include "jnode.h"

JNode::JNode(int width, int height, const char *name, bool show_info):width(width), height(height), name(name), show_info(show_info) {
    next = NULL;
}

JNode::~JNode() {
    if(next != NULL)delete next;
}

void JNode::setNext(JNode *next) {
    this->next = next;
}

void JNode::performJudge(const Result* result) {
    judge(result);
    if(next != NULL) {
        next->performJudge(&this->result);
        this->result = next->getResult();
    }
}

const Result& JNode::getResult() const {
    return result;
}
