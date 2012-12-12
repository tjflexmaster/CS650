#include "EdgeDetection.h"
#include <QImage>
#include <QPoint>
#include <QPair>
#include <qmath.h>
#include <QInputDialog>
#include "Utility.h"
#include <QDebug>
#include <QHash>
#include <QVector>
#include "Convolution.h"

EdgeDetection::EdgeDetection()
{
}

QImage EdgeDetection::gradientMagnitudeImage(QImage *image)
{
    QImage new_image(image->size(), QImage::Format_ARGB32_Premultiplied);
//    QImage orient_image(image->size(), QImage::Format_ARGB32_Premultiplied);

    QList<QPair<QPoint, QPair<int, int> > > x_y_struct;
    x_y_struct.append( qMakePair(QPoint(-1,-1), QPair<int,int>(-1, 1) ));
    x_y_struct.append( qMakePair(QPoint(-1,0), QPair<int,int>(-2, 0) ));
    x_y_struct.append( qMakePair(QPoint(-1,1), QPair<int,int>(-1,-1) ));
    x_y_struct.append( qMakePair(QPoint(0,-1), QPair<int,int>(0,2) ));
    x_y_struct.append( qMakePair(QPoint(0,0), QPair<int,int>(0,0) ));
    x_y_struct.append( qMakePair(QPoint(0,1), QPair<int,int>(0,-2) ));
    x_y_struct.append( qMakePair(QPoint(1,-1), QPair<int,int>(1,1) ));
    x_y_struct.append( qMakePair(QPoint(1,0), QPair<int,int>(2,0) ));
    x_y_struct.append( qMakePair(QPoint(1,1), QPair<int,int>(1, -1) ));

//    QList<QPair<QPoint, qreal> > orientations;
//    qreal orien_min = 1000;
//    qreal orien_max = 0;

//    int threshold = QInputDialog::getInteger(0, "Threshold", "Threshold value:", -1, -1, 255);

    for( int y=0; y < image->height(); y++ ) {
        for( int x=0; x < image->width(); x++ ) {
            QPoint point(x,y);
            int x_sum = 0;
            int x_rsum = 0;
            int x_gsum = 0;
            int x_bsum = 0;
            int y_sum = 0;
            int y_rsum = 0;
            int y_gsum = 0;
            int y_bsum = 0;
            //Perform Convolution on X
            for (int i=0; i < x_y_struct.size(); i++) {
                QPoint offset = x_y_struct.at(i).first;
                QPair<int,int> multiplier = x_y_struct.at(i).second;
                //Only convolve real pixels
                int pixel_x = offset.x() + x;
                int pixel_y = offset.y() + y;
                QRgb color;
                if ( pixel_x < 0 ||
                     pixel_x >= image->width() ||
                     pixel_y < 0 ||
                     pixel_y >= image->height() ) {
                    color = qRgb(128, 128, 128);
                } else {
                    color = image->pixel(QPoint(pixel_x, pixel_y));
                }

                int gray_val = qGray(color);
                x_sum += gray_val * multiplier.first;
                x_rsum += qRed(color) * multiplier.first;
                x_gsum += qGreen(color) * multiplier.first;
                x_bsum += qBlue(color) * multiplier.first;
                y_sum += gray_val * multiplier.second;
                y_rsum += qRed(color) * multiplier.second;
                y_gsum += qGreen(color) * multiplier.second;
                y_bsum += qBlue(color) * multiplier.second;
            }


            //Gradient
            qreal gradient = qSqrt(qPow(x_sum / 8, 2) + qPow(y_sum / 8, 2));
            qreal r_gradient = qSqrt(qPow(x_rsum / 8, 2) + qPow(y_rsum / 8, 2));
            qreal g_gradient = qSqrt(qPow(x_gsum / 8, 2) + qPow(y_gsum / 8, 2));
            qreal b_gradient = qSqrt(qPow(x_bsum / 8, 2) + qPow(y_bsum / 8, 2));

            //Orientation
//            qreal orientation = qAtan2(y_sum, x_sum);
//            if ( orientation < orien_min )
//                orien_min = orientation;

//            if ( orientation > orien_max )
//                orien_max = orientation;
//            orientations.append(qMakePair(point, orientation));

            //Threshold
//            QRgb val;
//            if ( threshold < 0 ) {
//                val = qRgb(gradient, gradient, gradient);
//            } else if ( gradient > threshold ) {
//                val = qRgb(255,255,255);
//            } else
//                val = qRgb(0,0,0);
//            new_image.setPixel(point, val);

            new_image.setPixel(point, qRgb(r_gradient, g_gradient, b_gradient));
        }
    }

//    for( int i=0; i < orientations.size(); i++ ) {
//        QPair<QPoint, qreal> o_pair = orientations.at(i);
//        qreal normal = Utility::rangeConvert(orien_max, orien_min, 255, 0, o_pair.second);
//        orient_image.setPixel(o_pair.first, qRgb(normal, normal, normal));
//    }


    return new_image;
}

