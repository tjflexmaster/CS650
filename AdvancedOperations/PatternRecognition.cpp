#include "PatternRecognition.h"
#include <QImage>
#include <QPoint>
#include <QMultiMap>
#include <QMap>
#include <QHash>
#include <QList>
#include <QSharedPointer>
#include <QImage>
#include <QDebug>
#include <QPair>
#include <QInputDialog>
#include "Utility.h"
#include "EdgeDetection.h"

PatternRecognition::PatternRecognition()
{
    _bounding_rect_feature = new QMap<int, qreal>();
    _circularity_feature = new QMap<int, qreal>();
    _true_circularity_feature = new QMap<int, qreal>();
    _moment_feature = new QMap<int, qreal >();
    _hollow_feature = new QMap<int, qreal >();
    _profile_feature = new QMap<int, qreal >();

    _k = 5;
    _k_means = new QMap<int, QList<qreal> >();
    _k_mean_components = new QMap<int, QList<int> >();
    _k_means_set = false;

    _label_colors.insert(-1, QColor(Qt::white).rgb());
    _label_colors.insert(0, QColor(Qt::blue).rgb());
    _label_colors.insert(1, QColor(Qt::cyan).rgb());
    _label_colors.insert(2, QColor(Qt::red).rgb());
    _label_colors.insert(3, QColor(Qt::yellow).rgb());
    _label_colors.insert(4, QColor(Qt::magenta).rgb());
    _label_colors.insert(5, QColor(Qt::darkGreen).rgb());
    _label_colors.insert(6, QColor(Qt::darkBlue).rgb());
    _label_colors.insert(7, QColor(Qt::darkCyan).rgb());
    _label_colors.insert(8, QColor(Qt::darkRed).rgb());
    _label_colors.insert(9, QColor(Qt::darkMagenta).rgb());
    _label_colors.insert(10, QColor(Qt::green).rgb());
    _label_colors.insert(11, QColor(Qt::darkYellow).rgb());
    _label_colors.insert(12, QColor(Qt::darkBlue).rgb());
    _label_colors.insert(13, QColor(Qt::gray).rgb());
    _label_colors.insert(14, QColor(Qt::darkGray).rgb());
    _label_colors.insert(15, qRgb(130, 240, 9));
    _label_colors.insert(16, qRgb(200,100,50));

    _pixel_labels = new QHash<QPoint, int>();
    _label_sets = QSharedPointer<QMultiMap<int, QPoint> >( new QMultiMap<int, QPoint>() ) ;

    //Set Active Features
    _active_features.append(_bounding_rect_feature);
//    _active_features.append(_true_circularity_feature);
//    _active_features.append(_circularity_feature);
//    _active_features.append(_moment_feature);
    _active_features.append(_hollow_feature);
    _active_features.append(_profile_feature);

}

PatternRecognition::~PatternRecognition()
{
    delete _bounding_rect_feature;
    delete _circularity_feature;
    delete _true_circularity_feature;
    delete _moment_feature;
    delete _hollow_feature;
    delete _profile_feature;
    delete _k_means;
    delete _k_mean_components;
    delete _pixel_labels;
}

QImage PatternRecognition::getConnectedComponents(QImage *image)
{
    _image = image;
    QImage new_image = Utility::blackImage(image->size());

    connectComponents(image, &new_image);

    return new_image;

}

QImage PatternRecognition::trainPatterns(QImage *image, QList<QPoint> *initial_components)
{
    //Connect component labels
    _image = image;
    QImage new_image = Utility::blackImage(image->size());

    //Set K value
    if ( !_k_means_set )
        _k = QInputDialog::getInt(0, "K Number", "Set K value:", 5, 1, 10);

    //Connect Components
    connectComponents(image);

    //First find the feature values
    findFeatures(_label_sets);

    //Display features
    for ( int i=0; i<_bounding_rect_feature->count(); i++) {
        qDebug() << "\\hline " << i << "&" << _bounding_rect_feature->value(i) << "&" << _true_circularity_feature->value(i) << "&" << _hollow_feature->value(i) << "&" << _profile_feature->value(i) << "\\\\";
    }

    //If k-means not set then randomly set them to some of the labels.
    if ( !_k_means_set ) {
        initializeKmeansByPoints(initial_components);
//        randomlyIntializeKmeans();
    }

    //Now assign each component to a class k
    int changes = 1;
    int loop_count = 0;
    while ( changes > 0 ) {
        qDebug() << "K Mean Loop Start";

        //Re-assign components to classes
        assignComponentsToKmeans();

        changes = calculateKmeansFromComponents();

        if ( loop_count > 10 ) {
            break;
        }
        loop_count++;
    }//end while

    //Draw our labeling
    labelPatterns(&new_image);

    return new_image;
}

