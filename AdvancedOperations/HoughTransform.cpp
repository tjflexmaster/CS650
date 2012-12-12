#include "HoughTransform.h"
#include <QImage>
#include <QList>
#include <QPoint>
#include <QInputDialog>
#include "EdgeDetection.h"
#include "Utility.h"
#include <QDebug>
#include <qmath.h>
#include <QSet>

HoughTransform::HoughTransform()
{
}


QImage HoughTransform::transform(QImage *image)
{
    int radius = QInputDialog::getInteger(0, "Radius", "Radius (16):", 16, 1, 100);
    QSize padding(radius,radius);
    EdgeDetection e;
    QImage new_image = Utility::blackImage(image->size() + padding + padding);
    QImage grad_image = e.gradientMagnitudeImage(image);
    QHash<QPoint, QPair<qreal, qreal> > grad_values = e.gradientValues(image);
    QImage gs_image = Utility::grayScaleImage(&grad_image);
    int threshold = QInputDialog::getInteger(0, "Threshold", "Threshold value:", 32, 0, 255);
    QImage binary_image = Utility::threshold(&gs_image, threshold);
    QImage test_image = e.combinationEdgeDetection(image);
    QList<QPoint> crossings = e.getZeroCrossings(image);


    QHash<QPoint, qreal> accumulator1;
    QHash<QPoint, qreal> accumulator2;
    QList<QPoint> features;
    QHash<QPoint, qreal> feature_sums;
    QHash<QPoint, QPoint> gerig_max;
    QList<QPoint> circle32 = getCircleTable(radius);


    int max = 0;
    //Loop through each feature
    foreach ( QPoint p, crossings ) {
        //If feature is real then loop through target boundary
        if ( qGray(binary_image.pixel(p)) > 0 ) {
            //Save all the points this feature touches
            features.append(p);

//            //Loop through degrees
//            const qreal PI  =3.141592653589793238462;
//            qreal theta = grad_values.value(p).second;
////            qDebug() << "Degree: " << (theta * 180) / PI;
//            qreal theta2 = theta + PI;
//            qreal offset = PI/18;
//            qreal onedegree = PI/180;

//            int i=0;
//            qreal angle1 = 0;
//            qreal angle2 = 0;
//            QSet<QPoint> set;
//            //Loop by 1 degree
//            while ( i < 20 ) {
//                angle1 = theta + offset + (onedegree * i);
//                angle2 = theta2 + offset + (onedegree * i);

//                QPointF p1 = QPointF((radius * qCos(angle1)), (radius * qSin(angle1)));
//                QPointF p2 = QPointF((radius * qCos(angle2)), (radius * qSin(angle2)));

//                QPoint new_p1 =  p1.toPoint() + (p + QPoint(padding.width(), padding.height()));
////                qDebug() << "Point: " << p1.toPoint() << "New Point:" << new_p1;
//                set.insert(new_p1);
//                set.insert( p2.toPoint() + (p + QPoint(padding.width(), padding.height())));
//                i++;
//            }

////            qDebug() << "Set size: " << set.size();

//            foreach (QPoint t, set) {
////                qDebug() << "Point Offset: " << t;
//                qreal val = accumulator1.value(t, 0) + 1;
//                accumulator1.insert(t, val);
//                if ( val > max ) {
//                    max = val;
//                }
//            }

            //Loop through target boundary
            foreach(QPoint i, circle32) {
                QPoint new_p = i + (p + QPoint(padding.width(), padding.height()));
                //Increment accumulator value
                qreal val = accumulator1.value(new_p, 0) + 1;
                accumulator1.insert(new_p, val);
                if ( val > max ) {
                    max = val;
                }
            }
        }
    }

    QHashIterator<QPoint, qreal> iter(accumulator1);
    //Threshold the accumulator
    int accum_thresh = QInputDialog::getInteger(0, "Threshold", "Accumulator Threshold value:", 10, 0, 255);
    while( iter.hasNext() ) {
        iter.next();
        int val = iter.value();
        QPoint p = iter.key();
        qreal circle_size = circle32.size();


        if ( val >= accum_thresh ) {
            qreal weight_x = 1;
            qreal weight_y = 1;
            qreal threshold = 0;
            qreal diff_x = 0;
            qreal diff_y = 0;
            bool edge = false;
            if ( p.x() - padding.width() < 0 ) {
                diff_x = qAbs(p.x() - padding.width());
                edge = true;
            } else if ( p.x() + padding.width() > new_image.width() - 1 ) {
                diff_x = qAbs(p.x() + padding.width() - (new_image.width() - 1));
                edge = true;
            }

            if ( p.y() - padding.height() < 0 ) {
                diff_y = qAbs(p.y() - padding.height());
                edge = true;
            } else if ( p.y() + padding.height() > new_image.height() - 1 ) {
                diff_y = qAbs(p.y() + padding.height() - (new_image.height() - 1));
                edge = true;
            }

            if ( edge ) {
                weight_x = qSqrt(qPow(radius,2) - qPow(diff_x,2)) / circle_size;
                weight_y = qSqrt(qPow(radius,2) - qPow(diff_y,2)) / circle_size;
                threshold = circle_size * weight_x * weight_y;
                if ( val < threshold ) {
                    accumulator1.remove(p);
                }
            }
            //Do nothing for normal positions

        } else {
            accumulator1.remove(p);
        }
    }

    //Blur Accumulator
//    QList<QPoint> kernel;
//    kernel.append(QPoint(-1,-1));
//    kernel.append(QPoint(-1,0));
//    kernel.append(QPoint(-1,1));
//    kernel.append(QPoint(0,-1));
//    kernel.append(QPoint(0,0));
//    kernel.append(QPoint(0,1));
//    kernel.append(QPoint(1,-1));
//    kernel.append(QPoint(1,0));
//    kernel.append(QPoint(1,1));
//    for( int y=0; y < image->height(); y++ ) {
//        for( int x=0; x < image->width(); x++ ) {
//            qreal sum = 0;
//            QPoint p(x + padding.width(), y + padding.height());
//            //Perform Convolution on X
//            foreach( QPoint i, kernel) {
//                QPoint new_p = p + i;
//                sum += accumulator1.value(new_p, 0);
//            }

//            accumulator2.insert(p, sum/kernel.size());
//        }
//    }
//    accumulator1.clear();
//    accumulator1 = accumulator2;
//    accumulator2.clear();

    //Gerig Solution
    max = 0;
//    int max_thresh = QInputDialog::getInteger(0, "Threshold", "Max Gerig Threshold value:", 10, 0, 255);
    qDebug() << "Gerig Solution";
    foreach(QPoint p, features) {
        qreal max = 0;
        QPoint max_point;
        foreach(QPoint i, circle32) {
            QPoint new_p = i + (p + QPoint(padding.width(), padding.height()));

            qreal val = accumulator1.value(new_p, 0);
            if ( val > max ) {
                max = val;
                max_point = new_p;
            }
        }

        //Save the max point, if no max then dont count it as a feature
        if ( max > 5 ) {
            gerig_max.insert(p, max_point);
        } else {
            int index = features.indexOf(p);
            features.removeAt(index);
        }
    }

    foreach(QPoint p, features) {
        QPoint mp = gerig_max.value(p);
        qreal val = accumulator2.value(mp, 0) + 1;
        if ( val > max ) {
            max = val;
        }
        accumulator2.insert(mp, val);
    }

    accumulator1.clear();
    accumulator1 = accumulator2;
    accumulator2.clear();

    //Sharpen Points
//    qreal sharp_thresh = circle32.size() * 0.3;
    int blursize = QInputDialog::getInteger(0, "Blur", "Sharpen size:", 5, 1, 21,2);

    QList<QPoint> kernel;
    int halfside;
    if ( blursize % 2 == 0 ) {
        halfside = blursize / 2;
    } else {
        halfside = (blursize-1) / 2;
    }
    for(int j=-halfside; j <= halfside; j++) {
        for(int i=-halfside; i <= halfside; i++) {
            kernel.append(QPoint(i,j));
        }
    }
    iter = QHashIterator<QPoint, qreal>(accumulator1);
    while( iter.hasNext() ) {
        iter.next();
        QPoint p = iter.key();
        qreal val = iter.value();

        bool max_val = true;
        foreach( QPoint i, kernel) {
            QPoint new_p = p + i;
            qreal neighbor_val = accumulator1.value(new_p, 0);
            if ( neighbor_val > val ) {
                max_val = false;
                break;
            }
        }
        if ( max_val ) {
            accumulator2.insert(p, val);
        }
    }
    accumulator1.clear();
    accumulator1 = accumulator2;
    accumulator2.clear();

    //Match with initial features
    iter = QHashIterator<QPoint, qreal>(accumulator1);
    while( iter.hasNext() ) {
        iter.next();
        QPoint p = iter.key();

        //Keep edge points
        QPoint moved_point = p - QPoint(padding.width(), padding.height());
        if ( moved_point.x() < 0
             || moved_point.x() >= image->width()
             || moved_point.y() < 0
             || moved_point.y() >= image->height() ) {

            accumulator2.insert(p, 200);
//            qreal hits = 0;
//            foreach(QPoint i, circle32) {
//                QPoint new_p = i + moved_point;
//                if ( (new_p.x() >= 0 && new_p.x() < gs_image.width()) &&
//                     (new_p.y() >= 0 && new_p.y() < gs_image.height()) ) {
//                    if ( qGray(test_image.pixel(new_p)) > 0 ) {
//                        hits++;
//                    }
//                }
//            }
//
            continue;
        }

        qreal hits = 0;
        foreach(QPoint i, circle32) {
            QPoint new_p = i + moved_point;
            if ( (new_p.x() >= 0 && new_p.x() < gs_image.width()) &&
                 (new_p.y() >= 0 && new_p.y() < gs_image.height()) ) {
                if ( qGray(test_image.pixel(new_p)) > 0 ) {
                    hits++;
                }
            }
        }

        qDebug() << "Hits: " << hits;
        if ( hits > circle32.size() * 0.25 ) {
            accumulator2.insert(p, iter.value());
        }
    }
    accumulator1.clear();
    accumulator1 = accumulator2;
    accumulator2.clear();

    //Variation Method
//    int iter_count = QInputDialog::getInteger(0, "Iteration Count", "Iteraction Count value:", 10, 0, 1000);
//    while( iter_count > 0 ) {
//        max = 0;
//        //Sum up the feature position values
//        foreach(QPoint p, features) {
//            qreal sum = 0;
//            foreach(QPoint i, circle32) {
//                QPoint new_p = i + (p + QPoint(padding.width(), padding.height()));
//                //Increment accumulator value
//                sum += accumulator1.value(new_p, 0);
//            }
//            feature_sums.insert(p, sum);
//        }

//        qDebug() << "Setting Second Accumulator";
//        //Set new accumulator value
//        foreach(QPoint p, features) {
//            qreal sum = feature_sums.value(p, 1);
////            qDebug() << p << " sum =" << sum;
//            foreach(QPoint i, circle32) {
//                QPoint new_p = i + (p + QPoint(padding.width(), padding.height()));
//                //Increment accumulator value
//                qreal weight = accumulator1.value(new_p, 0) / sum;
//                qreal new_val = accumulator2.value(new_p, 0) + weight;
//                accumulator2.insert(new_p, new_val);
//                if ( new_val > max )
//                    max = new_val;
//            }
//        }

//        //Set accumulator 1 = accumulator 2 for next iteration
//        accumulator1.clear();
//        accumulator1 = accumulator2;
//        accumulator2.clear();

//        //decrement loop counter
//        iter_count--;
//    }


    iter = QHashIterator<QPoint, qreal>(accumulator1);
    QImage final(*image);
    while( iter.hasNext() ) {
        iter.next();
//        qreal val = Utility::rangeConvert(max, 0, 255, 0, iter.value());
//        if ( val > 100 ) {
//            foreach(QPoint i, circle32) {
//                QPoint new_p = i + iter.key();
//                new_image.setPixel(new_p, qRgb(255,255,255));
//            }
//        }
        QPoint moved_point = iter.key() - QPoint(padding.width(), padding.height());
        foreach(QPoint i, circle32) {
            QPoint new_p = i + moved_point;
            if ( (new_p.x() >= 0 && new_p.x() < image->width()) &&
                 (new_p.y() >= 0 && new_p.y() < image->height()) ) {
                final.setPixel(new_p, qRgb(255,255,255));
            }
        }
//        new_image.setPixel(iter.key(), qRgb(val,val,val));
    }



    return final;

}