QImage EdgeDetection::gradientOrientationImage(QImage *image)
{
    QImage new_image(image->size(), QImage::Format_ARGB32_Premultiplied);

    QList<QPair<QPoint, QPair<int, int> > > x_y_struct;
    x_y_struct.append( qMakePair(QPoint(-1,-1), QPair<int,int>(-1, 1) ));
    x_y_struct.append( qMakePair(QPoint(-1,0), QPair<int,int>(-2, 0) ));
    x_y_struct.append( qMakePair(QPoint(-1,1), QPair<int,int>(-1,-1) ));
    x_y_struct.append( qMakePair(QPoint(0,-1), QPair<int,int>(0,2) ));
    x_y_struct.append( qMakePair(QPoint(0,0), QPair<int,int>(0,0) ));
    x_y_struct.append( qMakePair(QPoint(0,1), QPair<int,int>(0,-2) ));
    x_y_struct.append( qMakePair(QPoint(1,-1), QPair<int,int>(1,1) ));
    x_y_struct.append( qMakePair(QPoint(1,0), QPair<int,int>(2,0) ));
    x_y_struct.append( qMakePair(QPoint(1,1), QPair<int,int>(1, -1) ));

    QList<QPair<QPoint, qreal> > orientations;
    qreal orien_min = 1000;
    qreal orien_max = 0;

    for( int y=0; y < image->height(); y++ ) {
        for( int x=0; x < image->width(); x++ ) {
            QPoint point(x,y);
            int x_sum = 0;
            int y_sum = 0;
            int count = 0;
            //Perform Convolution on X
            for (int i=0; i < x_y_struct.size(); i++) {
                QPoint offset = x_y_struct.at(i).first;
                QPair<int,int> multiplier = x_y_struct.at(i).second;
                //Only convolve real pixels
                int pixel_x = offset.x() + x;
                int pixel_y = offset.y() + y;
                QRgb color;
                if ( pixel_x < 0 ||
                     pixel_x >= image->width() ||
                     pixel_y < 0 ||
                     pixel_y >= image->height() ) {
                    color = qRgb(128, 128, 128);
                } else {
                    color = image->pixel(QPoint(pixel_x, pixel_y));
                }

                int gray_val = qGray(color);
                x_sum += gray_val * multiplier.first;
                y_sum += gray_val * multiplier.second;
                count++;
            }

            //Orientation
            qreal orientation = qAtan2(y_sum/8, x_sum/8);
            if ( orientation < orien_min )
                orien_min = orientation;

            if ( orientation > orien_max )
                orien_max = orientation;
            orientations.append(qMakePair(point, orientation));

        }
    }

    for( int i=0; i < orientations.size(); i++ ) {
        QPair<QPoint, qreal> o_pair = orientations.at(i);
        qreal normal = Utility::rangeConvert(orien_max, orien_min, 255, 0, o_pair.second);
        new_image.setPixel(o_pair.first, qRgb(normal, normal, normal));
    }

    return new_image;
}

