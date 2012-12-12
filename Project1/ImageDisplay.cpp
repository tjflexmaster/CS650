#include "ImageDisplay.h"
#include <QMouseEvent>
#include <QLabel>
#include <QPainter>

ImageDisplay::ImageDisplay(QWidget *parent) :
    QLabel(parent)
{
}

void ImageDisplay::mousePressEvent(QMouseEvent *ev)
{
    start = ev->pos();
    mouse_down = true;
    ev->accept();
    emit mousePress(ev);
}

void ImageDisplay::mouseMoveEvent(QMouseEvent *ev)
{
    if ( mouse_down ) {
        current = ev->pos();
        this->update();
    }

    ev->accept();
    emit mouseMove(ev);

}

void ImageDisplay::mouseReleaseEvent(QMouseEvent *ev)
{
    mouse_down = false;

    ev->accept();
    emit mouseRelease(ev);
}
