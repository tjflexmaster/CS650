#ifndef GRAPHCUT_H
#define GRAPHCUT_H

#include <QObject>
#include <QImage>
#include <QList>
#include <QPoint>
#include "maxflow/block.h"
#include "maxflow/graph.h"
#include "AdvancedOperations_global.h"
#include "Histogram.h"
#include <QFutureWatcher>

typedef Graph<double,double,double> GraphType;
class ADVANCEDOPERATIONSSHARED_EXPORT GraphCut
{

public:
    GraphCut();
    QImage graphCutImage(QImage *image, QList<QPoint> *foreground, QList<QPoint> *background);
    void graphCutImageAlgorithm(GraphType *graph, QImage *image, QList<QPoint> *foreground, QList<QPoint> *background);

private:

    double getNLinkValue(QRgb seed, QRgb pixel);

    double getTLinkForeground(QPoint pixel);
    double getTLinkBackground(QPoint pixel);
    double getTLinkValue(QPoint pixel, QPoint seed);
    double thresholdValue(double value);

    QList<QPoint> *_foreground;
    QList<QPoint> *_background;
    QImage *_image;
    GraphType *_graph;

    Histogram histogram;
    QMap<int, int> fg_histogram;
    QMap<int, int> bg_histogram;

    qreal color_weight;
    qreal max_separation;
    double _red_sigma;
    double _green_sigma;
    double _blue_sigma;
    double _lambda;

    //T LInk Values
    double _sigma;
    double _a;
    double _c;
    double _threshold;

    QFutureWatcher<int> *nlink_watcher;
};

#endif // GRAPHCUT_H
