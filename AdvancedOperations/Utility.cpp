#include "Utility.h"
#include <QImage>
#include <QColor>
#include <qmath.h>

const double Utility::PI = qAtan(1.0) * 4;

Utility::Utility()
{
}

qreal Utility::rangeConvert(qreal old_max, qreal old_min, qreal new_max, qreal new_min, qreal value)
{
    return (((value - old_min) * (new_max - new_min)) / (old_max - old_min)) + new_min;
}

QImage Utility::threshold(QImage *image, int threshold)
{
    QImage new_image = Utility::binaryImage(image->size());
    for (int y=0; y<image->height(); y++) {
        for ( int x=0; x < image->width(); x++ ) {
            QPoint point(x,y);
            if ( qGray(image->pixel(point)) > threshold ) {
                new_image.setPixel(point, 1);
            }
        }
    }
    return new_image;
}

QImage Utility::thresholdLightness(QImage *image, int threshold)
{
    QImage new_image = Utility::blackImage(image->size());
    for (int y=0; y<image->height(); y++) {
        for ( int x=0; x < image->width(); x++ ) {
            QPoint point(x,y);
            QColor c = QColor::fromRgb(image->pixel(point));
            if ( c.lightness() < threshold ) {
                new_image.setPixel(point, qRgb(0,0,0));
            }
        }
    }
    return new_image;
}

QImage Utility::blackImage(QSize size)
{
    QImage new_image(size, QImage::Format_ARGB32_Premultiplied);
    new_image.fill(QColor(Qt::black).rgb());
    return new_image;
}

QImage Utility::binaryImage(QSize size)
{
    QImage new_image(size, QImage::Format_Indexed8);
    new_image.setColor(0, qRgb(0,0,0));
    new_image.setColor(1,qRgb(255,255,255));
    new_image.fill(0);
    return new_image;
}

QImage Utility::grayScaleImage(QImage *image)
{
    QImage new_image = Utility::blackImage(image->size());
    for (int y=0; y<image->height(); y++) {
        for ( int x=0; x < image->width(); x++ ) {
            QPoint point(x,y);
            int gray_val = qGray(image->pixel(point));
            new_image.setPixel(point, qRgb(gray_val, gray_val, gray_val) );
        }
    }
    return new_image;
}

qreal Utility::colorSimilarity(QRgb seed, QRgb target, qreal variance)
{
    double _c = 2 * qPow(variance, 2);
    double r_diff = (double) qRed(target) - qRed(seed);
    double g_diff = (double) qGreen(target) - qGreen(seed);
    double b_diff = (double) qBlue(target) - qBlue(seed);
    double r_energy = qExp( -(qPow((r_diff), 2) / _c));
    double g_energy = qExp( -(qPow((g_diff), 2) / _c));
    double b_energy = qExp( -(qPow((b_diff), 2) / _c));

    return (r_energy + g_energy + b_energy) / 3;

}

qreal Utility::colorSimilarity(QRgb seed, QRgb target)
{
    double r_diff = (double) qRed(target) - qRed(seed);
    double g_diff = (double) qGreen(target) - qGreen(seed);
    double b_diff = (double) qBlue(target) - qBlue(seed);

    return (qAbs(r_diff) + qAbs(g_diff) + qAbs(b_diff)) / 3;
}

QString Utility::pointToString(QPoint point)
{
    return QString("%1,%2").arg(point.x()).arg(point.y());
}
