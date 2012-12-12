#ifndef PATTERNRECOGNITION_H
#define PATTERNRECOGNITION_H

#include <QImage>
#include <QMultiMap>
#include <QPoint>
#include <QList>
#include <QSharedPointer>
#include <QPair>
#include <QHash>
#include <QImage>
#include "AdvancedOperations_global.h"

class ADVANCEDOPERATIONSSHARED_EXPORT PatternRecognition
{
public:
    PatternRecognition();
    ~PatternRecognition();
    QImage getConnectedComponents(QImage *image);
    QImage trainPatterns(QImage *image, QList<QPoint> *initial_components);
    void resetTraining();
    QImage recognizePatterns(QImage *image);


private:
    void findFeatures(QSharedPointer<QMultiMap<int, QPoint> > components);
    void connectComponents(QImage *image, QImage *new_image = 0);
    void randomlyIntializeKmeans();
    void initializeKmeansByPoints(QList<QPoint> *initial_components);
    void assignComponentsToKmeans();
    int calculateKmeansFromComponents();
    void labelPatterns(QImage *image);

    QList<QMap<int, qreal> *> _active_features;
    QMap<int, qreal> *_bounding_rect_feature;
    QMap<int, qreal> *_circularity_feature;
    QMap<int, qreal> *_true_circularity_feature;
    QMap<int, qreal> *_moment_feature;
    QMap<int, qreal> *_hollow_feature;
    QMap<int, qreal> *_profile_feature;

    //K-Means
    int _k;
    QMap<int, QList<qreal> > *_k_means;
    QMap<int, QList<int> > *_k_mean_components;
    bool _k_means_set;

    //Connected components
    QHash<QPoint, int> *_pixel_labels;
    QSharedPointer<QMultiMap<int, QPoint> > _label_sets;
    QMap<int, int> _label_pairings;
    QMap<int, QRgb> _label_colors;
    int _label_count;
    int _component_count;
    QImage *_image;




};

#endif // PATTERNRECOGNITION_H