QHash<QPoint, QPair<qreal, qreal> > EdgeDetection::gradientValues(QImage *image)
{
    QHash<QPoint, QPair<qreal, qreal> > gradient;
    QList<QPair<QPoint, QPair<int, int> > > x_y_struct;
    x_y_struct.append( qMakePair(QPoint(-1,-1), QPair<int,int>(-1, 1) ));
    x_y_struct.append( qMakePair(QPoint(-1,0), QPair<int,int>(-2, 0) ));
    x_y_struct.append( qMakePair(QPoint(-1,1), QPair<int,int>(-1,-1) ));
    x_y_struct.append( qMakePair(QPoint(0,-1), QPair<int,int>(0,2) ));
    x_y_struct.append( qMakePair(QPoint(0,0), QPair<int,int>(0,0) ));
    x_y_struct.append( qMakePair(QPoint(0,1), QPair<int,int>(0,-2) ));
    x_y_struct.append( qMakePair(QPoint(1,-1), QPair<int,int>(1,1) ));
    x_y_struct.append( qMakePair(QPoint(1,0), QPair<int,int>(2,0) ));
    x_y_struct.append( qMakePair(QPoint(1,1), QPair<int,int>(1, -1) ));

    for( int y=0; y < image->height(); y++ ) {
        for( int x=0; x < image->width(); x++ ) {
            QPoint point(x,y);
            int x_sum = 0;
            int y_sum = 0;
            int count = 0;
            //Perform Convolution on X
            for (int i=0; i < x_y_struct.size(); i++) {
                QPoint offset = x_y_struct.at(i).first;
                QPair<int,int> multiplier = x_y_struct.at(i).second;
                //Only convolve real pixels
                int pixel_x = offset.x() + x;
                int pixel_y = offset.y() + y;
                QRgb color;
                if ( pixel_x < 0 ||
                     pixel_x >= image->width() ||
                     pixel_y < 0 ||
                     pixel_y >= image->height() ) {
                    color = qRgb(128, 128, 128);
                } else {
                    color = image->pixel(QPoint(pixel_x, pixel_y));
                }
                int gray_val = qGray(color);
                x_sum += gray_val * multiplier.first;
                y_sum += gray_val * multiplier.second;
                count++;
            }

            //Orientation
            qreal magnitude = qSqrt(qPow(x_sum / 8, 2) + qPow(y_sum / 8, 2));
            qreal orientation = qAtan2(y_sum/count, x_sum/count);
            gradient.insert(point, qMakePair(magnitude, orientation));
        }
    }
    return gradient;
}

QList<QPoint> EdgeDetection::getZeroCrossings(QImage *image)
{
    QList<QPair<QPoint, int > > kernel;
    kernel.append( qMakePair(QPoint(-1,-1), 0 ));
    kernel.append( qMakePair(QPoint(-1,0), 1 ));
    kernel.append( qMakePair(QPoint(-1,1), 0 ));
    kernel.append( qMakePair(QPoint(0,-1), 1 ));
    kernel.append( qMakePair(QPoint(0,0), -4 ));
    kernel.append( qMakePair(QPoint(0,1), 1 ));
    kernel.append( qMakePair(QPoint(1,-1), 0 ));
    kernel.append( qMakePair(QPoint(1,0), 1 ));
    kernel.append( qMakePair(QPoint(1,1), 0 ));

    QList<QPoint> zero_crossings;
    QHash<QPoint, int> laplacian;

    for( int y=0; y < image->height(); y++ ) {
        for( int x=0; x < image->width(); x++ ) {
            QPoint point(x,y);
            int sum = 0;
            if ( laplacian.contains(point) ) {
                sum = laplacian.value(point, 0);
            } else {
                sum = Convolution::sumConvolution(image, &kernel, &point);
                laplacian.insert(point, sum);
            }

            QPoint right(x+1, y);
            int right_val = 0;
            if ( right.x() < image->width() ) {
                if ( laplacian.contains(right) ) {
                    right_val = laplacian.value(right,0);
                } else {
                    right_val = Convolution::sumConvolution(image, &kernel, &point);
                    laplacian.insert(right, right_val);
                }
            }

            QPoint bot(x, y+1);
            int bot_val = 0;
            if ( bot.y() < image->height() ) {
                if ( laplacian.contains(bot) ) {
                    bot_val = laplacian.value(bot,0);
                } else {
                    bot_val = Convolution::sumConvolution(image, &kernel, &point);
                    laplacian.insert(bot, bot_val);
                }
            }

            //Look Right for Zero Crossing
            if ( (sum > 0 && right_val < 0)
                 || (sum < 0 && right_val > 0)) {
                zero_crossings.append(point);
                continue; //Dont check vertical we know this is an edge already
            }

            //Look down for Zero Crossing
            if ( (sum > 0 && bot_val < 0)
                 || (sum < 0 && bot_val > 0)) {
                zero_crossings.append(point);
            }
        }
    }




    return zero_crossings;
}