QImage HoughTransform::transform2(QImage *image)
{
int radius = QInputDialog::getInteger(0, "Radius", "Radius (16):", 16, 1, 100);
QSize padding(radius,radius);
EdgeDetection e;
QImage new_image = Utility::blackImage(image->size() + padding + padding);
QImage binary_image(*image);
QList<QPoint> crossings = e.getZeroCrossings(image);

QHash<QPoint, qreal> accumulator1;
QHash<QPoint, qreal> accumulator2;
QList<QPoint> features;
QHash<QPoint, qreal> feature_sums;
QHash<QPoint, QPoint> gerig_max;
QList<QPoint> circle32 = getCircleTable(radius);


int max = 0;
//Loop through each feature
foreach ( QPoint p, crossings ) {
    //If feature is real then loop through target boundary
    if ( qGray(binary_image.pixel(p)) > 0 ) {
        //Save all the points this feature touches
        features.append(p);

        //Loop through target boundary
        foreach(QPoint i, circle32) {
            QPoint new_p = i + (p + QPoint(padding.width(), padding.height()));
            //Increment accumulator value
            qreal val = accumulator1.value(new_p, 0) + 1;
            accumulator1.insert(new_p, val);
            if ( val > max ) {
                max = val;
            }
        }
    }
}

QHashIterator<QPoint, qreal> iter(accumulator1);
//Threshold the accumulator
int accum_thresh = QInputDialog::getInteger(0, "Threshold", "Accumulator Threshold value:", 10, 0, 255);
while( iter.hasNext() ) {
    iter.next();
    int val = iter.value();
    QPoint p = iter.key();
    qreal circle_size = circle32.size();


    if ( val >= accum_thresh ) {
        qreal weight_x = 1;
        qreal weight_y = 1;
        qreal threshold = 0;
        qreal diff_x = 0;
        qreal diff_y = 0;
        bool edge = false;
        if ( p.x() - padding.width() < 0 ) {
            diff_x = qAbs(p.x() - padding.width());
            edge = true;
        } else if ( p.x() + padding.width() > new_image.width() - 1 ) {
            diff_x = qAbs(p.x() + padding.width() - (new_image.width() - 1));
            edge = true;
        }

        if ( p.y() - padding.height() < 0 ) {
            diff_y = qAbs(p.y() - padding.height());
            edge = true;
        } else if ( p.y() + padding.height() > new_image.height() - 1 ) {
            diff_y = qAbs(p.y() + padding.height() - (new_image.height() - 1));
            edge = true;
        }

        if ( edge ) {
            weight_x = qSqrt(qPow(radius,2) - qPow(diff_x,2)) / circle_size;
            weight_y = qSqrt(qPow(radius,2) - qPow(diff_y,2)) / circle_size;
            threshold = circle_size * weight_x * weight_y;
            if ( val < threshold ) {
                accumulator1.insert(p, 0);
            }
        }
        //Do nothing for normal positions

    } else {
        accumulator1.insert(p, 0);
    }

//        qreal circle_size = circle32.size();
//        bool edge = false;
//        qreal weight_x = 1;
//        if ( p.x() < radius-1 ) {
//            //How much of the circle is visible?
//            int diff = radius + 1 - p.x();
//            weight_x = (qSqrt(qPow(radius,2) - qPow(diff,2)) * 2) / circle_size;  //Double the height
//            edge = true;
//        } else if ( p.x() > new_image.width() - radius ) {
//            int diff = new_image.width() - radius + p.x();
//            weight_x = (qSqrt(qPow(radius,2) - qPow(diff,2)) * 2) / circle_size;  //Double the height
//            edge = true;
//        }

//        qreal weight_y = 1;
//        if ( p.y() < radius - 1 ) {
//            int diff = radius - p.y();
//            weight_y = (qSqrt(qPow(radius,2) - qPow(diff,2)) * 2) / circle_size;  //Double the height
//            edge = true;
//        } else if ( p.y() > new_image.height() - radius + p.y() ) {
//            int diff = new_image.height() - radius + p.y();
//            weight_y = (qSqrt(qPow(radius,2) - qPow(diff,2)) * 2) / circle_size;  //Double the height
//            edge = true;
//        }

//        if ( edge ) {
//            qreal threshold = circle_size * weight_x * weight_y;
//            if ( val < threshold ) {
//                accumulator1.insert(p, 0);
//            }
//        } else {
//            if ( val < accum_thresh ) {
//                accumulator1.insert(p, 0);
//            }
//        }

}

//Blur the accumulator

//Gerig Solution
max = 0;
qDebug() << "Gerig Solution";
foreach(QPoint p, features) {
    qreal max = 0;
    QPoint max_point;
    foreach(QPoint i, circle32) {
        QPoint new_p = i + (p + QPoint(padding.width(), padding.height()));
        //Increment accumulator value
        qreal val = accumulator1.value(new_p, 0);
        if ( val > max ) {
            max = val;
            max_point = new_p;
        }
    }

    //Save the max point, if no max then dont count it as a feature
    if ( max > 0 ) {
        gerig_max.insert(p, max_point);
    } else {
        int index = features.indexOf(p);
        features.removeAt(index);
    }
}

foreach(QPoint p, features) {
    QPoint mp = gerig_max.value(p);
    qreal val = accumulator2.value(mp, 0) + 1;
    if ( val > max ) {
        max = val;
    }
    accumulator2.insert(mp, val);
}

accumulator1.clear();
accumulator1 = accumulator2;
accumulator2.clear();

//    //Variation Method
//    int iter_count = QInputDialog::getInteger(0, "Iteration Count", "Iteraction Count value:", 10, 0, 1000);
//    while( iter_count > 0 ) {
//        max = 0;
//        //Sum up the feature position values
//        foreach(QPoint p, features) {
//            qreal sum = 0;
//            foreach(QPoint i, circle32) {
//                QPoint new_p = i + (p + QPoint(padding.width(), padding.height()));
//                //Increment accumulator value
//                sum += accumulator1.value(new_p, 0);
//            }
//            feature_sums.insert(p, sum);
//        }

//        qDebug() << "Setting Second Accumulator";
//        //Set new accumulator value
//        foreach(QPoint p, features) {
//            qreal sum = feature_sums.value(p, 1);
////            qDebug() << p << " sum =" << sum;
//            foreach(QPoint i, circle32) {
//                QPoint new_p = i + (p + QPoint(padding.width(), padding.height()));
//                //Increment accumulator value
//                qreal weight = accumulator1.value(new_p, 0) / sum;
//                qreal new_val = accumulator2.value(new_p, 0) + weight;
//                accumulator2.insert(new_p, new_val);
//                if ( new_val > max )
//                    max = new_val;
//            }
//        }

//        //Set accumulator 1 = accumulator 2 for next iteration
//        accumulator1.clear();
//        accumulator1 = accumulator2;
//        accumulator2.clear();

//        //decrement loop counter
//        iter_count--;
//    }


iter = QHashIterator<QPoint, qreal>(accumulator1);
while( iter.hasNext() ) {
    iter.next();
    qreal val = Utility::rangeConvert(max, 0, 255, 0, iter.value());
//        if ( val > 100 ) {
//            foreach(QPoint i, circle32) {
//                QPoint new_p = i + iter.key();
//                new_image.setPixel(new_p, qRgb(255,255,255));
//            }
//        }
    new_image.setPixel(iter.key(), qRgb(val,val,val));
}



return new_image;

}

QList<QPoint> HoughTransform::getCircleTable(int radius)
{
    QList<QPoint> kernel;
    int f = 1 - radius;
    int ddF_x = 1;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;

    kernel.append(QPoint(0, radius));
    kernel.append(QPoint(0, -radius));
    kernel.append(QPoint(radius, 0));
    kernel.append(QPoint(-radius, 0));

    while ( x < y ) {
        if ( f >= 0 ) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }

        x++;
        ddF_x += 2;
        f += ddF_x;
        kernel.append(QPoint(x, y));
        kernel.append(QPoint(-x, y));
        kernel.append(QPoint(x, -y));
        kernel.append(QPoint(-x, -y));
        kernel.append(QPoint(y, x));
        kernel.append(QPoint(-y, x));
        kernel.append(QPoint(y, -x));
        kernel.append(QPoint(-y, -x));
    }

    return kernel;
}
