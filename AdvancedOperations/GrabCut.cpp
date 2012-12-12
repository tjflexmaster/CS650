#include "GrabCut.h"

#include <QImage>
#include <QPoint>
#include <QList>
#include "GraphCut.h"
#include <QRectF>
#include <QRect>
#include "Utility.h"

GrabCut::GrabCut()
{
    gcut = new GraphCut();
    _unknown = new QList<QPoint>();
    _background = new QList<QPoint>();
    _foreground = new QList<QPoint>();
}

QImage GrabCut::getGrabCutImage(QImage *image, QRectF initial_rect)
{
    QImage new_image = Utility::blackImage(image->size());
    _image = image;

    QRect rect = initial_rect.toRect();
    //Initialize BG and unknowns
    for ( int y=0; y < image->height(); y++ ) {
        for ( int x=0; x < image->width(); x++ ) {
            QPoint p(x,y);
            QPoint temp(x - (image->width()/2), y - (image->height()/2));
            if ( !rect.contains(temp) ) {
                _background->append(p);
            } else {
                _unknown->append(p);
            }
        }
    }

    //Pick some pixels from _unknown to be our original foreground seeds
    QPoint seed = rect.center();
    for( int i=-1; i < 2; i++ ) {
        for ( int j=-1; j < 2; j++ ) {
            QPoint p(seed.x() + (image->width()/2) + j, seed.y() + (image->height()/2) + i);
            if (  !_unknown->isEmpty() ) {
                int index = _unknown->indexOf(p);
                if ( index >= 0 ) {
                    _foreground->append(p);
                    _unknown->removeAt(index);
                }
            }
        }
    }

    //Run Graph Cut
    int count = 0;
    while( count < 100 ) {
        int bg_changes = 0;
        GraphType *graph = new GraphType(image->height() * image->width(), image->height() * image->width() * 4);
        gcut->graphCutImageAlgorithm(graph, image, _foreground, _background);
        foreach(QPoint p, *_unknown) {
            int index = p.y() * image->width() + p.x();
            if ( graph->what_segment(index) == GraphType::SINK && _background->indexOf(p) < 0 ) {
                int uindex = _unknown->indexOf(p);
                _background->append(p);
                _unknown->removeAt(uindex);
                bg_changes++;
            }
        }

        delete graph;
        qDebug() << "BG changes: " << bg_changes << " iteration " << count;
        count++;

        //We converged lets leave
        if ( bg_changes < 5 ) {
            break;
        }
    }

    foreach(QPoint p, *_unknown) {
        new_image.setPixel(p, image->pixel(p));
    }

    foreach(QPoint p, *_foreground) {
        new_image.setPixel(p, image->pixel(p));
    }

    return new_image;
}
