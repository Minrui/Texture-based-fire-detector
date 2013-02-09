#include "jnodefactory.h"

#include "glcmjnode.h"
#include "gaussbgjnode.h"
#include "rgbjnode.h"
#include "hsvjnode.h"
#include "mdnjnode.h"
#include "jset.h"

JNodeFactory::JNodeFactory(int width, int height):width(width), height(height) {
}

JNode* JNodeFactory::createJNode(const TiXmlElement &element) {
    const char* name = element.Attribute("name");
    bool show_info = strcasecmp("false", element.Attribute("show_info")) != 0;
    JNode* jnode = NULL;

    const char* tag = element.Value();
    if(strcmp(tag, "GlcmJNode") == 0) {
        double stable_max;
        if(element.QueryDoubleAttribute("stable_max", &stable_max) != TIXML_SUCCESS)stable_max = 0.2;
        if(!name)name = "灰度共生矩阵算法";
        jnode = new GlcmJNode(width, height, name, show_info, stable_max);
    } else if(strcmp(tag, "GaussBgJNode") == 0) {
        if(!name)name = "高斯混合模型算法";
        jnode = new GaussBgJNode(width, height, name, show_info);
    } else if(strcmp(tag, "RgbJNode") == 0) {
        int r_avg;
        if(element.QueryIntAttribute("r_avg", &r_avg) != TIXML_SUCCESS)r_avg = 70;
        if(!name)name = "Rgb颜色空间算法";
        jnode = new RgbJNode(width, height, name, show_info, r_avg);
    } else if(strcmp(tag, "HsvJNode") == 0) {
        if(!name)name = "Hsv颜色空间算法";
        jnode = new HsvJNode(width, height, name, show_info);
        for(const TiXmlElement *spNode = element.FirstChildElement(); spNode; spNode = spNode->NextSiblingElement()) {
            if(strcmp(spNode->Value(), "sample") != 0)continue;
            int h_min, h_max, s_min, s_max, v_min, v_max;
            if(spNode->QueryIntAttribute("h_min", &h_min) != TIXML_SUCCESS)throw("Read hsv sample error.");
            if(spNode->QueryIntAttribute("h_max", &h_max) != TIXML_SUCCESS)throw("Read hsv sample error.");
            if(spNode->QueryIntAttribute("s_min", &s_min) != TIXML_SUCCESS)throw("Read hsv sample error.");
            if(spNode->QueryIntAttribute("s_max", &s_max) != TIXML_SUCCESS)throw("Read hsv sample error.");
            if(spNode->QueryIntAttribute("v_min", &v_min) != TIXML_SUCCESS)throw("Read hsv sample error.");
            if(spNode->QueryIntAttribute("v_max", &v_max) != TIXML_SUCCESS)throw("Read hsv sample error.");
            ((HsvJNode*)jnode)->addSample(h_min, h_max, s_min, s_max, v_min, v_max);
        }
    } else if(strcmp(tag, "MdnJNode") == 0) {
        if(!name)name = "形态学去噪算法";
        jnode = new MdnJNode(width, height, name, show_info);
    } else if(strcmp(tag, "JSet") == 0) {
        if(!name)name = "JSet集合算法";
        int type = JSET_OR;
        const char* typeStr = element.Attribute("type");
        if(strcasecmp(typeStr, "OR") == 0)type = JSET_OR;
        else if(strcasecmp(typeStr, "AND") == 0)type = JSET_AND;
        else if(strcasecmp(typeStr, "OR_PIXEL") == 0)type = JSET_OR_PIXEL;
        else if(strcasecmp(typeStr, "AND_PIXEL") == 0)type = JSET_AND_PIXEL;
        jnode = new JSet(width, height, name, show_info, type);
        for(const TiXmlElement *node = element.FirstChildElement("nodes")->FirstChildElement(); node; node = node->NextSiblingElement())
            ((JSet*)jnode)->addNode(createJNode(*node));
    }

    if(jnode != NULL) {
        JNode* nextJNode = NULL;
        const TiXmlElement* nextElement = element.FirstChildElement();
        if(nextElement && strcmp("next", nextElement->Value()) == 0) nextJNode = createJNode(*nextElement->FirstChildElement());
        jnode->setNext(nextJNode);
    }

    return jnode;
}

JNode* JNodeFactory::createFromXml(const char *filename) {
    TiXmlDocument doc;
    if(!doc.LoadFile(filename))return NULL;
    return createJNode(*doc.FirstChildElement());
}
