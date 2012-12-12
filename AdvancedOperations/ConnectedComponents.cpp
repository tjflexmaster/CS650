#include "ConnectedComponents.h"
#include "Utility.h"
#include <QInputDialog>
#include <QRect>
#include <QPoint>
#include <QMap>
#include <QHash>
#include <QList>
#include <QRgb>
#include <QImage>
#include <QDebug>
#include <QColor>
#include <QMultiMap>
#include <QSet>
#include <QSharedPointer>
#include "EdgeDetection.h"

ConnectedComponents::ConnectedComponents()
{
    label_count = 0;

    label_colors.insert(-1, QColor(Qt::white).rgb());
    label_colors.insert(0, QColor(Qt::darkBlue).rgb());
    label_colors.insert(1, QColor(Qt::cyan).rgb());
    label_colors.insert(2, QColor(Qt::darkCyan).rgb());
    label_colors.insert(3, QColor(Qt::red).rgb());
    label_colors.insert(4, QColor(Qt::darkRed).rgb());
    label_colors.insert(5, QColor(Qt::magenta).rgb());
    label_colors.insert(6, QColor(Qt::darkMagenta).rgb());
    label_colors.insert(7, QColor(Qt::green).rgb());
    label_colors.insert(8, QColor(Qt::darkGreen).rgb());
    label_colors.insert(9, QColor(Qt::yellow).rgb());
    label_colors.insert(10, QColor(Qt::darkYellow).rgb());
    label_colors.insert(11, QColor(Qt::blue).rgb());
    label_colors.insert(12, QColor(Qt::darkBlue).rgb());
    label_colors.insert(13, QColor(Qt::gray).rgb());
    label_colors.insert(14, QColor(Qt::darkGray).rgb());

    stack = new QList<QPoint>();
    pixel_labels = new QHash<QPoint, int>();
    label_sets = QSharedPointer<QMultiMap<int, QPoint> >( new QMultiMap<int, QPoint>() ) ;
}

ConnectedComponents::~ConnectedComponents()
{

    delete stack;
    delete pixel_labels;
    label_sets.clear();
}

QImage ConnectedComponents::connectComponents(QImage *image)
{
    _image = image;
    QImage new_image = Utility::blackImage(image->size());

//    qreal threshold = QInputDialog::getDouble(0, "Threshold", "Select a value between 0 and 1:", 0.9, 0.0, 1.0,2);
    threshold = QInputDialog::getDouble(0, "Threshold", "Select a color value:", 5, 0.0, 255,2);

    QRgb bg_color = image->pixel(QPoint(0,0));
    //First Pass
    for(int y=0; y < image->height(); y++) {
        for ( int x=0; x < image->width(); x++) {
            QPoint p(x, y);

            //Flood fill, if we have a label then skip
            if ( pixel_labels->value(p, -1) >= 0 || Utility::colorSimilarity(bg_color, image->pixel(p)) < threshold ) {
                continue;
            }

            //Create a new label
            int label = label_count;
            label_count++;


            label_sets->insert(label, p);
            pixel_labels->insert(p, label);
            stack->append(p);

            //recursively fill neighbors
            while ( !stack->isEmpty() ) {
//                qDebug() << "Stack size: " << stack->count();
                QPoint next = stack->first();
                stack->removeFirst();
                fillNeighbors(label, next);
            }

            qDebug() << "Label:" << label << " Label Count:" << label_sets->values(label).count();
        }
    }

    for ( int i=0; i < label_count; i++ ) {
        int index = (i % 12);
        QRgb rgb = label_colors.value(index, QColor(Qt::white).rgb());
        QList<QPoint> points = label_sets->values(i);
        foreach(QPoint p, points) {
            new_image.setPixel(p, rgb);
        }
    }

    return new_image;
}

void ConnectedComponents::fillNeighbors(int label, QPoint point)
{
    //Get Neighbors
    QList<QPoint> neighbors;
    neighbors.append(QPoint(point.x(), point.y()-1));
    neighbors.append(QPoint(point.x()+1, point.y()));
    neighbors.append(QPoint(point.x(), point.y()+1));
    neighbors.append(QPoint(point.x()-1, point.y()));

    foreach(QPoint neighbor, neighbors) {
        if ( _image->valid(neighbor) &&
             Utility::colorSimilarity(_image->pixel(point), _image->pixel(neighbor)) < threshold ) {
            //if it has a label then ignore it
            if ( pixel_labels->value(neighbor, -1) >= 0 ) {
                continue;
            } else {
                pixel_labels->insert(neighbor, label);
                if ( !label_sets->contains(label, neighbor) )
                    label_sets->insert(label, neighbor);
                stack->append(neighbor);
            }
        }
    }
}

QSharedPointer<QMultiMap<int, QPoint> > ConnectedComponents::getLabelSets()
{
    return QSharedPointer<QMultiMap<int, QPoint> >(label_sets);
}
