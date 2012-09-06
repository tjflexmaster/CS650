#ifndef OTSU_H
#define OTSU_H

#include "Otsu_global.h"
#include <QImage>
#include "Histogram.h"

class OTSUSHARED_EXPORT Otsu {
public:
    Otsu();
    QImage processImage(QImage* image);

private:
    int calculateThreshold(Histogram* histogram);


};

#endif // OTSU_H