QImage EdgeDetection::combinationEdgeDetection(QImage *image)
{
    QImage new_image = Utility::blackImage(image->size());
    QImage grad_image = gradientMagnitudeImage(image);
    QImage gs_image = Utility::grayScaleImage(&grad_image);
    int threshold = QInputDialog::getInteger(0, "Threshold", "Threshold value:", 32, 0, 255);
    QImage binary_image = Utility::threshold(&gs_image, threshold);
    QList<QPoint> crossings = getZeroCrossings(image);


    //Loop through each feature
    foreach ( QPoint p, crossings ) {
        //If feature is real then loop through target boundary
        if ( qGray(binary_image.pixel(p)) > 0 ) {
            new_image.setPixel(p, binary_image.pixel(p));
        }
    }

    return new_image;
}

QImage EdgeDetection::blurColorImage(QImage *image)
{
    QImage new_image = Utility::blackImage(image->size());
    int blursize = QInputDialog::getInteger(0, "Blur", "Blur size:", 3, 1, 11,2);

    QList<QPair<QPoint, int> > kernel;
    int halfside;
    if ( blursize % 2 == 0 ) {
        halfside = blursize / 2;
    } else {
        halfside = (blursize-1) / 2;
    }
    for(int j=-halfside; j <= halfside; j++) {
        for(int i=-halfside; i <= halfside; i++) {
            kernel.append(qMakePair(QPoint(i,j), 1));
        }
    }

    //Convolution
    for( int y=0; y < image->height(); y++ ) {
        for( int x=0; x < image->width(); x++ ) {
            QPoint point(x,y);
            QRgb val = Convolution::colorSumConvolution(image, &kernel, &point, kernel.size());
            new_image.setPixel(point, val);
        }
    }

    return new_image;
}

QImage EdgeDetection::bilateralColorImage(QImage *image)
{
    QImage new_image = Utility::blackImage(image->size());
    int blursize = QInputDialog::getInteger(0, "Blur", "Blur size:", 3, 1, 11,2);

    QList<QPair<QPoint, int> > kernel;
    int halfside;
    if ( blursize % 2 == 0 ) {
        halfside = blursize / 2;
    } else {
        halfside = (blursize-1) / 2;
    }
    for(int j=-halfside; j <= halfside; j++) {
        for(int i=-halfside; i <= halfside; i++) {
            kernel.append(qMakePair(QPoint(i,j), 1));
        }
    }

    //Convolution
    for( int y=0; y < image->height(); y++ ) {
        for( int x=0; x < image->width(); x++ ) {
            QPoint point(x,y);
            QRgb val = Convolution::colorSumConvolution(image, &kernel, &point, kernel.size());
            new_image.setPixel(point, val);
        }
    }

    return new_image;
}

QImage EdgeDetection::blurImage(QImage *image)
{
    QImage new_image(image->size(), QImage::Format_ARGB32_Premultiplied);

    int blursize = QInputDialog::getInteger(0, "Blur", "Blur size:", 3, 1, 11,2);

    QList<QPoint> kernel;
    int halfside;
    if ( blursize % 2 == 0 ) {
        halfside = blursize / 2;
    } else {
        halfside = (blursize-1) / 2;
    }
    for(int j=-halfside; j <= halfside; j++) {
        for(int i=-halfside; i <= halfside; i++) {
            kernel.append(QPoint(i,j));
        }
    }

//    qreal min = 1000;
//    qreal max = 0;

//    QList<QPair<QPoint, int> > sums;

    for( int y=0; y < image->height(); y++ ) {
        for( int x=0; x < image->width(); x++ ) {
            QPoint point(x,y);
            int sum = 0;
            int count = 0;
            //Perform Convolution on X
            foreach( QPoint i, kernel) {
                //Only convolve real pixels
                int pixel_x = i.x() + x;
                int pixel_y = i.y() + y;
                if ( pixel_x < 0 ||
                     pixel_x >= image->width() ||
                     pixel_y < 0 ||
                     pixel_y >= image->height() ) {
                    continue;
                }
                QPoint temp(pixel_x, pixel_y);
                int gray_val = qGray(image->pixel(temp));
                sum += gray_val;
                count++;
            }
//            if ( sum < min )
//                min = sum;

//            if ( sum > max )
//                max = sum;
//            sums.append(qMakePair(point, sum));
            int val = sum / count;
            new_image.setPixel(point, qRgb(val, val, val));
        }
    }

//    for( int i=0; i < sums.size(); i++ ) {
//        QPair<QPoint, int> s_pair = sums.at(i);
//        qreal val = Utility::rangeConvert(max, min, 255, 0, s_pair.second);
//        new_image.setPixel(s_pair.first, qRgb(val, val, val));
//    }

    return new_image;
}
