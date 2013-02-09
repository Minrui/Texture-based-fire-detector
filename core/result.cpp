#include "result.h"
#include "imagewrapper.h"

Result::Result() {
    image = NULL;
    mask = NULL;
    value = 0;
}

Result::Result(const IplImage* image) {
    setImage(image);
    setValue(0);
}

Result::Result(const IplImage* image, double value) {
    setImage(image);
    setValue(value);
}

Result::Result(const IplImage* image, const IplImage* mask) {
    setImage(image);
    setMask(mask);
}

void Result::setImage(const IplImage* image) {
    this->image = image;
}

void Result::setValue(double value) {
    mask = NULL;
    type = RESULT_TYPE_VALUE;
    this->value = value;
}

void Result::setMask(const IplImage* mask) {
    type = RESULT_TYPE_IMAGE;
    this->mask = mask;

    BwImage img = (IplImage*)mask;
    int hit = 0, total = 0;
    for(int i = 0; i < img.getHeight(); ++i)
        for(int j = 0; j < img.getWidth(); ++j) {
            ++total;
            if(img[i][j] != 0) ++hit;
        }
    value = double(hit)/total;
}

const IplImage* Result::getImage() const {
    return image;
}

const IplImage* Result::getMask() const {
    return mask;
}

double Result::getValue() const {
    return value;
}
