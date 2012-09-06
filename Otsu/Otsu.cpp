#include "Otsu.h"
#include <QImage>
#include "Histogram.h"
#include <QDebug>
#include <qmath.h>

Otsu::Otsu()
{
}

QImage Otsu::processImage(QImage *image)
{
    Histogram hist(image);
    int threshold = calculateThreshold(&hist);
    qDebug() << "Threshold: " << threshold;
    QImage result(image->size(), QImage::Format_ARGB32_Premultiplied);
    for (int y=0; y < image->height(); y++) {
        for ( int x=0; x < image->width(); x++) {
            int val = 0;
            int gray_val = qGray(image->pixel(x,y));
            if ( gray_val > threshold ) {
                val = 255;
            }
            result.setPixel(x,y, qRgb(val, val, val));
        }
    }
    qDebug() << "Finished Otsu Processing";
    return result;
}

int Otsu::calculateThreshold(Histogram* histogram)
{
    int threshold = -1;
    qreal max_between_variance = 0.0;

    int total_pixel_count = histogram->getPixelCount();

    qreal total_prob = 0.0;
    for ( int t=0; t < 256; t++ ) {
        total_prob +=  qreal(t) * qreal(histogram->getValue(t));
    }

    qreal bg_count = 0.0;
    qreal fg_count = 0.0;
    qreal bg_prob = 0.0;
    qreal fg_prob = 0.0;

    for ( int t=0; t < 256; t++ ) {
        int pixel_count = histogram->getValue(t);
        bg_count += qreal(pixel_count);
        fg_count = qreal(total_pixel_count) - bg_count;
        qreal weight1 = bg_count / qreal(total_pixel_count);
        if ( weight1 == 0 ) {
            continue;
        }
        qreal weight2 = fg_count / qreal(total_pixel_count);
        if ( weight2 == 0 ) {
            break;
        }
        bg_prob += qreal(t) * qreal(pixel_count);
        fg_prob = total_prob - bg_prob;
        qreal mean1 = bg_prob / bg_count;
        qreal mean2 = fg_prob / fg_count;

        qreal between_variance = weight1 * weight2 * qPow((mean1 - mean2), 2);

        qDebug() << "Pixel Count: " << pixel_count;
        qDebug() << "BG_count: " << bg_count;
        qDebug() << "FG_count: " << fg_count;
        qDebug() << "Total PC: " << total_pixel_count;
        qDebug() << "Weight1: " << weight1;
        qDebug() << "Weight2: " << weight2;
        qDebug() << "Total CP: " << total_prob;
        qDebug() << "BG_prob: " << bg_prob;
        qDebug() << "FG_prob: " << fg_prob;
        qDebug() << "Mean1: " << mean1;
        qDebug() << "Mean2: " << mean2;
        qDebug() << "Between: " << between_variance;

        if ( between_variance > max_between_variance ) {
            threshold = t;
            max_between_variance = between_variance;
        }

    }

    return threshold;
}
