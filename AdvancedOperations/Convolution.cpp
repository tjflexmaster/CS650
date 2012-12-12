#include "Convolution.h"
#include <QMap>
#include <QPoint>
#include <QImage>
#include <qmath.h>
#include <QDebug>
#include <QRgb>

Convolution::Convolution()
{
}

int Convolution::sumConvolution(QImage *image, QList<QPair<QPoint, int> > *kernel, QPoint *coordinate)
{
    int sum = 0;
    int count = 0;
    for (int i=0; i < kernel->size(); i++) {
        QPoint offset = kernel->at(i).first;
        int multiplier = kernel->at(i).second;
        //Only convolve real pixels
        int pixel_x = offset.x() + coordinate->x();
        int pixel_y = offset.y() + coordinate->y();
        if ( pixel_x < 0 ||
             pixel_x >= image->width() ||
             pixel_y < 0 ||
             pixel_y >= image->height() ) {
            continue;
        }

        QPoint temp(pixel_x, pixel_y);
        int gray_val = qGray(image->pixel(temp));
        sum += multiplier * gray_val;
        count++;
    }
    return sum/count;
}

QRgb Convolution::colorSumConvolution(QImage *image, QList<QPair<QPoint, int> > *kernel, QPoint *coordinate, int denominator)
{
    int r = 0;
    int g = 0;
    int b = 0;
    int count=0;
    for (int i=0; i < kernel->size(); i++) {
        QPoint offset = kernel->at(i).first;
        int multiplier = kernel->at(i).second;
        //Only convolve real pixels
        int pixel_x = offset.x() + coordinate->x();
        int pixel_y = offset.y() + coordinate->y();
        if ( pixel_x < 0 ||
             pixel_x >= image->width() ||
             pixel_y < 0 ||
             pixel_y >= image->height() ) {
            r += 128;
            g += 128;
            b += 128;
        } else {

            QPoint temp(pixel_x, pixel_y);
            QRgb color = image->pixel(temp);
            r += multiplier * qRed(color);
            g += multiplier * qGreen(color);
            b += multiplier * qBlue(color);
        }
        count++;
    }
    return qRgb(r/denominator, g/denominator, b/denominator);
}

QRgb Convolution::colorBilateralConvolution(QImage *image, QList<QPair<QPoint, int> > *kernel, QPoint *coordinate)
{
    int r = 0;
    int g = 0;
    int b = 0;
    int count=0;
    for (int i=0; i < kernel->size(); i++) {
        QPoint offset = kernel->at(i).first;
        int multiplier = kernel->at(i).second;
        //Only convolve real pixels
        int pixel_x = offset.x() + coordinate->x();
        int pixel_y = offset.y() + coordinate->y();
        if ( pixel_x < 0 ||
             pixel_x >= image->width() ||
             pixel_y < 0 ||
             pixel_y >= image->height() ) {
            continue;
        }

        QPoint temp(pixel_x, pixel_y);
        QRgb color = image->pixel(temp);

        r += multiplier * qRed(color);
        g += multiplier * qGreen(color);
        b += multiplier * qBlue(color);
        count++;

    }
    return qRgb(r/count, g/count, b/count);
}

//QMap<QPoint, int> Convolution::convolveGrayscale(QImage *image, QMap<QPoint, int> *structure)
//{
//    QMap<QPoint, int> result;
//    for( int y=0; y < image->height(); y++ ) {
//        for( int x=0; x < image->width(); x++ ) {
//            QPoint point(x,y);
//            int sum = 0;
//            foreach( QPoint i, structure->keys()) {
//                //Only convolve real pixels
//                int pixel_x = i.x() + x;
//                int pixel_y = i.y() + y;
//                if ( pixel_x < 0 ||
//                     pixel_x >= image->width() ||
//                     pixel_y < 0 ||
//                     pixel_y >= image->height() ) {
//                    continue;
//                }
//                QPoint temp(pixel_x, pixel_y);
//                sum += qGray(image->pixel(temp)) * structure->value(i,1);
//            }
//            result.insert(point, sum);
//        }
//    }
//    return result;
//}

//QMap<QPoint, int> Convolution::getSquareStruct(int side)
//{
//    QMap<QPoint, int> result;
//    int halfside;
//    if ( side % 2 == 0 ) {
//        halfside = side / 2;
//    } else {
//        halfside = (side-1) / 2;
//    }
//    for(int j=-halfside; j <= halfside; j++) {
//        for(int i=-halfside; i <= halfside; i++) {
//            result.insert(QPoint(i,j), 1);
//        }
//    }
//    return result;
//}

//QMap<QPoint, int> Convolution::getRectStruct(int height, int width)
//{
//    QMap<QPoint, int> result;
//    int half_w;
//    if ( width % 2 == 0 ) {
//        half_w = width / 2;
//    } else {
//        half_w = (width-1) / 2;
//    }
//    int half_h;
//    if ( height % 2 == 0 ) {
//        half_h = height / 2;
//    } else {
//        half_h = (height-1) / 2;
//    }
//    for(int j=-half_h; j <= half_h; j++) {
//        for(int i=-half_w; i <= half_w; i++) {
//            result.insert(QPoint(i,j), 1);
//        }
//    }
//    return result;
//}


//QMap<QPoint, int> Convolution::getCircleStruct(int radius)
//{
//    QMap<QPoint, int> result;

//    qreal r2 = qPow(radius, 2);

//    for(int j=-radius; j <= radius; j++) {
//        for(int i=-radius; i <= radius; i++) {
//            qreal val = qSqrt( (r2 - qPow(i,2)) );
//            if ( qRound(val) < qAbs(j) ) {
//                qDebug() << QPoint(i,j) << " OK";
//            } else {
//                qDebug() << QPoint(i,j) << " BAD";
//            }
//            result.insert(QPoint(i,j), 1);
//        }
//    }
//    return result;
//}

