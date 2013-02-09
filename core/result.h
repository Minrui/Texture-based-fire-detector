#ifndef RESULT_H
#define RESULT_H

#include <opencv2/opencv.hpp>

#define RESULT_TYPE_VALUE 0
#define RESULT_TYPE_IMAGE 1

class Result {
private:
    const IplImage* image; //BgrImage
    const IplImage* mask;  //BwImage
    double          value;
    int             type;
public:
    Result();
    Result(const IplImage* image);
    Result(const IplImage* image, double value);
    Result(const IplImage* image, const IplImage* mask);
    void setImage(const IplImage* image);
    void setValue(double value);
    void setMask(const IplImage* mask);
    const IplImage* getImage() const;
    const IplImage* getMask() const;
    double getValue() const;
};

#endif // RESULT_H