QImage PatternRecognition::recognizePatterns(QImage *image)
{
    //Connect component labels
    _image = image;
    QImage new_image = Utility::blackImage(image->size());

    //Connect Components
    connectComponents(image);

    //First find the feature values
    findFeatures(_label_sets);

    if ( _k_means_set ) {
        //Assign components to a class
        assignComponentsToKmeans();

        //Draw the labeling
        labelPatterns(&new_image);

    }

    return new_image;
}

void PatternRecognition::findFeatures(QSharedPointer<QMultiMap<int, QPoint> > components)
{
    //Clear findings
    _bounding_rect_feature->clear();
    _circularity_feature->clear();
    _true_circularity_feature->clear();
    _moment_feature->clear();
    _hollow_feature->clear();
    _profile_feature->clear();


    //Boundary neighbors
    QList<QPair<int, int> > neighbors;
    neighbors.append(QPair<int, int>(-1,-1)); //nw
    neighbors.append(QPair<int, int>(0,-1)); //n
    neighbors.append(QPair<int, int>(1,-1)); //ne
    neighbors.append(QPair<int, int>(1,0)); //e
    neighbors.append(QPair<int, int>(1,1));//se
    neighbors.append(QPair<int, int>(0,1));//s
    neighbors.append(QPair<int, int>(-1,1)); //sw
    neighbors.append(QPair<int, int>(-1,0)); //w

    //Normalize True Circularity
    qreal min_true_circ = 1000;
    qreal max_true_circ = 0;

    //Loop through each component
    int key = 0;
    bool loop = true;
    while ( loop ) {

        QList<QPoint> component_points = components->values(key);
        //Initialize BR values
        int max_x = -1, max_y = -1;
        int min_x = 9999, min_y = 9999;

        //Initialize Circularity Values
        QPoint start_point(999,999);

        //Initialize Moment Means
        int x_sum = 0;
        int y_sum = 0;

        foreach(QPoint p, component_points) {
            //Initialize Bounding Rect Values
            if ( p.x() > max_x )
                max_x = p.x();

            if ( p.y() > max_y )
                max_y = p.y();

            if ( p.x() < min_x )
                min_x = p.x();

            if ( p.y() < min_y )
                min_y = p.y();

            //Initialize Circularity Values
            if ( p.x() <= start_point.x() && p.y() <= start_point.y() ) {
                start_point = p;
            }

            //Initialize Moment Means
            x_sum += p.x();
            y_sum += p.y();
        }

        //********************************
        // DENSITY
        //********************************
        if ( _active_features.contains(_bounding_rect_feature) ) {
            int bounding_rect_area = (max_x - (min_x-1)) * (max_y - (min_y-1));
            qreal ratio = (qreal) component_points.count() / bounding_rect_area;
//            qDebug() << "BR ratio: " << ratio << " Component: " << key;
            _bounding_rect_feature->insert(key, ratio);
        }


        //********************************
        // OUTER CIRCULARITY
        //********************************
        if ( _active_features.contains(_circularity_feature) ) {
            bool finished = false;
            QPoint c = QPoint(start_point.x()-1, start_point.y());
            QList<QPoint> border;
            border.append(start_point);

            while( !finished ) {

                QPoint b = border.last();
                QPair<int, int> diff(c.x() - b.x(), c.y() - b.y());
                int index = neighbors.indexOf(diff) + 1;
                if ( index == neighbors.count() ) {
                    index = 0;
                }

                //Get the next point we will check
                QPair<int, int> next = neighbors.at(index);
                QPoint next_point = QPoint(b.x() + next.first,  b.y() + next.second);

                if ( component_points.contains(next_point) ) {
                    if ( border.contains(next_point) ) {
                        finished = true;
    //                    qDebug() << "Border:" << border;
                    } else {
                        border.append(next_point);
                    }
                } else {
                    c = next_point;
                }
            }
            qreal circularity = qPow((qreal) border.count(), 2) / (qreal) component_points.count();
//            qDebug() << "Circularity: " << circularity << " Component: " << key;
            _circularity_feature->insert(key, circularity);
        }


        //********************************
        // TRUE CIRCULARITY
        //********************************
        if ( _active_features.contains(_true_circularity_feature) ) {
            QList<QPoint> border;
            foreach(QPoint p, component_points) {
                for(int i=0; i<neighbors.count(); i++) {
                    QPair<int, int> d = neighbors.at(i);
                    if (!component_points.contains(QPoint(p.x() + d.first, p.y() + d.second))) {
                        border.append(p);
                        break;
                    }
                }
            }
            qreal true_circularity = qPow((qreal) border.count(),2) / (qreal) component_points.count();
            if ( true_circularity < min_true_circ ) {
                min_true_circ = true_circularity;
            }
            if ( true_circularity > max_true_circ ) {
                max_true_circ = true_circularity;
            }
//            qDebug() << "True Perimeter: " << true_circularity << " Component: " << key;
            _true_circularity_feature->insert(key, true_circularity);
        }

        //********************************
        // MOMENTS
        //********************************
        if ( _active_features.contains(_moment_feature)) {
            //Initialize Moment Values
            qreal x_mean = (qreal) x_sum / (qreal) component_points.count();
            qreal y_mean = (qreal) y_sum / (qreal) component_points.count();

            qreal mew11 = 0;
            qreal mew20 = 0;
            qreal mew02 = 0;

            foreach(QPoint p, component_points) {
                qreal xdiff = p.x() - x_mean;
                qreal ydiff = p.y() - y_mean;
                mew11 += xdiff * ydiff;
                mew20 += xdiff * xdiff;
                mew02 += ydiff * ydiff;
            }
            mew11 = mew11 / (qreal) component_points.count();
            mew20 = mew20 / (qreal) component_points.count();
            mew02 = mew02 / (qreal) component_points.count();

            //now get eigenvalues
            qreal a = 1;
            qreal b = -mew20 + -mew02;
            qreal c1 =  -(mew11 * mew11) + (mew20 * mew02);

            qreal top = qSqrt((b*b) - (4 * a * c1));
            QList<qreal> eigenvalues;
            eigenvalues.append( (-b + top) / (2*a) );
            eigenvalues.append( (-b - top) / (2*a) );

//            qDebug() << "Eigenvalues:" << eigenvalues << " Ratio: " << eigenvalues[0] / eigenvalues[1] << " Component: " << key;;
            _moment_feature->insert(key, eigenvalues[0]);
        }

        //********************************
        // PROFILER
        //********************************
        if ( _active_features.contains(_profile_feature) ) {
            int x_median = max_x - qRound((max_x - (min_x-1)) / 2);
            int y_median = max_y - qRound((max_y - (min_y-1)) / 2);
            qreal x_mean = (qreal) x_sum / (qreal) component_points.count();
            qreal y_mean = (qreal) y_sum / (qreal) component_points.count();

            int x_offset = qRound(x_median + (x_median - x_mean));
            int y_offset = qRound(y_median + (y_median - y_mean));

            bool on_x = false;
            int on_x_count = 0;
            QList<QPoint> x_contact_points;
            for ( int i=min_y; i <= max_y; i++) {
                if ( component_points.contains(QPoint(x_offset, i)) ) {
                    if ( !on_x ) {
                        x_contact_points.append(QPoint(x_offset, i));
                        on_x = true;
                        on_x_count++;
                    }
                } else {
                    on_x = false;
                }
            }

            bool on_y = false;
            int on_y_count = 0;
            QList<QPoint> y_contact_points;
            for ( int i=min_x; i <= max_x; i++) {
                if ( component_points.contains(QPoint(i, y_offset)) ) {
                    if ( !on_y ) {
                        y_contact_points.append(QPoint(i, y_offset));
                        on_y = true;
                        on_y_count++;
                    }
                } else {
                    on_y = false;
                }
            }

            //If this is a potential E or F that is not rotated
            if ( on_x_count == 3 && on_x_count > on_y_count ) {
                //Go to the middle contact point
                QPoint p = x_contact_points.at(1);

                //Look at the median and mean diff and move in the direction that takes you from the mean to the median
                QPoint diff_point(0,0);
                if ( x_mean < x_median ) {
                    diff_point.setX(1);
                } else if ( x_mean > x_median ) {
                    diff_point.setX(-1);
                }

                if ( diff_point.x() != 0 ) {
                    //Now move in the x direction using the counter we just obtained to see where the middle part of the letter stops
                    QPoint next_point = p;
                    while(component_points.contains(next_point)) {
                        p = next_point;
                        next_point += diff_point;
                    }

                    //Now we assume that we have the furthest reaching x point for the middle of the letter.  Now we see if it still crosses the component 3 times
                    on_x = false;
                    on_x_count = 0;
                    for ( int i=min_y; i <= max_y; i++) {
                        if ( component_points.contains(QPoint(p.x(), i)) ) {
                            if ( !on_x ) {
                                on_x = true;
                                on_x_count++;
                            }
                        } else {
                            on_x = false;
                        }
                    }
                }
            //Rotated 90 degrees
            } else if ( on_y_count == 3 && on_y_count > on_x_count ) {
                //Go to the middle contact point
                QPoint p = y_contact_points.at(1);

                //Look at the median and mean diff and move in the direction that takes you from the mean to the median
                QPoint diff_point(0,0);
                if ( y_mean < y_median ) {
                    diff_point.setY(1);
                } else if ( y_mean > y_median ) {
                    diff_point.setY(-1);
                }

                if ( diff_point.y() != 0 ) {
                    //Now move in the x direction using the counter we just obtained to see where the middle part of the letter stops
                    QPoint next_point = p;
                    while(component_points.contains(next_point)) {
                        p = next_point;
                        next_point += diff_point;
                    }

                    //Now we assume that we have the furthest reaching x point for the middle of the letter.  Now we see if it still crosses the component 3 times
                    on_y = false;
                    on_y_count = 0;
                    for ( int i=min_x; i <= max_x; i++) {
                        if ( component_points.contains(QPoint(i, p.y())) ) {
                            if ( !on_y ) {
                                on_y = true;
                                on_y_count++;
                            }
                        } else {
                            on_y = false;
                        }
                    }
                }
            }

            _profile_feature->insert(key, qMax(on_x_count, on_y_count) );

        }

        //********************************
        // HOLLOW
        //********************************
        if ( _active_features.contains(_hollow_feature) ) {
            //find center values
            int center_x = max_x - ((max_x - min_x) / 2);
            int center_y = max_y - ((max_y - min_y) / 2);

            QPoint center(center_x, center_y);
            QPoint next_point;
            qreal hollow = 0;
            if ( !component_points.contains( center ) ) {
                //it is potentially hollow
                //simple method is to move in four directions and see what we run into

                //Move east first
                next_point = center;
                QPoint mover(1,0);
                bool east = false;
                while(next_point.x() <= max_x) {
                    if ( component_points.contains(next_point) ) {
                        east = true;
                        break;
                    }
                    next_point += mover;
                }

                if ( !east ) {
                    hollow = 0;
                } else {
                    next_point = center;
                    mover = QPoint(-1,0);
                    bool west = false;
                    while(next_point.x() >= min_x) {
                        if ( component_points.contains(next_point) ) {
                            west = true;
                            break;
                        }
                        next_point += mover;
                    }
                    if ( !west ) {
                        hollow = 0;
                    } else {
                        next_point = center;
                        mover = QPoint(0,-1);
                        bool north = false;
                        while(next_point.y() >= min_y) {
                            if ( component_points.contains(next_point) ) {
                                north = true;
                                break;
                            }
                            next_point += mover;
                        }

                        if ( !north ) {
                            hollow = 0;
                        } else {
                            next_point = center;
                            mover = QPoint(0,1);
                            bool south = false;
                            while(next_point.y() <= max_y) {
                                if ( component_points.contains(next_point) ) {
                                    south = true;
                                    break;
                                }
                                next_point += mover;
                            }

                            if ( !south ) {
                                hollow = 0;
                            } else {
                                hollow = 1;
                            }
                        }
                    }
                }

            }
            _hollow_feature->insert(key, hollow);
        }


        //Do we loop again
        key++;
        if ( !components->contains(key) ) {
            loop = false;
            _component_count = key;
        }
    }//end while

    //Normalize True Circularity
    for(int i=0; i<_true_circularity_feature->count(); i++) {
        qreal value = _true_circularity_feature->value(i);
        qreal norm_value = (value + 1 - min_true_circ) / (max_true_circ + 1 - min_true_circ);
        _true_circularity_feature->insert(i, norm_value);
    }

}

