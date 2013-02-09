#ifndef IMAGEWRAPPER_H
#define IMAGEWRAPPER_H

#include <opencv2/opencv.hpp>

template<class T>
class ImageWrapper {
private:
    IplImage *image;
public:
    ImageWrapper(IplImage *image = NULL) {*this = image;}
    void operator= (IplImage *image) {this->image = image;}
    inline T* operator[] (const int rowIndex) {return (T*)(image->imageData + rowIndex * image->widthStep);}
    inline const T* operator[] (const int rowIndex) const {return (T*)(image->imageData + rowIndex * image->widthStep);}

    inline int getWidth() const {
        return image->width;
    }

    inline int getHeight() const {
        return image->height;
    }

    inline const IplImage *getImage() const {
        return image;
    }

    //virtual ~ImageWrapper() {image = NULL;}
};

typedef struct {
    unsigned char b, g, r;
} BgrPixel;

typedef struct {
    float b, g, r;
} BgrFloatPixel;

typedef ImageWrapper<BgrPixel>      BgrImage;
typedef ImageWrapper<BgrFloatPixel> BgrFloatImage;
typedef ImageWrapper<unsigned char> BwImage;
typedef ImageWrapper<float>         BwFloatImage;

#endif // IMAGEWRAPPER_H
