#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "Histogram_global.h"

#include <QImage>
#include <QMap>
#include <QWidget>

class HISTOGRAMSHARED_EXPORT Histogram {
public:
    Histogram();
    Histogram(QImage* image);
    void load(QImage* image);
    QMap<int, int>::iterator begin();
    QMap<int, int>::iterator end();
    int getMaxValue();
    int getPixelCount();
    int getValue(int t);
private:
    void initialize();
    QMap<int, int> _histogram;
    int _max;
    int _pixel_count;

};

#endif // HISTOGRAM_H