void PatternRecognition::connectComponents(QImage *image, QImage *new_image)
{
    _label_count = 0;
    _label_pairings.clear();
    _pixel_labels->clear();
    _label_sets->clear();

    QRgb bg_color = image->pixel(QPoint(0,0));

    QList<QPair<int, int> > neighbors;
    neighbors.append(QPair<int, int>(-1,0)); //w
//    neighbors.append(QPair<int, int>(-1,-1)); //nw
    neighbors.append(QPair<int, int>(0,-1)); //n
//    neighbors.append(QPair<int, int>(1,-1)); //ne

    //First Pass
    for(int y=0; y < image->height(); y++) {
        for ( int x=0; x < image->width(); x++) {
            QPoint p(x, y);

            QRgb p_color = image->pixel(p);

            //Are we background?
            if ( p_color == bg_color ) {
                continue;
            }

            QList<int> neighbor_labels;
            int min_label = _label_count + 1;
            for( int i=0; i < neighbors.count(); i++ ) {
                QPair<int, int> n = neighbors.at(i);
                QPoint neighbor(x+n.first, y+n.second);
                if ( image->valid(neighbor) ) {
                    QRgb color = image->pixel(neighbor);
                    if ( color == p_color ) {
                        int label = _pixel_labels->value(neighbor);
                        int label_min = _label_pairings.value(label, label);
                        neighbor_labels.append(label);
                        if ( label_min < min_label ) {
                            min_label = label_min;
                        }
                    }
                }
            }

            //If no matching neighbors then use the min and update the count
            if ( neighbor_labels.isEmpty() ) {
                _label_count++;
                _label_pairings.insert(_label_count, _label_count);
            } else if ( neighbor_labels.count() > 1 ) {
                foreach(int l, neighbor_labels) {
                    int neighbor_min = _label_pairings.value(l, _label_count);
                    if ( min_label < neighbor_min ) {
                        //If our neighbors are connected then we should store the minimum label they
                        //are connected too.
                        _label_pairings.insert(l, min_label);
                    }
                }
            }

            //Set the label
            _pixel_labels->insert(p, min_label);

        }
    }

    QList<int> used_labels;
    for(int y=0; y < image->height(); y++) {
        for ( int x=0; x < image->width(); x++) {

            QPoint p(x, y);
            if ( _pixel_labels->contains(p) ) {
                int label = _pixel_labels->value(p);
                int min_label = _label_pairings.value(label, label);
                while(label != min_label) {
                    label = min_label;
                    min_label =  _label_pairings.value(label, label);
                }

                int label_index = 0;
                if ( !used_labels.contains(min_label) ) {
                    used_labels.append(min_label);
                    label_index = used_labels.count() - 1;
                } else {
                    label_index = used_labels.indexOf(min_label);
                }
                _label_sets->insert(label_index, p);
                if ( new_image != 0 ) {
//                    int index = (min_label % 14);
                    QRgb rgb = _label_colors.value(label_index, QColor(Qt::white).rgb());
                    new_image->setPixel(p, rgb);
                }
            }
        }
    }
}

