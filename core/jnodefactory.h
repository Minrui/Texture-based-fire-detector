#ifndef JNODEFACTORY_H
#define JNODEFACTORY_H

#include "jnode.h"
#include "tinyxml/tinyxml.h"

class JNodeFactory {
private:
    const int width;
    const int height;
    JNode* createJNode(const TiXmlElement &element);
public:
    JNodeFactory(int width, int height);
    JNode* createFromXml(const char* filename);
};

#endif // JNODEFACTORY_H
