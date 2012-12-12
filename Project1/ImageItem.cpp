#include "ImageItem.h"
#include <QPixmap>
#include <QPointF>
#include <QPainter>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>

ImageItem::ImageItem(QPixmap pixmap)
{
    _pixmap = pixmap;
    setToolTip("Click and Drag to Select a Box Region");
    setAcceptedMouseButtons(Qt::LeftButton);
}

QRectF ImageItem::boundingRect() const
{
    return QRectF(-(_pixmap.width()/2), -(_pixmap.height()/2), _pixmap.width(), _pixmap.height() );
}

void ImageItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->drawPixmap(QRect(-(_pixmap.width()/2), -(_pixmap.height()/2), _pixmap.width(), _pixmap.height()), _pixmap);
    if ( _current != _start ) {
        painter->setPen(Qt::green);
        painter->drawRect(QRectF(_start, _current));
    }

    if ( _markers.count() > 0 ) {
        foreach(QPoint p, _markers) {
            painter->drawEllipse(p, 3, 3);
        }
    }
}

QRectF ImageItem::getRectF()
{
    return QRectF(_start, _current);
}

QList<QPoint> ImageItem::getMarkers()
{
    return _markers;
}

void ImageItem::clearMarkers()
{
    _markers.clear();
    update();
}

void ImageItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if ( event->button() == Qt::LeftButton ) {
        _mouseDown = true;
        _start = event->pos();
        _current = event->pos();
    }
}

void ImageItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QPoint click = event->pos().toPoint();
//    QRectF bound(click.x() - 2, click.y() - 2, click.x() + 2, click.y() + 2);
    QList<int> removed_markers;
    if ( _markers.count() > 0 ) {
        foreach(QPoint p, _markers) {
//            if ( bound.contains(p) ) {
            if ( (p - click).manhattanLength() < 2.0 ) {
                removed_markers.append(_markers.indexOf(p));
            }
        }
    }

    foreach(int i, removed_markers) {
        _markers.removeAt(i);
    }
    update();
}

void ImageItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if ( _mouseDown ) {
        _dragging = true;
        _mouseDown = false;
    }

    if ( _dragging ) {
        _current = event->pos();
        update();
    }
}

void ImageItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if ( _mouseDown ) {
        _markers.append(event->pos().toPoint());
        update();
        _mouseDown = false;
    } else if ( _dragging ) {
        _current = event->pos();
        update();
        _dragging = false;
    }
}