void PatternRecognition::resetTraining()
{
    _k_means->clear();
    _k_means_set = false;
    _label_sets->clear();
}

void PatternRecognition::randomlyIntializeKmeans()
{
    QList<int> used_values;
    for(int i=0; i<_k; i++) {
        int value = -1;
        while( used_values.contains(value) || value < 0 ) {
            value = qrand() % _component_count;
        }
        used_values.append(value);
        QList<qreal> feature_values;
        for ( int f_index=0; f_index < _active_features.count(); f_index++ ) {
            QMap<int, qreal> *feature = _active_features.at(f_index);
            feature_values.append(feature->value(value));
        }

        _k_means->insert(i, feature_values);
    }

    _k_means_set = true;
}

void PatternRecognition::initializeKmeansByPoints(QList<QPoint> *initial_components)
{
    if ( initial_components->count() >= _k ) {
        for( int i=0; i<_component_count; i++) {
            QList<QPoint> points = _label_sets->values(i);
            int k=-1;
            for ( int j=0; j < _k; j++ ) {
                if ( points.contains(initial_components->at(j))) {
                    k = j;
                    break;
                }
            }

            //setup this kmean to use values from this component
            if ( k >= 0 ) {
                QList<qreal> feature_values;
                for ( int f_index=0; f_index < _active_features.count(); f_index++ ) {
                    QMap<int, qreal> *feature = _active_features.at(f_index);
                    feature_values.append(feature->value(i));
                }

                _k_means->insert(k, feature_values);
                qDebug() << "New Means:" << feature_values << " K:" << k;
            }
        }
        _k_means_set = true;
    } else {
        randomlyIntializeKmeans();
    }
}

