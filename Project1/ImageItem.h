#ifndef IMAGEITEM_H
#define IMAGEITEM_H

#include <QGraphicsItem>
#include <QPointF>
#include <QList>
#include <QGraphicsSceneMouseEvent>

class ImageItem : public QGraphicsItem
{
public:
    ImageItem(QPixmap pixmap);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF getRectF();
    QList<QPoint> getMarkers();
    void clearMarkers();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    QPixmap _pixmap;

    bool _dragging;
    bool _mouseDown;
    bool _deleteMode;
    QPointF _start;
    QPointF _current;
    QList<QPoint> _markers;
};

#endif // IMAGEITEM_H
