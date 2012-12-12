#ifndef CONNECTEDCOMPONENTS_H
#define CONNECTEDCOMPONENTS_H

#include <QImage>
#include <QMap>
#include <QMultiMap>
#include <QHash>
#include <QList>
#include <QPoint>
#include <QRgb>
#include <QSet>
#include <QWeakPointer>
#include <QSharedPointer>

#include "AdvancedOperations_global.h"

class ADVANCEDOPERATIONSSHARED_EXPORT ConnectedComponents
{
public:
    ConnectedComponents();
    ~ConnectedComponents();
    QImage connectComponents(QImage *image);
    QSharedPointer<QMultiMap<int, QPoint> > getLabelSets();


private:
    void fillNeighbors(int label, QPoint point);
    QHash<QPoint, int> *pixel_labels;
    QSharedPointer<QMultiMap<int, QPoint> > label_sets;
    QMap<int, QRgb> label_colors;
    int label_count;
    QImage *_image;

    QList<QPoint> *stack;
    qreal threshold;
};

#endif // CONNECTEDCOMPONENTS_H