void PatternRecognition::assignComponentsToKmeans()
{
    _k_mean_components->clear();

    //Find mean values from kmeans
    QList<qreal> k_mean_values;
    for ( int i=0; i<_k; i++) {
        qreal mm = 0;
        for ( int f_index=0; f_index < _active_features.count(); f_index++ ) {
            mm += qPow(_k_means->value(i).value(f_index), 2);
        }
        k_mean_values.append(mm * 0.5);
    }

    //Re-assign components to classes
    for( int i=0; i < _component_count; i++ ) {
        int max_k = 0;
        qreal max_k_value = -9999;
        for ( int j=0; j < _k; j++ ) {
            qreal xm = 0;
            for ( int f_index=0; f_index < _active_features.count(); f_index++ ) {
                QMap<int, qreal> *feature = _active_features.at(f_index);
                xm += feature->value(i) * _k_means->value(j).value(f_index);
            }

            qreal gx = xm - k_mean_values.at(j);
            if ( gx > max_k_value ) {
                max_k = j;
                max_k_value = gx;
            }
        }
        QList<int> k_components = _k_mean_components->value(max_k, QList<int>());
        k_components.append(i);
        qDebug() << "Component: " << i << " K: " << max_k << " Diff: " << max_k_value;
        _k_mean_components->insert(max_k, k_components);
    }
}

