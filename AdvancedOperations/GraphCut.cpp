#include "GraphCut.h"
#include <QList>
#include <QImage>
#include <QColor>
#include <qmath.h>
#include "Utility.h"
#include <QDebug>
#include <QInputDialog>
#include "Histogram.h"
#include <QMap>
#include "stdlib.h"
#include <cmath>
#include <limits>
#include <QThreadPool>
#include <GaussianCalc.h>
#include <QFutureWatcher>

GraphCut::GraphCut()
{
    color_weight = 1;

    _red_sigma = 1.0;
    _green_sigma = 1.0;
    _blue_sigma = 1.0;
    _lambda = 1.0;

    //Setup TLink values
    _sigma = 3;
    _a = 1 / _sigma * qSqrt( 2 * Utility::PI );
    _c = 2 * qPow(_sigma, 2);
    _threshold = 0.0001;
}

void GraphCut::graphCutImageAlgorithm(GraphType *graph, QImage *image, QList<QPoint> *foreground, QList<QPoint> *background)
{
    _image = image;
    _foreground = foreground;
    _background = background;

    //Create maxflow graph
    for ( int y=0; y < image->height(); y++ ) {
        for ( int x=0; x < image->width(); x++ ) {
            graph->add_node();
        }
    }

    for ( int y=0; y < image->height(); y++ ) {
        for ( int x=0; x < image->width(); x++ ) {
            //Node value
            int index = y * image->width() + x;
            QPoint current_point(x,y);

            //Calculate right n link value
            if ( (x+1) < image->width() ) {
                double n_val = getNLinkValue(image->pixel(current_point), image->pixel(QPoint(x+1,y)));
                graph->add_edge(index, index + 1, n_val, n_val);
            }

            //Calculate bot n link value
            if ( (y+1) < image->height() ) {
                double n_val = getNLinkValue(image->pixel(current_point), image->pixel(QPoint(x,y+1)));
                graph->add_edge(index, (index + image->width()), n_val, n_val);
            }

            //Calculate t link values
            double f_tlink = 0;
            double b_tlink = 0;
            if ( _foreground->indexOf(current_point) >= 0 ) {
                f_tlink = 2;
            } else if ( _background->indexOf(current_point) >= 0) {
                b_tlink = 2;
            } else {
                f_tlink = getTLinkForeground(current_point);
                b_tlink = getTLinkBackground(current_point);
            }

//            qDebug() << QString("FG: %1  BG: %2").arg(f_tlink).arg(b_tlink);

            graph->add_tweights(index, f_tlink, b_tlink);
        }
    }

    int flow = graph->maxflow();
    qDebug() << "Flow: " << flow;
}

QImage GraphCut::graphCutImage(QImage *image, QList<QPoint> *foreground, QList<QPoint> *background)
{
    QImage new_image = Utility::blackImage(image->size());

    _graph = new GraphType(image->height() * image->width(), image->height() * image->width() * 4);

    _sigma = QInputDialog::getDouble(0, "Sigma", "Choose variance value:", 10, 0.1, 200);
    _c = 2 * qPow(_sigma, 2);

    graphCutImageAlgorithm(_graph, image, foreground, background);

    for ( int y=0; y < image->height(); y++ ) {
        for ( int x=0; x < image->width(); x++ ) {
            //Node value
            int index = y * image->width() + x;
            if ( _graph->what_segment(index) == GraphType::SOURCE ) {
                new_image.setPixel(QPoint(x,y), image->pixel(QPoint(x,y)));
            }
        }
    }

    return new_image;
}

double GraphCut::getNLinkValue(QRgb seed, QRgb pixel)
{
    double r_diff = (double) qRed(seed) - qRed(pixel);
    double g_diff = (double) qGreen(seed) - qGreen(pixel);
    double b_diff = (double) qBlue(seed) - qBlue(pixel);
    double r_energy = qExp( -(qPow((r_diff), 2) / _c));
    double g_energy = qExp( -(qPow((g_diff), 2) / _c));
    double b_energy = qExp( -(qPow((b_diff), 2) / _c));

    return (r_energy + g_energy + b_energy) / 3;

//    double energy = qExp( -( qPow(qGray(seed) - qGray(pixel), 2) / 50 ) );
//    return energy;
}

double GraphCut::getTLinkForeground(QPoint pixel)
{
    double prob = 0;
    foreach(QPoint p, *_foreground) {
        prob += getTLinkValue(pixel, p);
    }
    return prob / (double) _foreground->count();
}

double GraphCut::getTLinkBackground(QPoint pixel)
{
    double prob = 0;
    foreach(QPoint p, *_background) {
        prob += getTLinkValue(pixel, p);
    }
    return prob / (double) _background->count();
}

double GraphCut::getTLinkValue(QPoint pixel, QPoint seed)
{

    //Make this color!!!
    QRgb color1 = _image->pixel(pixel);
    QRgb color2 = _image->pixel(seed);

    double r_diff = (double) qRed(color1) - qRed(color2);
    double g_diff = (double) qGreen(color1) - qGreen(color2);
    double b_diff = (double) qBlue(color1) - qBlue(color2);

    double r_energy = qExp( -(qPow((r_diff), 2) / _c));
    double g_energy = qExp( -(qPow((g_diff), 2) / _c));
    double b_energy = qExp( -(qPow((b_diff), 2) / _c));

    return (thresholdValue(r_energy) +
            thresholdValue(g_energy) +
            thresholdValue(b_energy)) / 3;
}

double GraphCut::thresholdValue(double value)
{
    if ( value < _threshold )
        return 0.0;
    else
        return value;
}
