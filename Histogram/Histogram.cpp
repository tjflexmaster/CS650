#include "Histogram.h"
#include <QImage>
#include <QMap>
#include <QColor>
#include <QPainter>
#include <QDebug>

Histogram::Histogram()
{
    initialize();
}

Histogram::Histogram(QImage* image)
{
    initialize();
    load(image);
}

void Histogram::initialize()
{
     _pixel_count = 0;
     _max = 0;
    for ( int i=0; i < 256; i++ ) {
        _histogram.insert(i, 0);
    }
}

void Histogram::load(QImage* image)
{
    _pixel_count = image->height() * image->width();
    _max = 0;
    for (int y=0; y < image->height(); y++ ) {
        for ( int x=0; x < image->width(); x++) {
            int gray_val = qGray(image->pixel(x,y));
            int val = _histogram.value(gray_val) + 1;
//            qDebug() << "Pixel:" << image->pixel(x, y) << " Gray:" << gray_val << " Old Value:" << val;
            _histogram.insert(gray_val, val);
            if ( val > _max ) {
                _max = val;
            }
        }
    }

}

QMap<int, int>::iterator Histogram::begin()
{
    return _histogram.begin();
}

QMap<int, int>::iterator Histogram::end()
{
    return _histogram.end();
}

int Histogram::getMaxValue()
{
    return _max;
}

int Histogram::getPixelCount()
{
    return _pixel_count;
}

int Histogram::getValue(int t)
{
    if ( _histogram.contains(t) )
        return _histogram.value(t,0);
    else
        return -1;
}