int PatternRecognition::calculateKmeansFromComponents()
{
    int changes = 0;
    for ( int i=0; i < _k; i++ ) {

        //For each feature type get a mean value
        QList<int> k_components = _k_mean_components->value(i, QList<int>());

        QList<qreal> new_means;
        if ( k_components.count() > 0 ) {
            qreal feature_sums[10] = {0,0,0,0,0,0,0,0,0,0};
            foreach(int component, k_components) {
                for ( int f_index=0; f_index < _active_features.count(); f_index++ ) {
                    QMap<int, qreal> *feature = _active_features.at(f_index);
                    feature_sums[f_index] += feature->value(component);
                }
            }

            QList<qreal> old_means = _k_means->value(i);
            for ( int f_index=0; f_index < _active_features.count(); f_index++ ) {
                new_means.append( feature_sums[f_index] / (qreal) k_components.count() );
                if ( new_means.last() != old_means[f_index] ) {
                    changes++;
                }
            }
        } else {
            qDebug() << "No Components Randomly changing" << " K:" << i;
            int value = qrand() % _component_count;
            for ( int f_index=0; f_index < _active_features.count(); f_index++ ) {
                QMap<int, qreal> *feature = _active_features.at(f_index);
                new_means.append(feature->value(value));
            }
            changes++;
        }

        qDebug() << "New Means:" << new_means << " K:" << i;
        _k_means->insert(i, new_means);

    }
    return changes;

}

void PatternRecognition::labelPatterns(QImage *image)
{
    //Draw our labeling
    for ( int i=0; i < _k; i++ ) {
        QRgb color = _label_colors.value(i);
        QList<int> k_components = _k_mean_components->value(i);
        qDebug() << "K Count: " <<  k_components.count() << " Color:" << qRed(color) << "," << qGreen(color) << "," << qBlue(color);
        foreach( int k, k_components ) {
            QList<QPoint> pixels = _label_sets->values(k);
            foreach(QPoint p, pixels) {
                image->setPixel(p, color);
            }
        }

    }
}
