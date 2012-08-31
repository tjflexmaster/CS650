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
private:
    void initialize();
    QMap<int, int> _histogram;
    int _max;

};

#endif // HISTOGRAM_H
