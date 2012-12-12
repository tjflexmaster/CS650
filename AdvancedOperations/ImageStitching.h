#ifndef IMAGESTITCHING_H
#define IMAGESTITCHING_H

#include <QImage>
#include <QList>
#include <QPoint>
#include "tnt/tnt.h"
#include "tnt/tnt_array2d.h"
#include "AdvancedOperations_global.h"

class ADVANCEDOPERATIONSSHARED_EXPORT ImageStitching
{
public:
    ImageStitching();
    ~ImageStitching();
    QImage getStitchedImages(QImage *target, QList<QPoint> target_points, QImage *source, QList<QPoint> source_points);
    QImage getPaddedImage(QImage *image);

private:
    QImage *_target;
    QImage *_source;
    TNT::Array2D<double> _homography;
    TNT::Array2D<double> _h_prime;
    TNT::Array2D<double> _A;
    TNT::Array2D<double> _B;
};

#endif // IMAGESTITCHING_H
