#ifndef GRABCUT_H
#define GRABCUT_H

#include "GraphCut.h"
#include <QImage>
#include <QList>
#include <QPoint>
#include "AdvancedOperations_global.h"

class ADVANCEDOPERATIONSSHARED_EXPORT GrabCut
{
public:
    GrabCut();
    QImage getGrabCutImage(QImage *image, QRectF rect);

private:
    GraphCut *gcut;
    QImage *_image;
    QList<QPoint> *_foreground;
    QList<QPoint> *_unknown;
    QList<QPoint> *_background;
};

#endif // GRABCUT_H
